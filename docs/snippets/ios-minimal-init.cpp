// Minimal SDK 7 iOS startup (Cocos2d-x AppDelegate::applicationDidFinishLaunching).
// For listener ordering and threading, see docs/SDK7-Lifecycle.md.
// Full QA sample: Classes/AppDelegate.cpp

#include "AppsFlyer/AppsFlyerX.h"
#include "AppsFlyer/AppsFlyerDeepLinkData.h"

static void onSessionReady() {
    AppsFlyerX::start(false); // awaitResponse = false
}

bool AppDelegate::applicationDidFinishLaunching() {
    AppsFlyerX::InitParams init;
    init.devKey = "YOUR_DEV_KEY";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    init.appId = "YOUR_APPLE_APP_ID"; // numeric, App Store Connect
#endif

    AppsFlyerX::registerDeepLinkListener([](appsflyer::DeepLinkData) {})
        .whenComplete([](const AppsFlyerResult& rDl) {
            if (!rDl.success) return;
            AppsFlyerX::init(init).whenComplete([](const AppsFlyerResult& rInit) {
                if (!rInit.success) return;
                AppsFlyerX::registerConversionListener(
                    [](cocos2d::ValueMap) {}, [](cocos2d::ValueMap) {})
                    .whenComplete([](const AppsFlyerResult& rConv) {
                        if (!rConv.success) return;
                        AppsFlyerX::registerSessionReadyListener(onSessionReady);
                    });
            });
        });

    // ... existing Cocos director / GL setup ...
    return true;
}