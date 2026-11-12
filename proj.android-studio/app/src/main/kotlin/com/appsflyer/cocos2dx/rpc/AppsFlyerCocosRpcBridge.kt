package com.appsflyer.cocos2dx.rpc

import android.app.Activity
import android.content.Context
import android.os.Handler
import android.os.Looper
import android.util.Log
import com.appsflyer.AppsFlyerLib
import com.appsflyer.pluginbridge.handler.AppsFlyerRpcHandler
import com.appsflyer.pluginbridge.model.RpcResponse
import com.appsflyer.pluginbridge.parser.JsonRpcRequestParser
import org.json.JSONObject
import java.util.ArrayDeque
import java.util.concurrent.CountDownLatch
import java.util.concurrent.Executors
import java.util.concurrent.RejectedExecutionException
import java.util.concurrent.TimeUnit
import java.util.concurrent.atomic.AtomicReference

private const val AF_PLUGIN_TAG = "AppsFlyer_Cocos2dX_RPC"
private const val RPC_METHOD_INIT = "init"
private const val RPC_PARAM_AWAIT_RESPONSE = "awaitResponse"
private const val AWAIT_RESPONSE_TIMEOUT_MS = 30_000L
private const val RPC_EVENT_BUFFER_MAX = 64

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

/** Process-scoped Activity / application context for [AppsFlyerRpcHandler]. */
private object AppsFlyerCocosContextHolder {

    private val lock = Any()

    @Volatile
    private var appContext: Context? = null

    private val attachments = LinkedHashMap<Any, Activity>()

    fun current(): Context = synchronized(lock) {
        attachments.values.lastOrNull()
            ?: appContext
            ?: throw IllegalStateException("AppsFlyer Cocos RPC: no context (call attach first)")
    }

    fun hasContext(): Boolean = synchronized(lock) {
        attachments.isNotEmpty() || appContext != null
    }

    fun setAppContext(context: Context) {
        appContext = context.applicationContext
    }

    fun attachActivity(owner: Any, activity: Activity) {
        synchronized(lock) {
            attachments.remove(owner)
            attachments[owner] = activity
        }
    }

    fun detachActivity(owner: Any) {
        synchronized(lock) {
            attachments.remove(owner)
        }
    }
}

/** Buffers RPC events until a JNI sink is attached; delivers on the main looper. */
private object AppsFlyerCocosEventBus {

    private val lock = Any()
    private val buffer = ArrayDeque<String>()
    private val mainHandler = Handler(Looper.getMainLooper())

    @Volatile
    private var deliverToNative: ((String) -> Unit)? = null

    fun attach(deliver: (String) -> Unit) {
        synchronized(lock) {
            deliverToNative = deliver
            while (buffer.isNotEmpty()) {
                val event = buffer.removeFirst()
                postDeliver(event)
            }
        }
    }

    fun publish(eventJson: String) {
        mainHandler.post {
            synchronized(lock) {
                val deliver = deliverToNative
                if (deliver == null) {
                    if (buffer.size >= RPC_EVENT_BUFFER_MAX) {
                        buffer.removeFirst()
                        Log.w(AF_PLUGIN_TAG, "RPC event buffer overflow; dropped oldest event")
                    }
                    buffer.addLast(eventJson)
                    return@post
                }
                deliver(eventJson)
            }
        }
    }

    private fun postDeliver(eventJson: String) {
        synchronized(lock) {
            deliverToNative?.invoke(eventJson)
        }
    }
}

private fun normalizeRpcResponse(response: RpcResponse): String = when (response) {
    is RpcResponse.Success<*> -> JSONObject()
        .put("success", true)
        .put("data", JSONObject.wrap(response.result) ?: JSONObject.NULL)
        .toString()
    is RpcResponse.VoidSuccess -> JSONObject()
        .put("success", true)
        .put("data", JSONObject.NULL)
        .toString()
    is RpcResponse.Error -> normalizeRpcError(response.code, response.message)
}

private fun normalizeRpcError(code: Int, message: String): String = JSONObject()
    .put("success", false)
    .put("error", JSONObject().put("code", code).put("message", message))
    .toString()

private fun buildRpcRequest(method: String, params: JSONObject): String =
    JSONObject().put("method", method).put("params", params).toString()

internal fun isBlockingRpc(method: String, params: JSONObject): Boolean = when (method) {
    RPC_METHOD_INIT -> true
    "validateAndLogInAppPurchase" -> true
    "start", "logEvent" -> params.optBoolean(RPC_PARAM_AWAIT_RESPONSE, false)
    "generateInviteLink" -> params.optBoolean(RPC_PARAM_AWAIT_RESPONSE, true)
    else -> false
}
