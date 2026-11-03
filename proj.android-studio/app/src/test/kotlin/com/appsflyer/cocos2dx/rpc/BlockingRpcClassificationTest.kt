package com.appsflyer.cocos2dx.rpc

import org.json.JSONObject
import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Test

class BlockingRpcClassificationTest {

    @Test
    fun initIsBlocking() {
        assertTrue(isBlockingRpc("init", JSONObject()))
    }

    @Test
    fun startRespectsAwaitResponse() {
        assertFalse(isBlockingRpc("start", JSONObject().put("awaitResponse", false)))
        assertTrue(isBlockingRpc("start", JSONObject().put("awaitResponse", true)))
    }

    @Test
    fun logEventFireAndForgetIsFastLane() {
        assertFalse(isBlockingRpc("logEvent", JSONObject().put("awaitResponse", false)))
    }
}
