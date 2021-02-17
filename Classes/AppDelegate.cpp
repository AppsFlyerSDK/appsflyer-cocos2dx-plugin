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
#include "AppsFlyer/AppsFlyerXDeepLinkResult.h"
#include "../cocos2d/cocos/platform/CCPlatformMacros.h"

USING_NS_CC;

static cocos2d::Size designResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate() 
{
#if USE_AUDIO_ENGINE
    AudioEngine::end();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::end();
#endif
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
    
    
}

// if you want to use the package manager to install more packages,  
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

static void onConversionDataSuccess(cocos2d::ValueMap installData) {
    CCLOG("%s", "AppDelegate.cpp got conversion data!");
    std::string gcd = "Conversion Data : ";
    
    for (auto &t : installData){
        
        CCLOG("%s - %s", t.first.c_str(), t.second.asString().c_str());
        gcd.append(t.first.c_str());
        gcd.append(" : ");
        gcd.append(t.second.asString().c_str());
        gcd.append("\n");
    }
//    auto gcdStr = gcd;
//    auto currentScene = Director::getInstance()->getRunningScene();
//    auto size = Director::getInstance()->getVisibleSize();
//    Vec2 origin = Director::getInstance()->getVisibleOrigin();
//    auto myLabel = Label::createWithSystemFont(gcd.c_str(), "Arial", 6);
//    myLabel->setPosition(Vec2(origin.x + 80, origin.y + size.height - 180));
//    //myLabel->setHeight( origin.y + size.height - 180);
//    //myLabel->setHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
//    currentScene->addChild(myLabel,1);
   
    
}

static void onConversionDataFail(cocos2d::ValueMap map) {
    for (auto &t : map){
        CCLOG("%s - %s", t.first.c_str(), t.second.asString().c_str());
    }
}

static void onAppOpenAttribution(cocos2d::ValueMap map) {
    for (auto &t : map){
        CCLOG("%s - %s", t.first.c_str(), t.second.asString().c_str());
    }
}

static void onAppOpenAttributionFailure(cocos2d::ValueMap map) {
    for (auto &t : map){
        CCLOG("%s - %s", t.first.c_str(), t.second.asString().c_str());
    }
}

static void didResolveDeepLink(AppsFlyerXDeepLinkResult result){
    CCLOG("%s", "AppDelegate.cpp got ddl!");
    std::string ddl = "Deep link data is \n";
    switch (result.status) {
    case NOTFOUND:
            CCLOG("deep link not found");
            break;
        case FOUND:
            if (!result.deepLink.empty()){
                if (!result.getMediaSource().empty()) {
                    CCLOG("Media source is %s", result.getMediaSource().c_str());
                }
                for (auto &t : result.deepLink){
                    CCLOG("%s - %s", t.first.c_str(), t.second.asString().c_str());
                    ddl.append(t.first.c_str());
                    ddl.append(" : ");
                    ddl.append(t.second.asString().c_str());
                    ddl.append("\n");
                }
                auto currentScene = Director::getInstance()->getRunningScene();
                auto child = currentScene->getChildByTag(1);
                if (child != NULL)
                    currentScene->removeChildByTag(1);
                auto size = Director::getInstance()->getVisibleSize();
                Vec2 origin = Director::getInstance()->getVisibleOrigin();
                auto myLabel = Label::createWithSystemFont(ddl.c_str(), "Arial", 6);
                myLabel->setPosition(Vec2(origin.x + 80, origin.y + size.height - 180));
                //myLabel->setHeight( origin.y + size.height - 180);
                //myLabel->setHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
                currentScene->addChild(myLabel,1, 1);
            }
    }

}

bool AppDelegate::applicationDidFinishLaunching() {

    AppsFlyerX::stop(false);

    AppsFlyerX::setIsDebug(true);
    //AppsFlyerX::setMinTimeBetweenSessions(9);
    AppsFlyerX::setAppsFlyerDevKey("2fRSe9XZe7mSNL47cmANsG");

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerX::setAppleAppID("0666151819");
   // AppsFlyerX::waitForATTUserAuthorizationWithTimeoutInterval(60);

#endif
//        std::vector<std::string> partners;
//         partners.push_back("facebook_int");
//         partners.push_back("googleadwords_int");
//         AppsFlyerX::sharingFilter(partners);
//        AppsFlyerX::sharingFilterForAllPartners();
        AppsFlyerX::setOnConversionDataSuccess(onConversionDataSuccess);
        AppsFlyerX::setOnConversionDataFail(onConversionDataFail);
        AppsFlyerX::setOnAppOpenAttribution(onAppOpenAttribution);
        AppsFlyerX::setOnAppOpenAttributionFailure(onAppOpenAttributionFailure);
       AppsFlyerX::setDidResolveDeepLink(didResolveDeepLink);
    ValueMap data;
    data["“thePartnerId”"] = "abcd";
    data["Item_id"] = 1;
    data["isLegacy"] = false;
    AppsFlyerX::setPartnerData("partnerID", data);

    AppsFlyerX::logEvent(AFEventPurchase, {{ "key1", cocos2d::Value("value1")},
                                             { "key2", cocos2d::Value("value2")}});
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AppsFlyerX::start();
#endif

    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("AppsFlyerCocos2dX", cocos2d::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
        glview = GLViewImpl::create("AppsFlyerCocos2dX");
#endif
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);

    // Set the design resolution
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);
    auto frameSize = glview->getFrameSize();
    // if the frame's height is larger than the height of medium size.
    if (frameSize.height > mediumResolutionSize.height)
    {        
        director->setContentScaleFactor(MIN(largeResolutionSize.height/designResolutionSize.height, largeResolutionSize.width/designResolutionSize.width));
    }
    // if the frame's height is larger than the height of small size.
    else if (frameSize.height > smallResolutionSize.height)
    {        
        director->setContentScaleFactor(MIN(mediumResolutionSize.height/designResolutionSize.height, mediumResolutionSize.width/designResolutionSize.width));
    }
    // if the frame's height is smaller than the height of medium size.
    else
    {        
        director->setContentScaleFactor(MIN(smallResolutionSize.height/designResolutionSize.height, smallResolutionSize.width/designResolutionSize.width));
    }

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

    AppsFlyerX::didEnterBackground();

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

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AppsFlyerX::start();
#endif

#if USE_AUDIO_ENGINE
    AudioEngine::resumeAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    SimpleAudioEngine::getInstance()->resumeAllEffects();
#endif
}
