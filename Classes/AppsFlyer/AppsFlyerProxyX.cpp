//
// Created by Maxim Shoustin on 10/9/17.
//

#include <string>
#include "AppsFlyerProxyX.h"
#include "AppsFlyerXAppleDeepLinkResult.h"
#include <typeinfo>
#include<algorithm>
#include "../../cocos2d/cocos/platform/CCPlatformMacros.h"




#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

cocos2d::ValueMap getMapForCallback(JNIEnv *env, jobject attributionObject);
AppsFlyerXAppleDeepLinkResult getMapForCallbackDDL(JNIEnv *env, jobject result);
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
        void (*callbackMethod)(AppsFlyerXAppleDeepLinkResult result)) {
    if (NULL == callbackOnDeepLinking) {
        callbackOnDeepLinking = callbackMethod;
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

JNIEXPORT void JNICALL Java_com_appsflyer_AppsFlyer2dXConversionCallback_onDeepLinkingNative
            (JNIEnv *env, jobject obj, jobject result) {

        CCLOG("%s","Java_com_appsflyer_AppsFlyer2dXConversionCallback_onDeepLinkingNative is called");

        if (NULL == callbackOnDeepLinking) {
            return;
        }

    callbackOnDeepLinking(getMapForCallbackDDL(env,result));
    }



cocos2d::ValueMap getMapForCallback(JNIEnv *env, jobject attributionObject) {
    CCLOG("getMapForCallback");
    jclass clsHashMap = env->GetObjectClass(attributionObject);
    cocos2d::ValueMap map;
    jmethodID midKeySet = env->GetMethodID(clsHashMap, "keySet", "()Ljava/util/Set;");

    if (midKeySet == NULL) {
        CCLOG("getMapForCallback method null");
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
    CCLOG("arry size %d", arraySize);
    for (int i=0; i < arraySize; ++i){
        jstring objKey = (jstring) env->GetObjectArrayElement(arrayOfKeys, i);
        const char* c_string_key = env->GetStringUTFChars(objKey, 0);
        jmethodID midGet = env->GetMethodID(clsHashMap, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");
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
    for (auto &t : map){
        CCLOG(" map deep link %s - %s", t.first.c_str(), t.second.asString().c_str());
    }
    return map;
}

AppsFlyerXAppleDeepLinkResult getMapForCallbackDDL(JNIEnv *env, jobject result) {
    AppsFlyerXAppleDeepLinkResult deepLinkResult = AppsFlyerXAppleDeepLinkResult();
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
            //TIMEOUT, NETWORK, HTTP_STATUS_CODE, UNEXPECTED
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
        CCLOG("result DL is not null");
//        deepLinkResult.deepLink = getMapForCallback(env,resultDL);
//    }
        jclass clsDL = env->GetObjectClass(resultDL);
        if (clsDL == NULL)
            return deepLinkResult;

////    CllickEvent getClickEvent()Lorg/json/JSONObject;
//    methodId = env->GetMethodID(clsDL, "getClickEvent", "()Lorg/json/JSONObject;");
//    jobject clickEvent = (jobject) env->CallObjectMethod(resultDL, methodId);
//    if (clickEvent != NULL) {
//       CCLOG("clickevent not null click %s", clickEvent.afSub1)
//    }
//
//    //AfSub2
//    methodId = env->GetMethodID(clsDL, "getAfSub2", "()Ljava/lang/String;");
//    jstring afSub2Str = (jstring) env->CallObjectMethod(resultDL, methodId);
//    // Convert jstring to string
//    if (afSub2Str != NULL) {
//        name_char = env->GetStringUTFChars(afSub2Str, NULL);
//        std::string afSub2 = std::string(name_char);
//        mapDL["afSub2"] = afSub2;
//    }
//
//    //AfSub3
//    methodId = env->GetMethodID(clsDL, "getAfSub3", "()Ljava/lang/String;");
//    jstring afSub3Str = (jstring) env->CallObjectMethod(resultDL, methodId);
//    // Convert jstring to string
//    if (afSub3Str != NULL) {
//        name_char = env->GetStringUTFChars(afSub3Str, NULL);
//        printf("this is hte name 3 %s", name_char);
//        std::string afSub3 = std::string(name_char);
//        mapDL["afSub3"] = afSub3;
//    }
//
//    //AfSub4
//    methodId = env->GetMethodID(clsDL, "getAfSub4", "()Ljava/lang/String;");
//    jstring afSub4Str = (jstring) env->CallObjectMethod(resultDL, methodId);
//    // Convert jstring to string
//    if (afSub4Str != NULL) {
//        name_char = env->GetStringUTFChars(afSub4Str, NULL);
//        std::string afSub4 = std::string(name_char);
//        mapDL["afSub4"] = afSub4;
//    }
//
//    //AfSub5
//    methodId = env->GetMethodID(clsDL, "getAfSub5", "()Ljava/lang/String;");
//    jstring afSub5Str = (jstring) env->CallObjectMethod(resultDL, methodId);
//    // Convert jstring to string
//    if (afSub5Str != NULL) {
//        name_char = env->GetStringUTFChars(afSub5Str, NULL);
//        std::string afSub5 = std::string(name_char);
//        mapDL["afSub5"] = afSub5;
//    }
//
//    //Campagaign
//    methodId = env->GetMethodID(clsDL, "getCampaign", "()Ljava/lang/String;");
//    jstring campaignStr = (jstring) env->CallObjectMethod(resultDL, methodId);
//    // Convert jstring to string
//    if (campaignStr != NULL) {
//        name_char = env->GetStringUTFChars(campaignStr, NULL);
//        std::string campaign = std::string(name_char);
//        mapDL["campaign"] = campaign;
//    }
//
//    //Campaign id
//    methodId = env->GetMethodID(clsDL, "getCampaignId", "()Ljava/lang/String;");
//    jstring campaignIdStr = (jstring) env->CallObjectMethod(resultDL, methodId);
//    // Convert jstring to string
//    if (campaignIdStr != NULL) {
//        name_char = env->GetStringUTFChars(campaignIdStr, NULL);
//        std::string campaignID = std::string(name_char);
//        mapDL["campaign_id"] = campaignID;
//    }
//
//    //click_http_referrer
//    methodId = env->GetMethodID(clsDL, "getClickHttpReferrer", "()Ljava/lang/String;");
//    jstring clickHttpReferrerStr = (jstring) env->CallObjectMethod(resultDL, methodId);
//    // Convert jstring to string
//    if (clickHttpReferrerStr != NULL) {
//        name_char = env->GetStringUTFChars(clickHttpReferrerStr, NULL);
//        std::string clickHttpReferrer = std::string(name_char);
//        mapDL["click_http_referrer"] = clickHttpReferrer;
//    }
//
//    //is_deferred
//    methodId = env->GetMethodID(clsDL, "isDeferred", "()Ljava/lang/Boolean;");
//    jobject isDeferredBoolean =  env->CallObjectMethod(resultDL, methodId);
//    jclass jBooleanClass = env->FindClass("java/lang/Boolean");
//    if (isDeferredBoolean != NULL && env->IsInstanceOf(isDeferredBoolean, jBooleanClass)) {
//        jmethodID booleanValueMID = env->GetMethodID(jBooleanClass, "booleanValue", "()Z");
//        bool booleanValue = (bool) env->CallBooleanMethod(isDeferredBoolean, booleanValueMID);
//        mapDL["is_deferred"] = booleanValue;
//    }
//
//    //Deep link value
//    methodId = env->GetMethodID(clsDL, "getDeepLinkValue", "()Ljava/lang/String;");
//    jstring deep_link_valueStr = (jstring) env->CallObjectMethod(resultDL, methodId);
//    // Convert jstring to string
//    if (deep_link_valueStr != NULL) {
//        name_char = env->GetStringUTFChars(deep_link_valueStr, NULL);
//        std::string deep_link_value = std::string(name_char);
//        mapDL["deep_link_value"] = deep_link_value;
//
//    }
//
//    //media source
//    methodId = env->GetMethodID(clsDL, "getMediaSource", "()Ljava/lang/String;");
//    jstring media_sourceStr = (jstring) env->CallObjectMethod(resultDL, methodId);
//    // Convert jstring to string
//    if (media_sourceStr != NULL) {
//        name_char = env->GetStringUTFChars(media_sourceStr, NULL);
//        std::string media_source = std::string(name_char);
//        mapDL["media_source"] = media_source;
//    }
//
//
//    //match type
//    methodId = env->GetMethodID(clsDL, "getMatchType", "()Ljava/lang/String;");
//    jstring match_typeStr = (jstring) env->CallObjectMethod(resultDL, methodId);
//    // Convert jstring to string
//    if (match_typeStr != NULL) {
//        name_char = env->GetStringUTFChars(match_typeStr, NULL);
//        std::string match_type = std::string(name_char);
//        mapDL["match_type"] = match_type;
//    }
  // deepLinkResult.deepLink = getMapForCallback(env,resultDL);
       // Deep link value
        methodId = env->GetMethodID(clsDL, "toString", "()Ljava/lang/String;");
        if (methodId != NULL) {
            jstring clickEventStr = (jstring) env->CallObjectMethod(resultDL, methodId);
            if (clickEventStr != NULL) {
                CCLOG("click event stringt not null");
                const char *name_char = env->GetStringUTFChars(clickEventStr, NULL);
                std::string click_event = std::string(name_char);
                mapDL["deepLink"] = click_event;
                deepLinkResult.deepLink = stringToMap(click_event);
            }
        }
    }

  // map["deepLink"] = mapDL;





    //Delete
    env->DeleteLocalRef(status);
    env->DeleteLocalRef(error);
//    env->DeleteLocalRef(afSub1Str);
//    env->DeleteLocalRef(afSub2Str);
//    env->DeleteLocalRef(afSub3Str);
//    env->DeleteLocalRef(afSub4Str);
//    env->DeleteLocalRef(afSub5Str);
//    env->DeleteLocalRef(campaignIdStr);
//    env->DeleteLocalRef(campaignStr);
//    env->DeleteLocalRef(media_sourceStr);
//    env->DeleteLocalRef(match_typeStr);
//    env->DeleteLocalRef(deep_link_valueStr);
//    env->DeleteLocalRef(isDeferredBoolean);
//    env->DeleteLocalRef(clickHttpReferrerStr);
    env->DeleteLocalRef(resultDL);

   return deepLinkResult;



}

cocos2d::ValueMap stringToMap(std::string &s) {
    cocos2d::ValueMap m;
    s.erase(std::remove(s.begin(), s.end(), '{'), s.end());
    s.erase(std::remove(s.begin(), s.end(), '}'), s.end());
    std::string key, val;
    std::istringstream iss(s);
    CCLOG("string to map %s", s.c_str());
    while(std::getline(std::getline(iss, key, ':') >> std::ws, val, ',')) {
        CCLOG("string to map key %s", key.c_str());
        CCLOG("string to map val %s", val.c_str());
        m[key] = val;
    }

    for (auto &t : m){
        CCLOG("string to map %s - %s", t.first.c_str(), t.second.asString().c_str());
    }
    return m;
}

#endif

