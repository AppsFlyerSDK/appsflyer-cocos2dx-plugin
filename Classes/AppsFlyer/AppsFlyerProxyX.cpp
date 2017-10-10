//
// Created by Maxim Shoustin on 10/9/17.
//

#include "AppsFlyerProxyX.h"
#include "../../cocos2d/cocos/platform/CCPlatformMacros.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

void setAttributionCallbackMethod(void (*callbackMethod)(cocos2d::ValueMap installData)) {
    if (NULL == attributionCallbackMethod) {
        attributionCallbackMethod = callbackMethod;
    }
}


JNIEXPORT void JNICALL Java_com_appsflyer_AppsFlyer2dXConversionCallback_onInstallConversionDataLoadedNative
        (JNIEnv *env, jobject obj, jobject attributionObject) {

    CCLOG("%s","Java_com_appsflyer_AppsFlyer2dXConversionCallback_onInstallConversionDataLoadedNative is called");

    if (NULL == attributionCallbackMethod) {
        return;
    }



//    attributionCallbackMethod();
}

JNIEXPORT void JNICALL Java_com_appsflyer_AppsFlyer2dXConversionCallback_onInstallConversionFailureNative
        (JNIEnv *env, jobject obj, jstring stringError) {

    CCLOG("%s","Java_com_appsflyer_AppsFlyer2dXConversionCallback_onInstallConversionFailureNative is called");

    if (NULL == attributionCallbackMethod) {
        return;
    }
}

#endif

