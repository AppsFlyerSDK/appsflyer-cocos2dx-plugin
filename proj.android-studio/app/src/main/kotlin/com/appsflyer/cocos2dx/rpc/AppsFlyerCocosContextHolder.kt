package com.appsflyer.cocos2dx.rpc

import android.app.Activity
import android.content.Context

/**
 * Process-scoped Activity / application context for [AppsFlyerRpcHandler].
 */
internal object AppsFlyerCocosContextHolder {

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
