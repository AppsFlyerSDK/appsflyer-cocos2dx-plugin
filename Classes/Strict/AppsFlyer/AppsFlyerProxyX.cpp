//
// Created by Maxim Shoustin on 10/9/17.
//

#include <string>
#include "AppsFlyerProxyX.h"
#include "AppsFlyerXDeepLinkResult.h"
#include <typeinfo>
#include<algorithm>
#include "../../cocos2d/cocos/platform/CCPlatformMacros.h"




#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

cocos2d::ValueMap getMapForCallback(JNIEnv *env, jobject attributionObject, bool success);
AppsFlyerXDeepLinkResult getResultForCallbackDDL(JNIEnv *env, jobject result);
cocos2d::ValueMap stringToMap(std::string &s);

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

void setCallbackOnDeepLinking(
        void (*callbackMethod)(AppsFlyerXDeepLinkResult result)) {
    if (NULL == callbackOnDeepLinking) {
        callbackOnDeepLinking = callbackMethod;
    }
}

void setCallbackOnResponse(
        void (*callbackMethod)(std::string oneLinkURL)) {
    if (NULL == callbackOnResponse) {
        callbackOnResponse = callbackMethod;
    }
}

void setCallbackOnResponseError(
        void (*callbackMethod)(std::string oneLinkURL)) {
    if (NULL == callbackOnResponseError) {
        callbackOnResponseError = callbackMethod;
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
    attributionCallbackOnConversionDataReceived(getMapForCallback(env, attributionObject, true));
}


JNIEXPORT void JNICALL Java_com_appsflyer_AppsFlyer2dXConversionCallback_onAppOpenAttributionNative
        (JNIEnv *env, jobject obj, jobject attributionObject){
    CCLOG("%s","Java_com_appsflyer_AppsFlyer2dXConversionCallback_onAppOpenAttributionNative is called");

    if (NULL == attributionCallbackOnAppOpenAttribution) {
        return;
    }
    attributionCallbackOnAppOpenAttribution(getMapForCallback(env, attributionObject, true));

}

JNIEXPORT void JNICALL Java_com_appsflyer_AppsFlyer2dXConversionCallback_onAttributionFailureNative
        (JNIEnv *env, jobject obj, jobject stringError){

    CCLOG("%s","Java_com_appsflyer_AppsFlyer2dXConversionCallback_onInstallConversionFailureNative is called");

    if (NULL == attributionCallbackOnAppOpenAttributionFailure) {
        return;
    }
    attributionCallbackOnAppOpenAttributionFailure(getMapForCallback(env,stringError, false));
}

JNIEXPORT void JNICALL Java_com_appsflyer_AppsFlyer2dXConversionCallback_onInstallConversionFailureNative
        (JNIEnv *env, jobject obj, jobject stringError) {

    CCLOG("%s","Java_com_appsflyer_AppsFlyer2dXConversionCallback_onInstallConversionFailureNative is called");

    if (NULL == attributionCallbackOnConversionDataRequestFailure) {
        return;
    }

    attributionCallbackOnConversionDataRequestFailure(getMapForCallback(env,stringError, false));
}

JNIEXPORT void JNICALL Java_com_appsflyer_AppsFlyer2dXConversionCallback_onDeepLinkingNative
            (JNIEnv *env, jobject obj, jobject result) {

        CCLOG("%s","Java_com_appsflyer_AppsFlyer2dXConversionCallback_onDeepLinkingNative is called");

        if (NULL == callbackOnDeepLinking) {
            return;
        }
        callbackOnDeepLinking(getResultForCallbackDDL(env, result));
    }

JNIEXPORT void JNICALL Java_com_appsflyer_AppsFlyer2dXConversionCallback_onResponseNative
        (JNIEnv *env, jobject obj, jstring message) {

    CCLOG("%s","Java_com_appsflyer_AppsFlyer2dXConversionCallback_onResponseNative is called");

    if (NULL == callbackOnResponse) {
        CCLOG("%s","callback is null");
        return;
    }
    jboolean isCopy;
    const char *convertedValue = (env)->GetStringUTFChars(message, &isCopy);
    std::string string = std::string(convertedValue, (env)->GetStringLength(message));
    callbackOnResponse(string);
}

JNIEXPORT void JNICALL Java_com_appsflyer_AppsFlyer2dXConversionCallback_onResponseErrorNative
        (JNIEnv *env, jstring message) {

    CCLOG("%s","Java_com_appsflyer_AppsFlyer2dXConversionCallback_onResponseErrorNative is called");

    if (NULL == callbackOnResponseError) {
        CCLOG("%s","callback error is null");
        return;
    }
    jboolean isCopy;
    const char *convertedValue = (env)->GetStringUTFChars(message, &isCopy);
    std::string string = std::string(convertedValue, (env)->GetStringLength(message));
    callbackOnResponseError(string);
}



cocos2d::ValueMap getMapForCallback(JNIEnv *env, jobject attributionObject, bool success) {
    jclass clsHashMap = env->GetObjectClass(attributionObject);
    std::string s = typeid(clsHashMap).name();
    std::string s1 = typeid(attributionObject).name();
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
       jmethodID midGet;
        if (success)
             midGet = env->GetMethodID(clsHashMap, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");
        else
             midGet  = env->GetMethodID(clsHashMap, "get", "(Ljava/lang/String;)Ljava/lang/Object;");
    jobject objValue = env->CallObjectMethod(attributionObject, midGet, objKey);
        if   (objValue == NULL) {
            map[std::string(c_string_key)] = cocos2d::Value::Null;
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

AppsFlyerXDeepLinkResult getResultForCallbackDDL(JNIEnv *env, jobject result) {
    AppsFlyerXDeepLinkResult deepLinkResult = AppsFlyerXDeepLinkResult();
    cocos2d::ValueMap map;
    cocos2d::ValueMap mapDL;
    if (result == NULL) {
        return deepLinkResult;
    }
    jclass cls = env->GetObjectClass(result);
    if (cls == NULL) {
        return deepLinkResult;
    }

    //Status
    jmethodID methodId = env->GetMethodID(cls, "getStatus", "()Lcom/appsflyer/deeplink/DeepLinkResult$Status;");
    jobject status = env->CallObjectMethod(result,methodId);
    if (status != NULL) {
        jclass clsEnumStatus = env->FindClass("com/appsflyer/deeplink/DeepLinkResult$Status");
        jmethodID statusGetValueMethod = env->GetMethodID(clsEnumStatus, "ordinal", "()I");
        jint value = env->CallIntMethod(status, statusGetValueMethod);
        switch (value) {
            case 0:
                deepLinkResult.status = FOUND;
                break;
            case 1:
                deepLinkResult.status = NOTFOUND;
                break;
            default:
                deepLinkResult.status = FAILURE;
                break;
        }
    }

    //Error
    methodId = env->GetMethodID(cls, "getError", "()Lcom/appsflyer/deeplink/DeepLinkResult$Error;");
    jobject error = env->CallObjectMethod(result,methodId);
    if (error != NULL) {
        jclass clsEnumError = env->FindClass("com/appsflyer/deeplink/DeepLinkResult$Error");
        jmethodID errorGetValueMethod = env->GetMethodID(clsEnumError, "ordinal", "()I");
        jint value = env->CallIntMethod(error, errorGetValueMethod);
        switch (value) {
            case 0:
                deepLinkResult.error = TIMEOUT;
                break;
            case 1:
                deepLinkResult.error = NETWORK;
                break;
            case 2:
                deepLinkResult.error = HTTP_STATUS_CODE;
                break;
            case 3:
                deepLinkResult.error = UNEXPECTED;
                break;
            default:
                deepLinkResult.error = NONE;
                break;
        }
    }

    //Deep link
    methodId = env->GetMethodID(cls, "getDeepLink", "()Lcom/appsflyer/deeplink/DeepLink;");
    jobject resultDL = (jobject) env->CallObjectMethod(result,methodId);
    if (resultDL != NULL) {
        jclass clsDL = env->GetObjectClass(resultDL);
        if (clsDL == NULL)
            return deepLinkResult;
       // Deep link value
        methodId = env->GetMethodID(clsDL, "toString", "()Ljava/lang/String;");
        if (methodId != NULL) {
            jstring clickEventStr = (jstring) env->CallObjectMethod(resultDL, methodId);
            if (clickEventStr != NULL) {
                const char *name_char = env->GetStringUTFChars(clickEventStr, NULL);
                std::string click_event = std::string(name_char);
                mapDL["deepLink"] = click_event;
                deepLinkResult.deepLink = stringToMap(click_event);
            }
        }
    }

    //Delete
    env->DeleteLocalRef(status);
    env->DeleteLocalRef(error);
    env->DeleteLocalRef(resultDL);

   return deepLinkResult;



}

cocos2d::ValueMap stringToMap(std::string &s) {
    cocos2d::ValueMap m;
    s.erase(std::remove(s.begin(), s.end(), '{'), s.end());
    s.erase(std::remove(s.begin(), s.end(), '}'), s.end());
    s.erase(std::remove(s.begin(), s.end(), '\"'), s.end());
    std::string key, val;
    std::istringstream iss(s);
    while(std::getline(std::getline(iss, key, ':') >> std::ws, val, ',')) {
        m[key] = val;
    }
    return m;
}

#endif

