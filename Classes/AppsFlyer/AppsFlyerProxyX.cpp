//
// Created by Maxim Shoustin on 10/9/17.
//

#include <string>
#include "AppsFlyerProxyX.h"
#include "../../cocos2d/cocos/platform/CCPlatformMacros.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

void setAttributionCallbackOnConversionDataReceived(
        void (*callbackMethod)(cocos2d::ValueMap installData)) {
    if (NULL == attributionCallbackOnConversionDataReceived) {
        attributionCallbackOnConversionDataReceived = callbackMethod;
    }
}

void setAttributionCallbackOnAppOpenAttribution(
        void (*callbackMethod)(cocos2d::ValueMap attributionData)) {
    if (NULL == attributionCallbackOnAppOpenAttribution) {
        attributionCallbackOnAppOpenAttribution = callbackMethod;
    }
}

void setAttributionCallbackOnConversionDataRequestFailure(
        void (*callbackMethod)(cocos2d::ValueMap error)) {
    if (NULL == attributionCallbackOnConversionDataRequestFailure) {
        attributionCallbackOnConversionDataRequestFailure = callbackMethod;
    }
}

void setAttributionCallbackOnAppOpenAttributionFailure(
        void (*callbackMethod)(cocos2d::ValueMap error)) {
    if (NULL == attributionCallbackOnAppOpenAttributionFailure) {
        attributionCallbackOnAppOpenAttributionFailure = callbackMethod;
    }
}

/**
 * TODO: handle other types of data
 * */
JNIEXPORT void JNICALL Java_com_appsflyer_AppsFlyer2dXConversionCallback_onInstallConversionDataLoadedNative
        (JNIEnv *env, jobject obj, jobject attributionObject) {

    if (NULL == attributionCallbackOnConversionDataReceived) {
        return;
    }

    jclass clsHashMap = env->GetObjectClass(attributionObject);

    jmethodID midKeySet = env->GetMethodID(clsHashMap, "keySet", "()Ljava/util/Set;");

    if (midKeySet == NULL) {
        return; /* method not found */
    }

    jobject objKeySet = env->CallObjectMethod(attributionObject, midKeySet);

     //javap -s -p java.util.Set | grep -A 1 toArray
    jclass clsSet = env->GetObjectClass(objKeySet);

    jmethodID midToArray = env->GetMethodID(clsSet, "toArray", "()[Ljava/lang/Object;");

    if (midToArray == NULL) {
        return; /* method not found */
    }

    jobjectArray arrayOfKeys = (jobjectArray) env->CallObjectMethod(objKeySet, midToArray);
    int arraySize = env->GetArrayLength(arrayOfKeys);

    cocos2d::ValueMap map;

    for (int i=0; i < arraySize; ++i){
        jstring objKey = (jstring) env->GetObjectArrayElement(arrayOfKeys, i);
        const char* c_string_key = env->GetStringUTFChars(objKey, 0);
        jmethodID midGet = env->GetMethodID(clsHashMap, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");
        jstring objValue = (jstring)env->CallObjectMethod(attributionObject, midGet, objKey);
        const char* c_string_value = env->GetStringUTFChars(objValue, 0);

        map[std::string(c_string_key)] = c_string_value;
    }

    attributionCallbackOnConversionDataReceived(map);
}


JNIEXPORT void JNICALL


JNIEXPORT void JNICALL Java_com_appsflyer_AppsFlyer2dXConversionCallback_onInstallConversionFailureNative
        (JNIEnv *env, jobject obj, jstring stringError) {

    CCLOG("%s","Java_com_appsflyer_AppsFlyer2dXConversionCallback_onInstallConversionFailureNative is called");

    if (NULL == attributionCallbackOnConversionDataReceived) {
        return;
    }
}

#endif

