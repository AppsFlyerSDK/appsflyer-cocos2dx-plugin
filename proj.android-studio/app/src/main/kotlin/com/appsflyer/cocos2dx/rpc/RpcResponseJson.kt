package com.appsflyer.cocos2dx.rpc

import com.appsflyer.pluginbridge.model.RpcResponse
import org.json.JSONObject

internal fun normalizeRpcResponse(response: RpcResponse): String = when (response) {
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

internal fun normalizeRpcError(code: Int, message: String): String = JSONObject()
    .put("success", false)
    .put("error", JSONObject().put("code", code).put("message", message))
    .toString()

internal fun buildRpcRequest(method: String, params: JSONObject): String =
    JSONObject().put("method", method).put("params", params).toString()

internal fun isBlockingRpc(method: String, params: JSONObject): Boolean = when (method) {
    RPC_METHOD_INIT -> true
    "validateAndLogInAppPurchase" -> true
    "start", "logEvent" -> params.optBoolean(RPC_PARAM_AWAIT_RESPONSE, false)
    "generateInviteLink" -> params.optBoolean(RPC_PARAM_AWAIT_RESPONSE, true)
    else -> false
}
