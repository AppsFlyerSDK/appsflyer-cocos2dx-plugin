#include "AppDelegate.h"
#include "HelloWorldScene.h"

// #define USE_AUDIO_ENGINE 1
// #define USE_SIMPLE_AUDIO_ENGINE 1

#if USE_AUDIO_ENGINE && USE_SIMPLE_AUDIO_ENGINE
#error "Don't use AudioEngine and SimpleAudioEngine at the same time. Please just select one in your game!"
#endif

#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
#elif USE_SIMPLE_AUDIO_ENGINE
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
#endif

#include "AppsFlyer/AppsFlyerX.h"
#include "AppsFlyer/AppsFlyerDeepLinkData.h"
#include "platform/CCCommon.h"
#include "../cocos2d/cocos/platform/CCPlatformMacros.h"

#include <cstdio>

USING_NS_CC;

static void runAutoPostStartFlow() {
    AppsFlyerX::LogEventParams event;
    event.eventName = "af_auto_launch";
    ValueMap autoValues;
    autoValues["source"] = "AppDelegate";
    event.eventValues = autoValues;
    event.awaitResponse = false;
    AppsFlyerX::logEvent(event).whenComplete([](const AppsFlyerResult& rLog) {
        CCLOG("[AppsFlyer] auto logEvent finished success=%d", rLog.success ? 1 : 0);
        AppsFlyerX::fetchAppsFlyerUID().whenComplete([](const AppsFlyerResult& rUid) {
            if (rUid.success) {
                CCLOG("[AppsFlyer] auto fetchAppsFlyerUID: %s", rUid.dataJson.c_str());
            } else {
                CCLOG("[AppsFlyer] auto fetchAppsFlyerUID failed");
            }
            AppsFlyerX::isSessionReady().whenComplete([](const AppsFlyerResult& rReady) {
                CCLOG("[AppsFlyer] auto isSessionReady: success=%d data=%s", rReady.success ? 1 : 0,
                      rReady.dataJson.c_str());
            });
        });
    });
}

static void onSessionReadyStartSdk() {
    AppsFlyerX::start(true).whenComplete([](const AppsFlyerResult& r) {
        HelloWorld::appendAfResult("start", r);
        CCLOG("[AppsFlyer] start awaitResponse complete, success=%d", r.success ? 1 : 0);
        if (!r.success) {
            return;
        }
        runAutoPostStartFlow();
    });
}

// Portrait-friendly logical size (iPhone-class). FIXED_WIDTH keeps the QA panel readable on tall devices.
static cocos2d::Size designResolutionSize = cocos2d::Size(390, 844);

AppDelegate::AppDelegate() {}

AppDelegate::~AppDelegate() {
#if USE_AUDIO_ENGINE
    AudioEngine::end();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::end();
#endif
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs() {
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages,
// don't modify or remove this function
static int register_all_packages() {
    return 0; // flag for packages manager
}

static void onConversionDataSuccess(cocos2d::ValueMap installData) {
    CCLOG("%s", "AppDelegate.cpp got conversion data!");
    for (auto& t : installData) {
        CCLOG("%s - %s", t.first.c_str(), t.second.asString().c_str());
    }
    HelloWorld::appendConversionData("Conversion data", installData);
}

static void onConversionDataFail(cocos2d::ValueMap map) {
    for (auto& t : map) {
        CCLOG("%s - %s", t.first.c_str(), t.second.asString().c_str());
    }
    HelloWorld::appendConversionData("Conversion data (fail)", map);
}

static void didResolveDeepLink(appsflyer::DeepLinkData result) {
    CCLOG("%s", "AppDelegate.cpp got ddl!");
    HelloWorld::appendDeepLink(result);
}

bool AppDelegate::applicationDidFinishLaunching() {
    AppsFlyerX::stop(false);

    AppsFlyerX::SetPartnerDataParams partnerParams;
    partnerParams.partnerId = "partnerID";
    ValueMap partnerData;
    partnerData["thePartnerId"] = "abcd";
    partnerData["Item_id"] = 1;
    partnerData["isLegacy"] = false;
    partnerParams.data = partnerData;
    AppsFlyerX::setPartnerData(partnerParams);

    // SDK 7 manual startup: deep link + conversion listeners, then session-ready → start() (after init task chain).
    // Schema task order: deep link → enableDebug → init → conversion + session-ready listeners → start().
    AppsFlyerX::registerDeepLinkListener(
        [](appsflyer::DeepLinkData result) { didResolveDeepLink(result); })
        .whenComplete([](const AppsFlyerResult& rDeepLink) {
            if (!rDeepLink.success) {
                CCLOG("[AppsFlyer] registerDeepLinkListener failed");
                return;
            }
            AppsFlyerX::enableDebug(true).whenComplete([](const AppsFlyerResult& rDebug) {
                if (!rDebug.success) {
                    CCLOG("[AppsFlyer] enableDebug failed");
                    return;
                }
                AppsFlyerX::InitParams params;
                params.devKey = "H9xZweqPFhzBEtiDh2vDj";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
                params.appId = "942960987";
#endif
                AppsFlyerX::init(params).whenComplete([](const AppsFlyerResult& rInit) {
                    HelloWorld::appendAfResult("init", rInit);
                    if (!rInit.success) {
                        CCLOG("[AppsFlyer] init failed");
                        return;
                    }
                    AppsFlyerX::setCurrencyCode("USD").whenComplete([](const AppsFlyerResult& rCur) {
                        CCLOG("[AppsFlyer] auto setCurrencyCode success=%d", rCur.success ? 1 : 0);
                    });
                    AppsFlyerX::setCustomerUserId("cocos2dx-qa-auto").whenComplete(
                        [](const AppsFlyerResult& rCuid) {
                            CCLOG("[AppsFlyer] auto setCustomerUserId success=%d", rCuid.success ? 1 : 0);
                        });
                    ValueMap additional;
                    additional["auto_init"] = true;
                    AppsFlyerX::setAdditionalData(additional);
                    AppsFlyerX::registerConversionListener(onConversionDataSuccess,
                                                           onConversionDataFail)
                        .whenComplete([](const AppsFlyerResult& rConv) {
                            if (!rConv.success) {
                                CCLOG("[AppsFlyer] registerConversionListener failed");
                                return;
                            }
                            AppsFlyerX::registerSessionReadyListener(onSessionReadyStartSdk);
                        });
                });
            });
        });

    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if (!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || \
    (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("AppsFlyerCocos2dX",
                                            cocos2d::Rect(0, 0, designResolutionSize.width,
                                                          designResolutionSize.height));
#else
        glview = GLViewImpl::create("AppsFlyerCocos2dX");
#endif
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);

    const auto frameSize = glview->getFrameSize();
    const float designAspect = designResolutionSize.width / designResolutionSize.height;
    const float frameAspect = frameSize.width / frameSize.height;
    const auto policy = (frameAspect > designAspect) ? ResolutionPolicy::FIXED_HEIGHT
                                                     : ResolutionPolicy::FIXED_WIDTH;
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height,
                                    policy);

    register_all_packages();

    // create a scene. it's an autorelease object
    auto scene = HelloWorld::createScene();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AppsFlyerX::onPause();
#endif

#if USE_AUDIO_ENGINE
    AudioEngine::pauseAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    SimpleAudioEngine::getInstance()->pauseAllEffects();
#endif
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    //CCLOG("%s", "~+~+~+~+~   applicationWillEnterForeground ~+~+~+~+~");

#if USE_AUDIO_ENGINE
    AudioEngine::resumeAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    SimpleAudioEngine::getInstance()->resumeAllEffects();
#endif
}