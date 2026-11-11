#include "AppsFlyerRpcClient.h"

#include "AppsFlyerCocosThread.h"

#include "cocos2d.h"

#include <mutex>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "AppsFlyerCocosRpcBridgePlatform.h"
#endif

namespace appsflyer {
namespace rpc {

namespace {

std::mutex g_eventHandlerMutex;
RpcEventHandler g_eventHandler;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

std::string jstringToStd(JNIEnv* env, jstring value) {
    if (value == nullptr) {
        return {};
    }
    const char* utf = env->GetStringUTFChars(value, nullptr);
    std::string out(utf ? utf : "");
    if (utf) {
        env->ReleaseStringUTFChars(value, utf);
    }
    return out;
}

#endif

} // namespace

void setRpcEventHandler(RpcEventHandler handler) {
    std::lock_guard<std::mutex> lock(g_eventHandlerMutex);
    g_eventHandler = std::move(handler);
}

void onNativeRpcEventFromBridge(const std::string& eventJson) {
    RpcEventHandler handler;
    {
        std::lock_guard<std::mutex> lock(g_eventHandlerMutex);
        handler = g_eventHandler;
    }
    if (!handler) {
        return;
    }
    cocos::runOnGameThread([handler, eventJson]() { handler(eventJson); });
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

void onNativeRpcEventFromJni(const std::string& eventJson) {
    onNativeRpcEventFromBridge(eventJson);
}

std::string buildRequestJson(const std::string& method, const std::string& paramsJsonObject) {
    return std::string("{\"method\":\"") + method + "\",\"params\":" +
           (paramsJsonObject.empty() ? "{}" : paramsJsonObject) + "}";
}

RpcResult executeBlocking(const std::string& method, const std::string& paramsJsonObject) {
    RpcResult result;
    cocos2d::JniMethodInfo methodInfo;
    if (!cocos2d::JniHelper::getStaticMethodInfo(
            methodInfo,
            "com/appsflyer/cocos2dx/rpc/AppsFlyerCocosRpcBridge",
            "executeRpcBlocking",
            "(Ljava/lang/String;)Ljava/lang/String;")) {
        result.success = false;
        result.errorCode = 500;
        result.errorMessage = "AppsFlyerCocosRpcBridge JNI entry not found";
        return result;
    }

    const std::string requestJson = buildRequestJson(method, paramsJsonObject);
    jstring jRequest = methodInfo.env->NewStringUTF(requestJson.c_str());
    jstring jResponse = (jstring)methodInfo.env->CallStaticObjectMethod(
        methodInfo.classID, methodInfo.methodID, jRequest);
    methodInfo.env->DeleteLocalRef(jRequest);

    const std::string responseJson = jstringToStd(methodInfo.env, jResponse);
    if (jResponse) {
        methodInfo.env->DeleteLocalRef(jResponse);
    }

    // Minimal envelope parse (replace with proper JSON lib in Phase 2).
    result.success = responseJson.find("\"success\":true") != std::string::npos;
    if (!result.success) {
        result.errorCode = 500;
        result.errorMessage = responseJson;
    } else {
        result.dataJson = responseJson;
    }
    return result;
}

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

RpcResult executeBlocking(const std::string& method, const std::string& paramsJsonObject) {
    RpcResult result;
    const std::string requestJson = std::string("{\"method\":\"") + method + "\",\"params\":" +
                                    (paramsJsonObject.empty() ? "{}" : paramsJsonObject) + "}";
    const std::string responseJson = executeRpcBlockingRequest(requestJson);
    result.success = responseJson.find("\"success\":true") != std::string::npos;
    if (!result.success) {
        result.errorCode = 500;
        result.errorMessage = responseJson;
    } else {
        result.dataJson = responseJson;
    }
    return result;
}

#else

RpcResult executeBlocking(const std::string& /*method*/, const std::string& /*paramsJsonObject*/) {
    RpcResult result;
    result.success = false;
    result.errorCode = 501;
    result.errorMessage = "RPC supported on Android/iOS only";
    return result;
}

#endif

} // namespace rpc
} // namespace appsflyer

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

extern "C" JNIEXPORT void JNICALL
Java_com_appsflyer_cocos2dx_rpc_AppsFlyerCocosRpcBridge_nativeDeliverRpcEvent(
    JNIEnv* env,
    jclass /*clazz*/,
    jstring eventJson) {
    if (eventJson == nullptr) {
        return;
    }
    const char* utf = env->GetStringUTFChars(eventJson, nullptr);
    std::string payload(utf ? utf : "");
    if (utf) {
        env->ReleaseStringUTFChars(eventJson, utf);
    }
    appsflyer::rpc::onNativeRpcEventFromJni(payload);
}

#endif
