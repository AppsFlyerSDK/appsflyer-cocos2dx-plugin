#include "AppsFlyerXCore.h"

#include "AFSDKXPurchaseDetails.h"
#include "AFSDKXValidateAndLogResult.h"
#include "AFXAdRevenueData.h"
#include "AppsFlyerX.h"

#include "AppsFlyer/rpc/AppsFlyerCocosThread.h"
#include "AppsFlyer/rpc/AppsFlyerRpcClient.h"

#include "cocos2d.h"

#include <atomic>
#include <functional>
#include <mutex>
#include <thread>

namespace appsflyer {
namespace {

constexpr const char* kPluginName = "cocos2dx";
constexpr const char* kPluginVersion = "7.0.0-poc";

constexpr const char* kEventSessionReady = "onSessionReady";
constexpr const char* kEventConversionSuccess = "onConversionDataSuccess";
constexpr const char* kEventConversionFail = "onConversionDataFail";
constexpr const char* kEventDeepLinking = "onDeepLinking";

struct PlatformConfig {
    const char* deepLinkRegisterMethod;
    const char* deepLinkRegisterLogLabel;
    std::function<std::string(const std::string& devKey, const std::string& appleAppId)> buildInitParams;
};

std::string gDevKey;
std::string gAppleAppId;
std::atomic<bool> gPendingIsDebug{false};
std::atomic<bool> gHasPendingIsDebug{false};

void (*gOnSessionReady)() = nullptr;
void (*gOnConversionSuccess)(cocos2d::ValueMap) = nullptr;
void (*gOnConversionFail)(cocos2d::ValueMap) = nullptr;
void (*gOnDeepLinking)(AppsFlyerXDeepLinkResult) = nullptr;

std::mutex gListenerMutex;
bool gRpcEventHandlerInstalled = false;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

PlatformConfig platformConfig() {
    PlatformConfig config;
    config.deepLinkRegisterMethod = "subscribeForDeepLink";
    config.deepLinkRegisterLogLabel = "subscribeForDeepLink";
    config.buildInitParams = [](const std::string& devKey, const std::string&) -> std::string {
        if (devKey.empty()) {
            CCLOG("[AF-SDK7] init skipped: dev key not set");
            return {};
        }
        return std::string("{\"devKey\":\"") + devKey + "\"}";
    };
    return config;
}

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

PlatformConfig platformConfig() {
    PlatformConfig config;
    config.deepLinkRegisterMethod = "registerDeeplinkListener";
    config.deepLinkRegisterLogLabel = "registerDeeplinkListener";
    config.buildInitParams = [](const std::string& devKey,
                                const std::string& appleAppId) -> std::string {
        if (devKey.empty()) {
            CCLOG("[AF-SDK7] init skipped: dev key not set");
            return {};
        }
        if (appleAppId.empty()) {
            CCLOG("[AF-SDK7] init skipped: Apple app ID not set");
            return {};
        }
        return std::string("{\"devKey\":\"") + devKey + "\",\"appId\":\"" + appleAppId + "\"}";
    };
    return config;
}

#else

PlatformConfig platformConfig() { return {}; }

#endif

void logRpcFailure(const char* method, const rpc::RpcResult& result) {
    if (!result.success) {
        CCLOG("[AF-SDK7] %s failed: %s", method, result.errorMessage.c_str());
    }
}

rpc::RpcResult rpcCall(const char* method, const std::string& paramsJson = "{}") {
    return rpc::executeBlocking(method, paramsJson);
}

void runOnWorker(std::function<void()> task) {
    std::thread([task = std::move(task)]() { task(); }).detach();
}

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

void ensureRpcEventHandler() {
    if (gRpcEventHandlerInstalled) {
        return;
    }
    gRpcEventHandlerInstalled = true;
    rpc::setRpcEventHandler([](const std::string& eventJson) {
        const std::string eventName = extractJsonStringField(eventJson, "event");
        CCLOG("[AF-SDK7] event (game thread): %s", eventJson.c_str());

        if (eventName == kEventSessionReady) {
            void (*onReady)() = nullptr;
            {
                std::lock_guard<std::mutex> lock(gListenerMutex);
                onReady = gOnSessionReady;
            }
            if (onReady) {
                onReady();
            }
            return;
        }

        if (eventName == kEventConversionSuccess) {
            void (*onSuccess)(cocos2d::ValueMap) = nullptr;
            {
                std::lock_guard<std::mutex> lock(gListenerMutex);
                onSuccess = gOnConversionSuccess;
            }
            if (onSuccess) {
                onSuccess(cocos2d::ValueMap{});
            }
            return;
        }

        if (eventName == kEventConversionFail) {
            void (*onFail)(cocos2d::ValueMap) = nullptr;
            {
                std::lock_guard<std::mutex> lock(gListenerMutex);
                onFail = gOnConversionFail;
            }
            if (onFail) {
                onFail(cocos2d::ValueMap{});
            }
            return;
        }

        if (eventName == kEventDeepLinking) {
            void (*onDeepLink)(AppsFlyerXDeepLinkResult) = nullptr;
            {
                std::lock_guard<std::mutex> lock(gListenerMutex);
                onDeepLink = gOnDeepLinking;
            }
            if (onDeepLink) {
                AppsFlyerXDeepLinkResult result;
                result.status = FOUND;
                onDeepLink(result);
            }
        }
    });
}

} // namespace

void setDevKey(const std::string& devKey) { gDevKey = devKey; }
std::string devKey() { return gDevKey; }

void setAppleAppId(const std::string& appleAppId) { gAppleAppId = appleAppId; }
std::string appleAppId() { return gAppleAppId; }

void setPendingIsDebug(bool isDebug) {
    gPendingIsDebug = isDebug;
    gHasPendingIsDebug = true;
}

bool pendingIsDebug() { return gPendingIsDebug.load(); }

void registerSessionReadyListener(void (*callback)()) {
    ensureRpcEventHandler();
    std::lock_guard<std::mutex> lock(gListenerMutex);
    gOnSessionReady = callback;
}

void setOnConversionSuccess(void (*callback)(cocos2d::ValueMap)) {
    ensureRpcEventHandler();
    std::lock_guard<std::mutex> lock(gListenerMutex);
    gOnConversionSuccess = callback;
}

void setOnConversionFail(void (*callback)(cocos2d::ValueMap)) {
    ensureRpcEventHandler();
    std::lock_guard<std::mutex> lock(gListenerMutex);
    gOnConversionFail = callback;
}

void setOnDeepLinking(void (*callback)(AppsFlyerXDeepLinkResult)) {
    ensureRpcEventHandler();
    std::lock_guard<std::mutex> lock(gListenerMutex);
    gOnDeepLinking = callback;
}

void initSdk() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    const PlatformConfig platform = platformConfig();
    ensureRpcEventHandler();
    runOnWorker([platform]() {
        bool wantDeepLink = false;
        bool wantConversion = false;
        bool wantSessionReady = false;
        {
            std::lock_guard<std::mutex> lock(gListenerMutex);
            wantDeepLink = gOnDeepLinking != nullptr;
            wantConversion = gOnConversionSuccess != nullptr || gOnConversionFail != nullptr;
            wantSessionReady = gOnSessionReady != nullptr;
        }

        if (wantDeepLink) {
            logRpcFailure(platform.deepLinkRegisterLogLabel,
                          rpcCall(platform.deepLinkRegisterMethod));
        }
        if (wantConversion) {
            logRpcFailure("registerConversionListener", rpcCall("registerConversionListener"));
        }
        if (wantSessionReady) {
            logRpcFailure("registerSessionReadyListener", rpcCall("registerSessionReadyListener"));
        }

        const std::string pluginInfo = std::string("{\"plugin\":\"") + kPluginName +
                                       "\",\"pluginVersion\":\"" + kPluginVersion + "\"}";
        logRpcFailure("setPluginInfo", rpcCall("setPluginInfo", pluginInfo));
        if (gHasPendingIsDebug.load()) {
            const bool debug = gPendingIsDebug.load();
            logRpcFailure("isDebug",
                          rpcCall("isDebug", debug ? "{\"isDebug\":true}" : "{\"isDebug\":false}"));
        }

        const std::string initParams = platform.buildInitParams(gDevKey, gAppleAppId);
        if (initParams.empty()) {
            return;
        }
        logRpcFailure("init", rpcCall("init", initParams));
    });
#endif
}

void start(bool awaitResponse, std::function<void(bool success)> onComplete) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    runOnWorker([awaitResponse, onComplete = std::move(onComplete)]() {
        const std::string params =
            awaitResponse ? "{\"awaitResponse\":true}" : "{\"awaitResponse\":false}";
        const rpc::RpcResult result = rpcCall("start", params);
        if (onComplete) {
            const bool success = result.success;
            cocos::runOnGameThread([onComplete = std::move(onComplete), success]() {
                onComplete(success);
            });
        } else {
            logRpcFailure("start", result);
        }
    });
#endif
}

void stop(bool /*shouldStop*/) {}

void logEvent(const std::string& eventName, cocos2d::ValueMap /*eventValues*/, bool awaitResponse,
              std::function<void(bool success)> onComplete) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    runOnWorker([eventName, awaitResponse, onComplete = std::move(onComplete)]() {
        const std::string params = std::string("{\"eventName\":\"") + eventName +
                                   "\",\"eventValues\":{},\"awaitResponse\":" +
                                   (awaitResponse ? "true" : "false") + "}";
        const rpc::RpcResult result = rpcCall("logEvent", params);
        if (onComplete) {
            const bool success = result.success;
            cocos::runOnGameThread([onComplete = std::move(onComplete), success]() {
                onComplete(success);
            });
        } else {
            logRpcFailure("logEvent", result);
        }
    });
#endif
}

void enableTCFDataCollection(bool) {}
void setConsentData(const AppsFlyerXConsent&) {}

void setCustomerUserID(const std::string&) {}
std::string customerUserID() { return {}; }

void setAdditionalData(cocos2d::ValueMap) {}
cocos2d::ValueMap customData() { return {}; }

void setCurrencyCode(const std::string&) {}
std::string currencyCode() { return {}; }

void setDisableAdvertisingIdentifiers(bool) {}
void disableAdvertisingIdentifier(bool) {}
bool isDisabledAdvertisingIdentifier() { return false; }

void didEnterBackground() {}

void setShouldCollectDeviceName(bool) {}
bool isShouldCollectDeviceName() { return false; }

void setAppInviteOneLink(const std::string&) {}
std::string appInviteOneLinkID() { return {}; }

void anonymizeUser(bool) {}
bool isAnonymizedUser() { return false; }

void setDisableCollectASA(bool) {}
bool isDisabledCollectASA() { return false; }

void setUseReceiptValidationSandbox(bool) {}
bool isUseReceiptValidationSandbox() { return false; }

void setUseUninstallSandbox(bool) {}
bool isUseUninstallSandbox() { return false; }

void setUserEmails(std::vector<std::string>, EmailCryptTypeX) {}

void logAdRevenue(const AFXAdRevenueData&, cocos2d::ValueMap) {
    CCLOG("[AF-SDK7] logAdRevenue not in Phase 1");
}

void validateAndLogInAppPurchase(const std::string&, const std::string&, const std::string&,
                                 const std::string&, cocos2d::ValueMap,
                                 std::function<void(cocos2d::ValueMap)>,
                                 std::function<void(cocos2d::ValueMap)>) {}

void validateAndLogInAppPurchase(const std::string&, const std::string&, const std::string&,
                                 const std::string&, const std::string&, cocos2d::ValueMap) {}

void validateAndLogInAppPurchase(AFSDKXPurchaseDetails&, cocos2d::ValueMap,
                                 std::function<void(cocos2d::ValueMap, cocos2d::ValueMap)>) {}

void validateAndLogInAppPurchase(AFSDKXPurchaseDetails&, cocos2d::ValueMap,
                                 std::function<void(AFSDKXValidateAndLogResult)>) {}

void logLocation(double, double) {}

std::string getAppsFlyerUID() { return {}; }

void handleOpenURL(const std::string&, const std::string&) {}
void handleOpenURL(std::string, std::string, void*) {}
void handleOpenURL(std::string, cocos2d::ValueMap) {}

void handlePushNotification(cocos2d::ValueMap) {}
void registerUninstall(void*, unsigned long) {}
void registerUninstall(const std::string&) {}

void setHost(const std::string&) {}
std::string getHost() { return {}; }

void setMinTimeBetweenSessions(unsigned long) {}
unsigned long getMinTimeBetweenSessions() { return 0; }

void setOnAppOpenAttribution(void (*)(cocos2d::ValueMap)) {}
void setOnAppOpenAttributionFailure(void (*)(cocos2d::ValueMap)) {}

void sharingFilter(std::vector<std::string>) {}
void sharingFilterForAllPartners() {}

void disableSKAdNetwork(bool) {}
bool isDisabledSKAdNetwork() { return false; }

void waitForATTUserAuthorizationWithTimeoutInterval(double) {}

void setPhoneNumber(const std::string&) {}

void setPartnerData(const std::string&, cocos2d::ValueMap) {}
void setOneLinkCustomDomains(std::vector<std::string>) {}
void setCurrentDeviceLanguage(const std::string&) {}
void setSharingFilterForPartners(std::vector<std::string>) {}

void logInvite(const std::string&, cocos2d::ValueMap) {}

void generateInviteUrl(cocos2d::ValueMap, void (*)(std::string), void (*)(std::string)) {}

void generateUserInviteLink(cocos2d::ValueMap, std::function<void(std::string)>) {}

void setDisableNetworkData(bool) {}

} // namespace appsflyer
