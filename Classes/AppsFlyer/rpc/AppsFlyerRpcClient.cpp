#include "AppsFlyerRpcClient.h"

#include "AppsFlyer/AppsFlyerTaskState.h"
#include "AppsFlyerCocosThread.h"

#include "cocos2d.h"

#include <atomic>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <utility>

#if defined(AF_RPC_HEADLESS_TEST)

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "AppsFlyerCocosRpcBridgePlatform.h"
#endif

namespace appsflyer {
namespace detail {

namespace {

std::atomic<bool> g_useTestGameThread{false};
GameThreadPoster g_testGameThreadPoster;
std::atomic<bool> g_useTestRpc{false};
RpcExecutor g_testRpcExecutor;

} // namespace

bool rpcTestHookActive() {
    return g_useTestRpc.load() && static_cast<bool>(g_testRpcExecutor);
}

rpc::RpcResult invokeRpcTestHook(const std::string& method, const std::string& paramsJson) {
    return g_testRpcExecutor(method, paramsJson);
}

void postOnGameThread(const std::function<void()>& fn) {
    if (g_useTestGameThread.load() && g_testGameThreadPoster) {
        g_testGameThreadPoster(fn);
        return;
    }
    cocos::runOnGameThread(fn);
}

AppsFlyerResult toAppsFlyerResult(const rpc::RpcResult& rpc) {
    AppsFlyerResult result;
    result.success = rpc.success;
    result.dataJson = rpc.dataJson;
    if (!rpc.success) {
        result.hasError = true;
        result.error.code = rpc.errorCode != 0 ? rpc.errorCode : 500;
        result.error.message = rpc.errorMessage.empty() ? rpc.dataJson : rpc.errorMessage;
    }
    return result;
}

void postCompletionOnGameThread(const std::function<void()>& fn) { postOnGameThread(fn); }

void setGameThreadPosterForTests(GameThreadPoster poster) {
    g_testGameThreadPoster = std::move(poster);
    g_useTestGameThread.store(static_cast<bool>(g_testGameThreadPoster));
}

void setRpcExecutorForTests(RpcExecutor executor) {
    g_testRpcExecutor = std::move(executor);
    g_useTestRpc.store(static_cast<bool>(g_testRpcExecutor));
}

void resetTestHooks() {
    g_useTestGameThread.store(false);
    g_useTestRpc.store(false);
    g_testGameThreadPoster = {};
    g_testRpcExecutor = {};
}

AppsFlyerTask unsupportedPlatformTask(AppsFlyerCompletion onComplete) {
    AppsFlyerResult result;
    result.success = false;
    result.hasError = true;
    result.error.code = 501;
    result.error.message = "RPC supported on Android/iOS only";
    auto state = std::make_shared<AppsFlyerTaskState>();
    AppsFlyerTask task(state);
    if (onComplete) {
        task.whenComplete(std::move(onComplete));
    }
    postOnGameThread([state, result]() { state->finish(result); });
    return task;
}

rpc::RpcResult runRpc(const std::string& method, const std::string& paramsJson) {
    return rpc::executeBlocking(method, paramsJson);
}

} // namespace detail

namespace rpc {

namespace {

std::mutex g_eventHandlerMutex;
RpcEventHandler g_eventHandler;

#if !defined(AF_RPC_HEADLESS_TEST) && (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

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

#if defined(AF_RPC_HEADLESS_TEST)

RpcResult platformExecuteBlocking(const std::string&, const std::string&) {
    RpcResult result;
    result.success = false;
    result.errorMessage = "headless test stub";
    return result;
}

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

void onNativeRpcEventFromJni(const std::string& eventJson) {
    onNativeRpcEventFromBridge(eventJson);
}

std::string buildRequestJson(const std::string& method, const std::string& paramsJsonObject) {
    return std::string("{\"method\":\"") + method + "\",\"params\":" +
           (paramsJsonObject.empty() ? "{}" : paramsJsonObject) + "}";
}

RpcResult platformExecuteBlocking(const std::string& method, const std::string& paramsJsonObject) {
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

RpcResult platformExecuteBlocking(const std::string& method, const std::string& paramsJsonObject) {
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

RpcResult platformExecuteBlocking(const std::string& /*method*/, const std::string& /*paramsJsonObject*/) {
    RpcResult result;
    result.success = false;
    result.errorCode = 501;
    result.errorMessage = "RPC supported on Android/iOS only";
    return result;
}

#endif

RpcResult executeBlocking(const std::string& method, const std::string& paramsJsonObject) {
    if (detail::rpcTestHookActive()) {
        return detail::invokeRpcTestHook(method, paramsJsonObject);
    }
    return platformExecuteBlocking(method, paramsJsonObject);
}

bool isInitWireMethod(const std::string& wireMethod) {
    return wireMethod == "init" || wireMethod == "initialize";
}

constexpr const char* kSetPluginInfoParamsJson =
    R"({"plugin":"cocos2dx","pluginVersion":"7.0.0-poc","additionalParams":{}})";

} // namespace rpc

AppsFlyerTask executeRpc(const std::string& wireMethod, const std::string& paramsJsonObject,
                         AppsFlyerCompletion onComplete) {
    auto state = std::make_shared<AppsFlyerTaskState>();
    AppsFlyerTask task(state);
    if (onComplete) {
        task.whenComplete(std::move(onComplete));
    }

    std::thread([state, wireMethod, paramsJsonObject]() {
        if (rpc::isInitWireMethod(wireMethod)) {
            const rpc::RpcResult pluginInfo =
                detail::runRpc("setPluginInfo", rpc::kSetPluginInfoParamsJson);
            if (!pluginInfo.success) {
                const AppsFlyerResult failed = detail::toAppsFlyerResult(pluginInfo);
                detail::postOnGameThread([state, failed]() { state->finish(failed); });
                return;
            }
        }
        const rpc::RpcResult rpc = detail::runRpc(wireMethod, paramsJsonObject);
        const AppsFlyerResult result = detail::toAppsFlyerResult(rpc);
        detail::postOnGameThread([state, result]() { state->finish(result); });
    }).detach();

    return task;
}

AppsFlyerTask registerListenerRpc(const std::string& wireMethod,
                                  const std::string& paramsJsonObject,
                                  const std::function<void()>& installListener,
                                  const std::function<void()>& restoreListener,
                                  AppsFlyerCompletion onComplete) {
    installListener();

    auto state = std::make_shared<AppsFlyerTaskState>();
    AppsFlyerTask task(state);
    if (onComplete) {
        task.whenComplete(std::move(onComplete));
    }

    std::thread([state, wireMethod, paramsJsonObject, restoreListener]() {
        const rpc::RpcResult rpc = detail::runRpc(wireMethod, paramsJsonObject);
        const AppsFlyerResult result = detail::toAppsFlyerResult(rpc);
        detail::postOnGameThread([state, result, restoreListener]() {
            if (!result.success) {
                restoreListener();
            }
            state->finish(result);
        });
    }).detach();

    return task;
}

namespace internal {

constexpr const char* kEventSessionReady = "onSessionReady";
constexpr const char* kEventConversionSuccess = "onConversionDataSuccess";
constexpr const char* kEventConversionFail = "onConversionDataFail";
constexpr const char* kEventDeepLinking = "onDeepLinking";
constexpr const char* kEventDeepLinkReceived = "onDeepLinkReceived";

struct ListenerRegistry {
    std::mutex mutex;
    bool rpcEventHandlerInstalled = false;

    void (*onSessionReady)() = nullptr;
    void (*onConversionSuccess)(cocos2d::ValueMap) = nullptr;
    void (*onConversionFail)(cocos2d::ValueMap) = nullptr;
    void (*onDeepLinkingFn)(AppsFlyerXDeepLinkResult) = nullptr;
    std::function<void(AppsFlyerXDeepLinkResult)> onDeepLinkingHandler;
};

ListenerRegistry gListeners;

std::string extractJsonStringField(const std::string& json, const char* field) {
    const std::string key = std::string("\"") + field + "\":\"";
    const auto pos = json.find(key);
    if (pos == std::string::npos) {
        return {};
    }
    const auto start = pos + key.size();
    const auto end = json.find('"', start);
    if (end == std::string::npos) {
        return {};
    }
    return json.substr(start, end - start);
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
const char* deepLinkRegisterWireMethod() { return "subscribeForDeepLink"; }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
const char* deepLinkRegisterWireMethod() { return "registerDeeplinkListener"; }
#else
const char* deepLinkRegisterWireMethod() { return nullptr; }
#endif

void ensureRpcEventHandlerInstalled() {
    if (gListeners.rpcEventHandlerInstalled) {
        return;
    }
    gListeners.rpcEventHandlerInstalled = true;
    rpc::setRpcEventHandler([](const std::string& eventJson) {
        const std::string eventName = extractJsonStringField(eventJson, "event");
        CCLOG("[AF-SDK7] event (game thread): %s", eventJson.c_str());

        if (eventName == kEventSessionReady) {
            void (*onReady)() = nullptr;
            {
                std::lock_guard<std::mutex> lock(gListeners.mutex);
                onReady = gListeners.onSessionReady;
            }
            if (onReady) {
                onReady();
            }
            return;
        }

        if (eventName == kEventConversionSuccess) {
            void (*onSuccess)(cocos2d::ValueMap) = nullptr;
            {
                std::lock_guard<std::mutex> lock(gListeners.mutex);
                onSuccess = gListeners.onConversionSuccess;
            }
            if (onSuccess) {
                onSuccess(cocos2d::ValueMap{});
            }
            return;
        }

        if (eventName == kEventConversionFail) {
            void (*onFail)(cocos2d::ValueMap) = nullptr;
            {
                std::lock_guard<std::mutex> lock(gListeners.mutex);
                onFail = gListeners.onConversionFail;
            }
            if (onFail) {
                onFail(cocos2d::ValueMap{});
            }
            return;
        }

        if (eventName == kEventDeepLinking || eventName == kEventDeepLinkReceived) {
            void (*onDeepLinkFn)(AppsFlyerXDeepLinkResult) = nullptr;
            std::function<void(AppsFlyerXDeepLinkResult)> onDeepLinkHandler;
            {
                std::lock_guard<std::mutex> lock(gListeners.mutex);
                onDeepLinkFn = gListeners.onDeepLinkingFn;
                onDeepLinkHandler = gListeners.onDeepLinkingHandler;
            }
            AppsFlyerXDeepLinkResult result;
            result.status = FOUND;
            if (onDeepLinkHandler) {
                onDeepLinkHandler(result);
            } else if (onDeepLinkFn) {
                onDeepLinkFn(result);
            }
        }
    });
}

void setSessionReadyCallback(void (*callback)()) {
    std::lock_guard<std::mutex> lock(gListeners.mutex);
    gListeners.onSessionReady = callback;
}

void (*sessionReadyCallbackSnapshot())() {
    std::lock_guard<std::mutex> lock(gListeners.mutex);
    return gListeners.onSessionReady;
}

void setConversionSuccessCallback(void (*callback)(cocos2d::ValueMap)) {
    std::lock_guard<std::mutex> lock(gListeners.mutex);
    gListeners.onConversionSuccess = callback;
}

void (*conversionSuccessCallbackSnapshot())(cocos2d::ValueMap) {
    std::lock_guard<std::mutex> lock(gListeners.mutex);
    return gListeners.onConversionSuccess;
}

void setConversionFailCallback(void (*callback)(cocos2d::ValueMap)) {
    std::lock_guard<std::mutex> lock(gListeners.mutex);
    gListeners.onConversionFail = callback;
}

void (*conversionFailCallbackSnapshot())(cocos2d::ValueMap) {
    std::lock_guard<std::mutex> lock(gListeners.mutex);
    return gListeners.onConversionFail;
}

void setDeepLinkHandler(std::function<void(AppsFlyerXDeepLinkResult)> handler) {
    std::lock_guard<std::mutex> lock(gListeners.mutex);
    gListeners.onDeepLinkingHandler = std::move(handler);
    gListeners.onDeepLinkingFn = nullptr;
}

void setDeepLinkCallback(void (*callback)(AppsFlyerXDeepLinkResult)) {
    std::lock_guard<std::mutex> lock(gListeners.mutex);
    gListeners.onDeepLinkingFn = callback;
    gListeners.onDeepLinkingHandler = {};
}

void (*deepLinkCallbackSnapshot())(AppsFlyerXDeepLinkResult) {
    std::lock_guard<std::mutex> lock(gListeners.mutex);
    return gListeners.onDeepLinkingFn;
}

std::function<void(AppsFlyerXDeepLinkResult)> deepLinkHandlerSnapshot() {
    std::lock_guard<std::mutex> lock(gListeners.mutex);
    return gListeners.onDeepLinkingHandler;
}

} // namespace internal

AppsFlyerTask registerDeepLinkListener(std::function<void(AppsFlyerXDeepLinkResult)> onDeepLink,
                                       AppsFlyerCompletion onComplete) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    internal::ensureRpcEventHandlerInstalled();
    const auto previousHandler = internal::deepLinkHandlerSnapshot();
    void (*previousFn)(AppsFlyerXDeepLinkResult) = internal::deepLinkCallbackSnapshot();

    std::function<void(AppsFlyerXDeepLinkResult)> handler = std::move(onDeepLink);
    auto install = [handler]() {
        internal::setDeepLinkCallback(nullptr);
        internal::setDeepLinkHandler(handler);
    };
    auto restore = [previousHandler, previousFn]() {
        internal::setDeepLinkHandler(previousHandler);
        internal::setDeepLinkCallback(previousFn);
    };

    return registerListenerRpc(internal::deepLinkRegisterWireMethod(), "{}", install, restore,
                               std::move(onComplete));
#else
    return detail::unsupportedPlatformTask(std::move(onComplete));
#endif
}

AppsFlyerTask registerSessionReadyListener(void (*sessionReadyHandler)(),
                                           AppsFlyerCompletion onComplete) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    internal::ensureRpcEventHandlerInstalled();
    void (*previous)() = internal::sessionReadyCallbackSnapshot();
    auto install = [sessionReadyHandler]() { internal::setSessionReadyCallback(sessionReadyHandler); };
    auto restore = [previous]() { internal::setSessionReadyCallback(previous); };
    return registerListenerRpc("registerSessionReadyListener", "{}", install, restore,
                               std::move(onComplete));
#else
    return detail::unsupportedPlatformTask(std::move(onComplete));
#endif
}

AppsFlyerTask registerConversionListener(void (*onSuccess)(cocos2d::ValueMap),
                                         void (*onFail)(cocos2d::ValueMap),
                                         AppsFlyerCompletion onComplete) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    internal::ensureRpcEventHandlerInstalled();
    void (*prevSuccess)(cocos2d::ValueMap) = internal::conversionSuccessCallbackSnapshot();
    void (*prevFail)(cocos2d::ValueMap) = internal::conversionFailCallbackSnapshot();
    auto install = [onSuccess, onFail]() {
        internal::setConversionSuccessCallback(onSuccess);
        internal::setConversionFailCallback(onFail);
    };
    auto restore = [prevSuccess, prevFail]() {
        internal::setConversionSuccessCallback(prevSuccess);
        internal::setConversionFailCallback(prevFail);
    };
    return registerListenerRpc("registerConversionListener", "{}", install, restore,
                               std::move(onComplete));
#else
    return detail::unsupportedPlatformTask(std::move(onComplete));
#endif
}

#if defined(AF_RPC_HEADLESS_TEST)
void setDeepLinkCallback(void (*callback)(AppsFlyerXDeepLinkResult)) {
    internal::setDeepLinkCallback(callback);
}

std::function<void(AppsFlyerXDeepLinkResult)> deepLinkHandlerSnapshot() {
    return internal::deepLinkHandlerSnapshot();
}
#endif

} // namespace appsflyer

#if !defined(AF_RPC_HEADLESS_TEST) && (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

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
