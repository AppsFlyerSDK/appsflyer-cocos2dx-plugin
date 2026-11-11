package com.appsflyer.cocos2dx.rpc

import android.os.Handler
import android.os.Looper
import android.util.Log
import java.util.ArrayDeque

/**
 * Buffers RPC events until a JNI sink is attached; delivers on the main looper.
 */
internal object AppsFlyerCocosEventBus {

    private const val MAX_BUFFER = 64
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

    fun detach() {
        synchronized(lock) {
            deliverToNative = null
        }
    }

    fun publish(eventJson: String) {
        mainHandler.post {
            synchronized(lock) {
                val deliver = deliverToNative
                if (deliver == null) {
                    if (buffer.size >= MAX_BUFFER) {
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
