package com.appsflyer.cocos2dx.rpc

import android.app.Activity
import android.util.Log
import com.appsflyer.AppsFlyerLib
import com.appsflyer.pluginbridge.handler.AppsFlyerRpcHandler
import com.appsflyer.pluginbridge.model.RpcResponse
import com.appsflyer.pluginbridge.parser.JsonRpcRequestParser
import org.json.JSONObject
import java.util.concurrent.CountDownLatch
import java.util.concurrent.Executors
import java.util.concurrent.RejectedExecutionException
import java.util.concurrent.TimeUnit
import java.util.concurrent.atomic.AtomicReference

/**
 * JNI-facing RPC entry for the cocos2d-x C++ layer.
 */
object AppsFlyerCocosRpcBridge {

    private val rpcExecutor = Executors.newSingleThreadExecutor()
    private val blockingRpcExecutor = Executors.newSingleThreadExecutor()

    private val handlerLock = Any()
    private var rpcHandler: AppsFlyerRpcHandler? = null

    private val activityOwner = Any()

    init {
        AppsFlyerCocosEventBus.attach { eventJson ->
            nativeDeliverRpcEvent(eventJson)
        }
    }

    @JvmStatic
    fun onActivityCreated(activity: Activity) {
        AppsFlyerCocosContextHolder.setAppContext(activity)
        AppsFlyerCocosContextHolder.attachActivity(activityOwner, activity)
    }

    @JvmStatic
    fun onActivityDestroyed() {
        AppsFlyerCocosContextHolder.detachActivity(activityOwner)
    }

    /**
     * Blocks the caller until the RPC completes (POC). Do not call from the Android main thread
     * for slow / blocking RPCs.
     */
    @JvmStatic
    fun executeRpcBlocking(requestJson: String): String {
        val request = JSONObject(requestJson)
        val method = request.getString("method")
        val params = request.optJSONObject("params") ?: JSONObject()
        val executor = if (isBlockingRpc(method, params)) blockingRpcExecutor else rpcExecutor
        val resultRef = AtomicReference<String>()
        val errorRef = AtomicReference<Throwable>()
        val latch = CountDownLatch(1)
        try {
            executor.execute {
                try {
                    resultRef.set(dispatchRequest(method, params))
                } catch (t: Throwable) {
                    errorRef.set(t)
                } finally {
                    latch.countDown()
                }
            }
        } catch (e: RejectedExecutionException) {
            return normalizeRpcError(503, "RPC executor shut down")
        }
        if (!latch.await(AWAIT_RESPONSE_TIMEOUT_MS, TimeUnit.MILLISECONDS)) {
            return normalizeRpcError(504, "RPC timed out waiting for native handler")
        }
        errorRef.get()?.let {
            Log.e(AF_PLUGIN_TAG, "executeRpcBlocking failed", it)
            return normalizeRpcError(500, it.message ?: "Unexpected RPC failure")
        }
        return resultRef.get() ?: normalizeRpcError(500, "Empty RPC result")
    }

    private fun dispatchRequest(method: String, params: JSONObject): String =
        executeRpcSync(method, params)

    private fun executeRpcSync(method: String, params: JSONObject): String {
        if (!AppsFlyerCocosContextHolder.hasContext()) {
            return normalizeRpcError(
                503,
                "AppsFlyer RPC context not ready (attach Activity before calling SDK)",
            )
        }
        return try {
            val response = handler().execute(buildRpcRequest(method, params))
            normalizeRpcResponse(response)
        } catch (e: Exception) {
            Log.e(AF_PLUGIN_TAG, "RPC dispatch error for $method", e)
            normalizeRpcError(500, e.message ?: "Native RPC failure")
        }
    }

    private fun handler(): AppsFlyerRpcHandler = synchronized(handlerLock) {
        rpcHandler ?: AppsFlyerRpcHandler(
            contextProvider = AppsFlyerCocosContextHolder::current,
            pluginNotifier = { raw -> AppsFlyerCocosEventBus.publish(raw) },
            appsFlyerLib = AppsFlyerLib.getInstance(),
            requestParser = JsonRpcRequestParser(),
        ).also { rpcHandler = it }
    }

    @JvmStatic
    private external fun nativeDeliverRpcEvent(eventJson: String)
}
