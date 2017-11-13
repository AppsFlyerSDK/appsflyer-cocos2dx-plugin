//
// Created by Maxim Shoustin on 10/9/17.
//

#ifndef PROJ_ANDROID_STUDIO_APPSFLYERPROXYX_H
#define PROJ_ANDROID_STUDIO_APPSFLYERPROXYX_H


#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include "cocos2d.h"

extern "C" {

static void (*attributionCallbackOnConversionDataReceived)(cocos2d::ValueMap installData);

static void (*attributionCallbackOnConversionDataRequestFailure)(cocos2d::ValueMap error);

static void (*attributionCallbackOnAppOpenAttribution)(cocos2d::ValueMap attributionData);

static void (*attributionCallbackOnAppOpenAttributionFailure)(cocos2d::ValueMap attributionData);

JNIEXPORT void JNICALL Java_com_appsflyer_AppsFlyer2dXConversionCallback_onInstallConversionDataLoadedNative
        (JNIEnv *, jobject, jobject);
JNIEXPORT void JNICALL Java_com_appsflyer_AppsFlyer2dXConversionCallback_onInstallConversionFailureNative
        (JNIEnv *, jobject, jstring);
JNIEXPORT void JNICALL Java_com_appsflyer_AppsFlyer2dXConversionCallback_onAppOpenAttributionNative
        (JNIEnv *, jobject, jobject);
JNIEXPORT void JNICALL Java_com_appsflyer_AppsFlyer2dXConversionCallback_onAttributionFailureNative
        (JNIEnv *, jobject, jobject);

 void setAttributionCallbackOnConversionDataReceived(
         void (*callbackMethod)(cocos2d::ValueMap installData));

void setAttributionCallbackOnAppOpenAttribution(
        void (*callbackMethod)(cocos2d::ValueMap attributionData));

void setAttributionCallbackOnConversionDataRequestFailure(
        void (*callbackMethod)(cocos2d::ValueMap error));

void setAttributionCallbackOnAppOpenAttributionFailure(
        void (*callbackMethod)(cocos2d::ValueMap error));

}
#endif


#endif //PROJ_ANDROID_STUDIO_APPSFLYERPROXYX_H
