//
// Created by Maxim Shoustin on 10/9/17.
//

#include <string>
#include "AppsFlyerProxyX.h"
#include <typeinfo>
#include "../../cocos2d/cocos/platform/CCPlatformMacros.h"




#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

cocos2d::ValueMap getMapForCallback(JNIEnv *env, jobject attributionObject);

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
    attributionCallbackOnConversionDataReceived(getMapForCallback(env, attributionObject));
}


JNIEXPORT void JNICALL Java_com_appsflyer_AppsFlyer2dXConversionCallback_onAppOpenAttributionNative
        (JNIEnv *env, jobject obj, jobject attributionObject){
    CCLOG("%s","Java_com_appsflyer_AppsFlyer2dXConversionCallback_onAppOpenAttributionNative is called");

    if (NULL == attributionCallbackOnAppOpenAttribution) {
        return;
    }
    attributionCallbackOnAppOpenAttribution(getMapForCallback(env, attributionObject));

}

JNIEXPORT void JNICALL Java_com_appsflyer_AppsFlyer2dXConversionCallback_onAttributionFailureNative
        (JNIEnv *env, jobject obj, jobject stringError){

    CCLOG("%s","Java_com_appsflyer_AppsFlyer2dXConversionCallback_onInstallConversionFailureNative is called");

    if (NULL == attributionCallbackOnAppOpenAttributionFailure) {
        return;
    }
    attributionCallbackOnAppOpenAttributionFailure(getMapForCallback(env,stringError));
}

JNIEXPORT void JNICALL Java_com_appsflyer_AppsFlyer2dXConversionCallback_onInstallConversionFailureNative
        (JNIEnv *env, jobject obj, jobject stringError) {

    CCLOG("%s","Java_com_appsflyer_AppsFlyer2dXConversionCallback_onInstallConversionFailureNative is called");

    if (NULL == attributionCallbackOnConversionDataRequestFailure) {
        return;
    }

    attributionCallbackOnConversionDataRequestFailure(getMapForCallback(env,stringError));
}


cocos2d::ValueMap getMapForCallback(JNIEnv *env, jobject attributionObject) {
    jclass clsHashMap = env->GetObjectClass(attributionObject);
    cocos2d::ValueMap map;
    jmethodID midKeySet = env->GetMethodID(clsHashMap, "keySet", "()Ljava/util/Set;");

    if (midKeySet == NULL) {
        return map; /* method not found */
    }

    jobject objKeySet = env->CallObjectMethod(attributionObject, midKeySet);

    //javap -s -p java.util.Set | grep -A 1 toArray
    jclass clsSet = env->GetObjectClass(objKeySet);

    jmethodID midToArray = env->GetMethodID(clsSet, "toArray", "()[Ljava/lang/Object;");

    if (midToArray == NULL) {
        return map; /* method not found */
    }

    jobjectArray arrayOfKeys = (jobjectArray) env->CallObjectMethod(objKeySet, midToArray);
    int arraySize = env->GetArrayLength(arrayOfKeys);
    jclass jBooleanClass = env->FindClass("java/lang/Boolean");
    jclass jStringClass = env->FindClass("java/lang/String");

    for (int i=0; i < arraySize; ++i){
        jstring objKey = (jstring) env->GetObjectArrayElement(arrayOfKeys, i);
        const char* c_string_key = env->GetStringUTFChars(objKey, 0);
        jmethodID midGet = env->GetMethodID(clsHashMap, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");
        jobject objValue = env->CallObjectMethod(attributionObject, midGet, objKey);
        if   (objValue == NULL) {
            map[std::string(c_string_key)] = NULL;
        }
        else if (env->IsInstanceOf(objValue, jBooleanClass)){
            jmethodID booleanValueMID   = env->GetMethodID(jBooleanClass, "booleanValue", "()Z");
            bool booleanValue           = (bool) env->CallBooleanMethod(objValue, booleanValueMID);
            map[std::string(c_string_key)] = booleanValue;
        } else if (env->IsInstanceOf(objValue, jStringClass)){
            jstring objString = (jstring)objValue;
            const char *c_string_value = env->GetStringUTFChars(objString, 0);
            map[std::string(c_string_key)] = c_string_value;
        }

        env->DeleteLocalRef(objValue);
    }

    return map;
}


#endif

