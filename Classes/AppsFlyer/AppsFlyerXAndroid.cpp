//
// Created by Maxim Shoustin on 10/9/17.
//

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include <jni.h>
#include "stdlib.h"
#include "AppsFlyerXAndroid.h"
#include "AppsFlyerProxyX.h"

std::string afDevKey;
bool isConveriosnListenerInitialized = false;
bool isSubscribedForDeepLink = false;

const char *pluginVersion = "6.13.0";

// Headers
void initConvertionCallback();
void subscribeForDeepLink();

std::string callStringMethod(const char *method_name, const char *descriptor);

void callVoidMethodWithStringParam(const std::string &param, const char *method_name,
                                   const char *descriptor);

void callSetPluginInfo(jobject extensionObject);

void callVoidMethodWithBoolParam(bool param, const char *method_name, const char *descriptor);

void
callVoidMethodWithLongParam(unsigned long param, const char *method_name, const char *descriptor);

jobject valueMapToHashMap(cocos2d::JniMethodInfo jniGetInstance, cocos2d::ValueMap values);


/*
 * AppsFlyerLib.getInstance()
 *  */
cocos2d::JniMethodInfo getAppsFlyerInstance() {

    cocos2d::JniMethodInfo jniGetInstance;

    if (!cocos2d::JniHelper::getStaticMethodInfo(jniGetInstance,
                                                 "com/appsflyer/AppsFlyerLib",
                                                 "getInstance",
                                                 "()Lcom/appsflyer/AppsFlyerLib;")) {
        return jniGetInstance;
    }

    return jniGetInstance;
}

/*
 * AppsFlyerLib.getInstance().enableTCFDataCollection(true);
 */

void AppsFlyerXAndroid::enableTCFDataCollection(bool shouldCollectConsentData) {
    callVoidMethodWithBoolParam(shouldCollectConsentData, "enableTCFDataCollection", "(Z)V");
}

void AppsFlyerXAndroid::setConsentData(const AppsFlyerXConsent& consentData){
    cocos2d::JniMethodInfo jniGetConsentInstance;
    jobject resultConsent;

    if (consentData.IsUserSubjectToGDPR()) {
        bool hasConsentForDataUsage = consentData.HasConsentForDataUsage();
        bool hasConsentForAdsPersonalization = consentData.HasConsentForAdsPersonalization();
        jboolean jHasConsentForDataUsage = (jboolean)hasConsentForDataUsage;
        jboolean jHasConsentForAdsPersonalization = (jboolean)hasConsentForAdsPersonalization;
        if (cocos2d::JniHelper::getStaticMethodInfo(jniGetConsentInstance,
                                                     "com/appsflyer/AppsFlyerConsent",
                                                     "forGDPRUser",
                                                     "(ZZ)Lcom/appsflyer/AppsFlyerConsent;")) {

            resultConsent = (jobject) jniGetConsentInstance.
                    env->CallStaticObjectMethod(jniGetConsentInstance.classID, jniGetConsentInstance.methodID,
                                                jHasConsentForDataUsage,
                                                jHasConsentForAdsPersonalization);
        }
        else{
            CCLOG("%s", "'AppsFlyerConsent' is not loaded");
            return;
        }
    }
    else{
        if (cocos2d::JniHelper::getStaticMethodInfo(jniGetConsentInstance,
                                                     "com/appsflyer/AppsFlyerConsent",
                                                     "forNonGDPRUser",
                                                     "()Lcom/appsflyer/AppsFlyerConsent;")) {
            resultConsent = (jobject) jniGetConsentInstance.
                    env->CallStaticObjectMethod(jniGetConsentInstance.classID, jniGetConsentInstance.methodID);
        }
        else{
            CCLOG("%s", "'AppsFlyerConsent' is not loaded");
            return;
        }
    }
    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {

        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);

        jmethodID methodId = jniGetInstance.env->GetMethodID(cls, "setConsentData", "(Lcom/appsflyer/AppsFlyerConsent;)V");

        jniGetInstance.env->CallVoidMethod(afInstance, methodId, resultConsent);

        jniGetInstance.env->DeleteLocalRef(resultConsent);
        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
    } else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
    }

}

void AppsFlyerXAndroid::stop(bool shouldStop) {

    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {
        CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");

        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);

        jmethodID methodId = jniGetInstance.env->GetMethodID(cls, "stop", "(ZLandroid/content/Context;)V");

        cocos2d::JniMethodInfo miGetContext;
        if (!cocos2d::JniHelper::getStaticMethodInfo(miGetContext, "org/cocos2dx/lib/Cocos2dxActivity", "getContext", "()Landroid/content/Context;")) {
            return;
        }
        jobject jContext = (jobject)miGetContext.env->CallStaticObjectMethod(miGetContext.classID, miGetContext.methodID);


        jniGetInstance.env->CallVoidMethod(afInstance, methodId, shouldStop, jContext);

        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
    } else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
    }

}


void AppsFlyerXAndroid::setIsDebug(bool isDebug) {
    callVoidMethodWithBoolParam(isDebug, "setDebugLog", "(Z)V");
}

void AppsFlyerXAndroid::didEnterBackground(){

    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {
        //CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");

        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);


        cocos2d::JniMethodInfo miGetContext;

        if (!cocos2d::JniHelper::getStaticMethodInfo(miGetContext, "org/cocos2dx/lib/Cocos2dxActivity", "getContext", "()Landroid/content/Context;")) {
            return;
        }
        jobject jContext = (jobject)miGetContext.env->CallStaticObjectMethod(miGetContext.classID, miGetContext.methodID);



        jmethodID methodId = jniGetInstance.env->GetMethodID(cls, "onPause", "(Landroid/content/Context;)V");
        jniGetInstance.env->CallVoidMethod(afInstance, methodId, jContext);

        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
    } else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
    }
}

std::string AppsFlyerXAndroid::customerUserID() {
    return callStringMethod("getCustomerUserId", "()Ljava/lang/String;");
}

void AppsFlyerXAndroid::setCustomerUserID(const std::string &customerUserID) {
    callVoidMethodWithStringParam(customerUserID, "setCustomerUserId", "(Ljava/lang/String;)V");
}

void AppsFlyerXAndroid::setCurrencyCode(const std::string &currencyCode) {
    callVoidMethodWithStringParam(currencyCode, "setCurrencyCode", "(Ljava/lang/String;)V");
}

void AppsFlyerXAndroid::setAppInviteOneLink(const std::string &appInviteOneLinkID) {
    callVoidMethodWithStringParam(appInviteOneLinkID, "setAppInviteOneLink",
                                  "(Ljava/lang/String;)V");
}

void AppsFlyerXAndroid::anonymizeUser(bool shouldAnonymize) {
    callVoidMethodWithBoolParam(shouldAnonymize, "anonymizeUser", "(Z)V");
}

void AppsFlyerXAndroid::setAdditionalData(cocos2d::ValueMap customData) {
    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();
    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {
        //CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");

        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);

        jobject hashMapObj = valueMapToHashMap(jniGetInstance, customData);

        jmethodID methodId = jniGetInstance.env->GetMethodID(cls, "setAdditionalData",
                                                             "(Ljava/util/HashMap;)V");

        jniGetInstance.env->CallVoidMethod(afInstance, methodId, hashMapObj);

        jniGetInstance.env->DeleteLocalRef(hashMapObj);
        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
    } else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
    }
}

void AppsFlyerXAndroid::setHost(const std::string &host) {
    callVoidMethodWithStringParam(host, "setHostName", "(Ljava/lang/String;)V");
}

void AppsFlyerXAndroid::registerUninstall(const std::string &token) {
    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);


    cocos2d::JniMethodInfo miGetContext;

    if (!cocos2d::JniHelper::getStaticMethodInfo(miGetContext, "org/cocos2dx/lib/Cocos2dxActivity",
                                                 "getContext", "()Landroid/content/Context;")) {
        return;
    }
    jobject jContext = (jobject) miGetContext.env->CallStaticObjectMethod(miGetContext.classID,
                                                                          miGetContext.methodID);

    if (NULL != afInstance) {
        //CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");
        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);

        jmethodID methodId = jniGetInstance.env->GetMethodID(cls, "updateServerUninstallToken",
                                                             "(Landroid/content/Context;Ljava/lang/String;)V");

        jstring jtoken = jniGetInstance.env->NewStringUTF(token.c_str());

        jniGetInstance.env->CallVoidMethod(afInstance, methodId, jContext, jtoken);

        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);

        return;
    } else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
        return;
    }
}

void
AppsFlyerXAndroid::setOnConversionDataSuccess(void(*callback)(cocos2d::ValueMap installData)) {

    if (afDevKey.empty()) {
        CCLOGWARN("%s", "AppsFlyer Dev Key is not provided");
        return;
    }

    setAttributionCallbackOnConversionDataReceived(callback);

    if (!isConveriosnListenerInitialized) {
        initConvertionCallback();
    }
}

void
AppsFlyerXAndroid::setOnAppOpenAttribution(void(*callback)(cocos2d::ValueMap attributionData)) {
    if (afDevKey.empty()) {
        CCLOGWARN("%s", "AppsFlyer Dev Key is not provided");
        return;
    }

    setAttributionCallbackOnAppOpenAttribution(callback);

    if (!isConveriosnListenerInitialized) {
        initConvertionCallback();
    }

}

void
AppsFlyerXAndroid::setOnConversionDataFail(void(*callback)(cocos2d::ValueMap error)) {
    if (afDevKey.empty()) {
        CCLOGWARN("%s", "AppsFlyer Dev Key is not provided");
        return;
    }

    setAttributionCallbackOnConversionDataRequestFailure(callback);

    if (!isConveriosnListenerInitialized) {
        initConvertionCallback();
    }
}

void AppsFlyerXAndroid::setOnAppOpenAttributionFailure(void(*callback)(cocos2d::ValueMap error)) {
    if (afDevKey.empty()) {
        CCLOGWARN("%s", "AppsFlyer Dev Key is not provided");
        return;
    }

    setAttributionCallbackOnAppOpenAttributionFailure(callback);

    if (!isConveriosnListenerInitialized) {
        initConvertionCallback();
    }
}

void AppsFlyerXAndroid::start() {

    if (afDevKey.empty()) {
        CCLOGWARN("%s", "AppsFlyer Dev Key is not provided");
        return;
    }

    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    //AppsFlyerLib afLib instance
    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {
        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);

        cocos2d::JniMethodInfo jniGetContext;

        if (!cocos2d::JniHelper::getStaticMethodInfo(jniGetContext,
                                                     "org/cocos2dx/lib/Cocos2dxActivity",
                                                     "getContext",
                                                     "()Landroid/content/Context;")) {
            return;
        }

        jobject jContext = (jobject) jniGetContext.env->CallStaticObjectMethod(
                jniGetContext.classID, jniGetContext.methodID);


        jstring jAppsFlyerDevKey = jniGetInstance.env->NewStringUTF(afDevKey.c_str());

        jmethodID startTrackingMethodId = jniGetInstance.env->GetMethodID(cls,
                                                                          "start",
                                                                          "(Landroid/content/Context;Ljava/lang/String;)V");

        // This is what we actually do: afLib.startTracking((Application) context.getApplicationContext())
        jniGetInstance.env->CallVoidMethod(afInstance, startTrackingMethodId, jContext,
                                           jAppsFlyerDevKey);

        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
    } else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
    }
}

void callSetPluginInfo(jobject extensionObject) {
    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {
        //CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");

        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);

        jmethodID methodId = jniGetInstance.env->GetMethodID(cls, "setPluginInfo", "(Lcom/appsflyer/internal/platform_extension/PluginInfo;)V");
        jniGetInstance.env->CallVoidMethod(afInstance, methodId, extensionObject);

        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
    } else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
    }
}

void AppsFlyerXAndroid::logEvent(const std::string &eventName, cocos2d::ValueMap values) {

    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {
        //CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");

        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);


        cocos2d::JniMethodInfo jniGetContext;

        if (!cocos2d::JniHelper::getStaticMethodInfo(jniGetContext,
                                                     "org/cocos2dx/lib/Cocos2dxActivity",
                                                     "getContext",
                                                     "()Landroid/content/Context;")) {
            return;
        }


        jobject jContext = (jobject) jniGetContext.env->CallStaticObjectMethod(
                jniGetContext.classID, jniGetContext.methodID);

        jobject hashMapObj = valueMapToHashMap(jniGetInstance, values);

        jmethodID methodId = jniGetInstance.env->GetMethodID(cls, "logEvent",
                                                             "(Landroid/content/Context;Ljava/lang/String;Ljava/util/Map;)V");

        jstring jEventName = jniGetInstance.env->NewStringUTF(eventName.c_str());

        jniGetInstance.env->CallVoidMethod(afInstance, methodId, jContext, jEventName, hashMapObj);

        jniGetInstance.env->DeleteLocalRef(hashMapObj);
        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
    } else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
    }
}


void AppsFlyerXAndroid::logAdRevenue(const AFXAdRevenueData &adRevenueData, cocos2d::ValueMap params) {
    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    std::string appsFlyerUid;

    cocos2d::JniMethodInfo miGetContext;

    if (!cocos2d::JniHelper::getStaticMethodInfo(miGetContext, "org/cocos2dx/lib/Cocos2dxActivity",
                                                 "getContext", "()Landroid/content/Context;")) {
        return;
    }
    jobject jContext = (jobject) miGetContext.env->CallStaticObjectMethod(miGetContext.classID,
                                                                          miGetContext.methodID);

    jobject hashMapObj = valueMapToHashMap(jniGetInstance, params);

    if (NULL != afInstance) {
        // CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");
        jclass afAdRevenueDataClass = jniGetInstance.env->FindClass(
                "com/appsflyer/AFAdRevenueData");
        jmethodID afAdRevenueDataMethodId = jniGetInstance.env->GetMethodID(afAdRevenueDataClass,
                                                                    "<init>",
                                                                    "(Ljava/lang/String;Lcom/appsflyer/MediationNetwork;Ljava/lang/String;D)V");

        jclass mediationNetworkClass = jniGetInstance.env->FindClass("com/appsflyer/MediationNetwork");

        // Get the enum value by name (e.g., "IRONSOURCE", "APPLOVIN_MAX")
        jmethodID valueOfMethod = jniGetInstance.env->GetStaticMethodID(mediationNetworkClass, "valueOf", "(Ljava/lang/String;)Lcom/appsflyer/MediationNetwork;");

        // Check if the valueOf method was found
        if (valueOfMethod == nullptr) {
            // Handle error, method not found
            return;
        }

        jstring enumNameString = jniGetInstance.env->NewStringUTF(adRevenueData.meditationNetworkString().c_str());

        jobject jMediationNetworkEnumObject = jniGetInstance.env->CallStaticObjectMethod(mediationNetworkClass, valueOfMethod, enumNameString);
        jobject jMonetizationNetwork = jniGetInstance.env->NewStringUTF(adRevenueData.getMonetizationNetwork().c_str());
        jobject jCurrencyCode = jniGetInstance.env->NewStringUTF(adRevenueData.getCurrencyIso4217Code().c_str());

        jobject afAdRevenueDataObject = jniGetInstance.env->NewObject(afAdRevenueDataClass, afAdRevenueDataMethodId,
                                                                      jMonetizationNetwork,
                                                                      jMediationNetworkEnumObject,
                                                                      jCurrencyCode,
                                                                      adRevenueData.getEventRevenue());

        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);

        jmethodID methodId = jniGetInstance.env->GetMethodID(cls, "logAdRevenue",
                                                             "(Lcom/appsflyer/AFAdRevenueData;"
                                                             "Ljava/util/Map;)V");

        jniGetInstance.env->CallVoidMethod(afInstance, methodId, afAdRevenueDataObject, hashMapObj);

        // cleanup
        jniGetInstance.env->DeleteLocalRef(enumNameString);
        jniGetInstance.env->DeleteLocalRef(afAdRevenueDataObject);
        jniGetInstance.env->DeleteLocalRef(jMediationNetworkEnumObject);
        jniGetInstance.env->DeleteLocalRef(jMonetizationNetwork);
        jniGetInstance.env->DeleteLocalRef(jCurrencyCode);
        jniGetInstance.env->DeleteLocalRef(afInstance);
    } else {
        CCLOGWARN("com/appsflyer/AppsFlyerLib is not loaded");
    }
}

void AppsFlyerXAndroid::setUserEmails(std::vector<std::string> userEmails, EmailCryptTypeX type) {

}

void AppsFlyerXAndroid::setAppsFlyerDevKey(const std::string &appsFlyerDevKey) {
    afDevKey = appsFlyerDevKey;

    if (afDevKey.empty()) {
        CCLOGWARN("%s", "AppsFlyer Dev Key is not provided");
        return;
    }

    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    //AppsFlyerLib afLib instance
    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {
        //CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");

        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);

        jclass clsExtension = jniGetInstance.env->FindClass(
                "com/appsflyer/internal/platform_extension/PluginInfo");


        jmethodID extensionConstructor = jniGetInstance.env->GetMethodID(clsExtension,
                                                                         "<init>",
                                                                         "(Lcom/appsflyer/internal/platform_extension/Plugin;Ljava/lang/String;)V");

        jclass enumClass = jniGetInstance.env->FindClass(
                "com/appsflyer/internal/platform_extension/Plugin");
        jfieldID fid = jniGetInstance.env->GetStaticFieldID(enumClass, "COCOS_2DX",
                                                            "Lcom/appsflyer/internal/platform_extension/Plugin;");
        jobject plugin = jniGetInstance.env->GetStaticObjectField(enumClass, fid);
        jstring version = jniGetInstance.env->NewStringUTF(pluginVersion);
        jobject extensionObject = jniGetInstance.env->NewObject(clsExtension, extensionConstructor,
                                                                plugin, version);

        callSetPluginInfo(extensionObject);


        cocos2d::JniMethodInfo jniGetContext;

        if (!cocos2d::JniHelper::getStaticMethodInfo(jniGetContext,
                                                     "org/cocos2dx/lib/Cocos2dxActivity",
                                                     "getContext",
                                                     "()Landroid/content/Context;")) {
            return;
        }

        jobject jContext = (jobject) jniGetContext.env->CallStaticObjectMethod(
                jniGetContext.classID, jniGetContext.methodID);


        jstring jAppsFlyerDevKey = jniGetInstance.env->NewStringUTF(afDevKey.c_str());

        // call Init if no GCD registered
        if (!isConveriosnListenerInitialized) {
            jmethodID initMethodId = jniGetInstance.env->GetMethodID(cls,
                                                                     "init",
                                                                     "(Ljava/lang/String;Lcom/appsflyer/AppsFlyerConversionListener;Landroid/content/Context;)Lcom/appsflyer/AppsFlyerLib;");

            // This is what we actually do: afLib.init(appsFlyerDevKey, null)
            jniGetInstance.env->CallObjectMethod(afInstance, initMethodId, jAppsFlyerDevKey, NULL,
                                                 jContext);

            jniGetInstance.env->DeleteLocalRef(afInstance);
            jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
        }
    }
    else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
    }
}

std::string AppsFlyerXAndroid::appsFlyerDevKey() {
    return afDevKey;
}


std::string AppsFlyerXAndroid::getHost() {
    return callStringMethod("getHost", "()Ljava/lang/String;");
}

void AppsFlyerXAndroid::setMinTimeBetweenSessions(unsigned long minTimeBetweenSessions) {
    callVoidMethodWithLongParam(minTimeBetweenSessions, "setMinTimeBetweenSessions", "(I)V");
}

std::string AppsFlyerXAndroid::getAppsFlyerUID() {

    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    std::string appsFlyerUid;

    cocos2d::JniMethodInfo miGetContext;

    if (!cocos2d::JniHelper::getStaticMethodInfo(miGetContext, "org/cocos2dx/lib/Cocos2dxActivity",
                                                 "getContext", "()Landroid/content/Context;")) {
        return appsFlyerUid;
    }
    jobject jContext = (jobject) miGetContext.env->CallStaticObjectMethod(miGetContext.classID,
                                                                          miGetContext.methodID);

    if (NULL != afInstance) {
        //CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");
        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);

        jmethodID methodId = jniGetInstance.env->GetMethodID(cls, "getAppsFlyerUID",
                                                             "(Landroid/content/Context;)Ljava/lang/String;");
        // Get customer ID as a jstinrg
        jstring userId = (jstring) jniGetInstance.env->CallObjectMethod(afInstance, methodId,
                                                                        jContext);

        // Convert jstring tio string
        const char *name_char = jniGetInstance.env->GetStringUTFChars(userId, NULL);
        appsFlyerUid = std::string(name_char);

        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);

        return appsFlyerUid;
    } else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
        return NULL;
    }
}

void AppsFlyerXAndroid::validateAndLogInAppPurchase(const std::string &publicKey,
                                                      const std::string &signature,
                                                      const std::string &purchaseData,
                                                      const std::string &price,
                                                      const std::string &currency,
                                                      cocos2d::ValueMap additionalParameters) {

    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    std::string appsFlyerUid;

    cocos2d::JniMethodInfo miGetContext;

    if (!cocos2d::JniHelper::getStaticMethodInfo(miGetContext, "org/cocos2dx/lib/Cocos2dxActivity",
                                                 "getContext", "()Landroid/content/Context;")) {
        return;
    }
    jobject jContext = (jobject) miGetContext.env->CallStaticObjectMethod(miGetContext.classID,
                                                                          miGetContext.methodID);

    // Convert string params to jstring
    jstring jPublicKey = jniGetInstance.env->NewStringUTF(publicKey.c_str());
    jstring jSignature = jniGetInstance.env->NewStringUTF(signature.c_str());
    jstring jPurchaseData = jniGetInstance.env->NewStringUTF(purchaseData.c_str());
    jstring jPrice = jniGetInstance.env->NewStringUTF(price.c_str());
    jstring jCurrency = jniGetInstance.env->NewStringUTF(currency.c_str());

    // Convert ValueMap to HashSet

    jobject hashMapObj = valueMapToHashMap(jniGetInstance, additionalParameters);

    if (NULL != afInstance) {
       // CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");

        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);

        jmethodID methodId = jniGetInstance.env->GetMethodID(cls, "validateAndLogInAppPurchase",
                                                             "(Landroid/content/Context;"
                                                                     "Ljava/lang/String;"
                                                                     "Ljava/lang/String;"
                                                                     "Ljava/lang/String;"
                                                                     "Ljava/lang/String;"
                                                                     "Ljava/lang/String;"
                                                                     "Ljava/util/Map;)V");

        jniGetInstance.env->CallVoidMethod(afInstance, methodId, jContext, jPublicKey, jSignature,
                                           jPurchaseData, jPrice, jCurrency, hashMapObj);

        jniGetInstance.env->DeleteLocalRef(hashMapObj);
        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
    } else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
    }
}

void AppsFlyerXAndroid::validateAndLogInAppPurchase(AFSDKXPurchaseDetails &details, cocos2d::ValueMap params, std::function<void(AFSDKXValidateAndLogResult)> completionHandler) {
    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    cocos2d::JniMethodInfo miGetContext;

    if (!cocos2d::JniHelper::getStaticMethodInfo(miGetContext, "org/cocos2dx/lib/Cocos2dxActivity",
                                                 "getContext", "()Landroid/content/Context;")) {
        return;
    }
    jobject jContext = (jobject) miGetContext.env->CallStaticObjectMethod(miGetContext.classID,
                                                                          miGetContext.methodID);

    std::string jPurchaseTypeString = details.getPurchaseType();
    jstring jPurchaseToken = jniGetInstance.env->NewStringUTF(details.getPurchaseToken().c_str());
    jstring jProductId = jniGetInstance.env->NewStringUTF(details.getProductId().c_str());
    jstring jPrice = jniGetInstance.env->NewStringUTF(details.getPrice().c_str());
    jstring jCurrency = jniGetInstance.env->NewStringUTF(details.getCurrency().c_str());

    jobject hashMapObj = valueMapToHashMap(jniGetInstance, params);


    if (NULL != afInstance) {
        jclass afPurchaseDetailsClass = jniGetInstance.env->FindClass("com/appsflyer/AFPurchaseDetails");
        jmethodID purchaseDetails = jniGetInstance.env->GetMethodID(afPurchaseDetailsClass, "<init>",
                                                 "(Lcom/appsflyer/AFPurchaseType;"
                                                 "Ljava/lang/String;"
                                                 "Ljava/lang/String;"
                                                 "Ljava/lang/String;"
                                                 "Ljava/lang/String;)V");

        jclass afPurchaseTypeClass = jniGetInstance.env->FindClass("com/appsflyer/AFPurchaseType");

        jmethodID afPurchaseTypeMethodId = jniGetInstance.env->GetStaticMethodID(afPurchaseTypeClass, "values", "()[Lcom/appsflyer/AFPurchaseType;");
        jobjectArray afPurchaseTypeArray = (jobjectArray) jniGetInstance.env->CallStaticObjectMethod(afPurchaseTypeClass, afPurchaseTypeMethodId);
        if (afPurchaseTypeArray == nullptr) {
            CCLOG("Error: Call to values() returned null.");
            return;
        }

        jsize arrayLength = jniGetInstance.env->GetArrayLength(afPurchaseTypeArray);

        jobject selectedPurchaseType;
        if (jPurchaseTypeString.compare("subscription") == 0) {
            CCLOG("Purchase type is 'subscription'.");
            selectedPurchaseType = jniGetInstance.env->GetObjectArrayElement(afPurchaseTypeArray, 0);
            CCLOG("First enum constant (SUBSCRIPTION): %p", selectedPurchaseType);
        } else if (jPurchaseTypeString.compare("one_time_purchase") == 0) {
            CCLOG("Purchase type is 'one_time_purchase'.");
            selectedPurchaseType = jniGetInstance.env->GetObjectArrayElement(afPurchaseTypeArray, 1);
            CCLOG("Second enum constant (ONE_TIME_PURCHASE): %p", selectedPurchaseType);
        } else {
            CCLOG("Error: Unknown purchase type.");
            return;
        }

        CCLOG("afPurchaseTypeClass: %p", afPurchaseTypeClass);
        jobject jPurchaseDetails = jniGetInstance.env->NewObject(afPurchaseDetailsClass,
                                                                 purchaseDetails,
                                                                 selectedPurchaseType,
                                                                 jPurchaseToken,
                                                                 jProductId,
                                                                 jPrice,
                                                                 jCurrency);

        CCLOG("Purchase details: %p", jPurchaseDetails);
        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);
        jmethodID methodId = jniGetInstance.env->GetMethodID(
                cls,
                "validateAndLogInAppPurchase",
                "(Lcom/appsflyer/AFPurchaseDetails;"
                "Ljava/util/Map;"
                "Lcom/appsflyer/AppsFlyerInAppPurchaseValidationCallback;)V"
        );

        jniGetInstance.env->CallVoidMethod(afInstance, methodId, jPurchaseDetails, hashMapObj,
                                           nullptr);

        jniGetInstance.env->DeleteLocalRef(hashMapObj);
        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);

        jniGetInstance.env->DeleteLocalRef(jPurchaseToken);
        jniGetInstance.env->DeleteLocalRef(jProductId);
        jniGetInstance.env->DeleteLocalRef(jPrice);
        jniGetInstance.env->DeleteLocalRef(jCurrency);
    } else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
    }
}

//Sharing data filter
void AppsFlyerXAndroid::sharingFilter(std::vector<std::string> partners){
    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {
        //CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");

        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);


        cocos2d::JniMethodInfo jniGetContext;

        if (!cocos2d::JniHelper::getStaticMethodInfo(jniGetContext,
                                                     "org/cocos2dx/lib/Cocos2dxActivity",
                                                     "getContext",
                                                     "()Landroid/content/Context;")) {
            return;
        }


        jobjectArray result;
        result = (jobjectArray)jniGetInstance.env->NewObjectArray(partners.size(),jniGetInstance.env->FindClass("java/lang/String"),jniGetInstance.env->NewStringUTF(""));
        for(int i=0; i<partners.size(); i++) {
            jniGetInstance.env->SetObjectArrayElement(result,i,jniGetInstance.env->NewStringUTF(partners[i].c_str()));
        }

        jmethodID methodId = jniGetInstance.env->GetMethodID(cls, "setSharingFilter",
                                                             "([Ljava/lang/String;)V");
//        jobject jContext = (jobject) jniGetContext.env->CallStaticObjectMethod(
//                jniGetContext.classID, jniGetContext.methodID);




        jniGetInstance.env->CallVoidMethod(afInstance, methodId, result);

        jniGetInstance.env->DeleteLocalRef(result);
        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
    } else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
    }
}

void AppsFlyerXAndroid::sharingFilterForAllPartners(){
    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {
        //CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");

        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);


        cocos2d::JniMethodInfo jniGetContext;

        if (!cocos2d::JniHelper::getStaticMethodInfo(jniGetContext,
                                                     "org/cocos2dx/lib/Cocos2dxActivity",
                                                     "getContext",
                                                     "()Landroid/content/Context;")) {
            return;
        }


        jmethodID methodId = jniGetInstance.env->GetMethodID(cls, "setSharingFilterForAllPartners",
                                                             "()V");
//        jobject jContext = (jobject) jniGetContext.env->CallStaticObjectMethod(
//                jniGetContext.classID, jniGetContext.methodID);




        jniGetInstance.env->CallVoidMethod(afInstance, methodId);


        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
    } else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
    }
}

void AppsFlyerXAndroid::setOnDeepLinking(void(*callback)(AppsFlyerXDeepLinkResult result)) {

    if (!isSubscribedForDeepLink) {
        subscribeForDeepLink();
    }

    setCallbackOnDeepLinking(callback);

}

void AppsFlyerXAndroid::setPartnerData(const std::string& partnerId, cocos2d::ValueMap data){
    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);


    if (NULL != afInstance) {
        //CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");

        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);


        cocos2d::JniMethodInfo jniGetContext;

            if (!cocos2d::JniHelper::getStaticMethodInfo(jniGetContext,
                                                         "org/cocos2dx/lib/Cocos2dxActivity",
                                                         "getContext",
                                                         "()Landroid/content/Context;")) {
                return;
            }


            jobject jContext = (jobject) jniGetContext.env->CallStaticObjectMethod(
                    jniGetContext.classID, jniGetContext.methodID);


        jobject hashMapObj = valueMapToHashMap(jniGetInstance, data);
        jstring jpartnerId = jniGetInstance.env->NewStringUTF(partnerId.c_str());

        jmethodID methodId = jniGetInstance.env->GetMethodID(cls, "setPartnerData",
                                                             "(Ljava/lang/String;Ljava/util/Map;)V");





        jniGetInstance.env->CallVoidMethod(afInstance, methodId, jpartnerId, hashMapObj);

        jniGetInstance.env->DeleteLocalRef(hashMapObj);
        jniGetInstance.env->DeleteLocalRef(jpartnerId);
        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
    } else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
    }
}



//================== Utils ==================//

void callVoidMethodWithStringParam(const std::string &param, const char *method_name,
                                   const char *descriptor) {
    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {
        //CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");

        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);

        jmethodID methodId = jniGetInstance.env->GetMethodID(cls, method_name, descriptor);
        jstring customerId = jniGetInstance.env->NewStringUTF(param.c_str());
        jniGetInstance.env->CallVoidMethod(afInstance, methodId, customerId);

        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
    } else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
    }
}

void callVoidMethodWithBoolParam(bool param, const char *method_name, const char *descriptor) {
    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {
        CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");

        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);

        jmethodID methodId = jniGetInstance.env->GetMethodID(cls, method_name, descriptor);
        jniGetInstance.env->CallVoidMethod(afInstance, methodId, param);

        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
    } else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
    }
}

void
callVoidMethodWithLongParam(unsigned long param, const char *method_name, const char *descriptor) {
    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {
        //CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");

        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);

        jmethodID methodId = jniGetInstance.env->GetMethodID(cls, method_name, descriptor);
        jniGetInstance.env->CallVoidMethod(afInstance, methodId, param);

        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
    } else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
    }
}

std::string callStringMethod(const char *method_name, const char *descriptor) {
    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {
        //CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");
        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);

        jmethodID methodId = jniGetInstance.env->GetMethodID(cls, method_name, descriptor);
        // Get customer ID as a jstinrg
        jstring userId = (jstring) jniGetInstance.env->CallObjectMethod(afInstance, methodId);

        // Convert jstring tio string
        const char *name_char = jniGetInstance.env->GetStringUTFChars(userId, NULL);
        std::string name = std::string(name_char);

        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);

        return name;
    } else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
        return NULL;
    }
}

jobject valueMapToHashMap(cocos2d::JniMethodInfo jniGetInstance, cocos2d::ValueMap values) {
    jclass hashMapClass = jniGetInstance.env->FindClass("java/util/HashMap");

    jmethodID hashMapInit = jniGetInstance.env->GetMethodID(hashMapClass, "<init>", "(I)V");

    jobject hashMapObj = jniGetInstance.env->NewObject(hashMapClass, hashMapInit,
                                                       values.size());

    //javap -s -p java.util.HashMap  | grep -A 1 put\(
    jmethodID hashMapId = jniGetInstance.env->GetMethodID(hashMapClass, "put",
                                                          "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

    //jniGetInstance.env->CallObjectMethod(hashMapObj, hashMapId,jniGetInstance.env->NewStringUTF("xxx"),jniGetInstance.env->NewStringUTF("ssss"));

    for (auto it : values) {   //https://github.com/cocos2d/cocos2d-x/blob/v3/cocos/base/CCValue.h
        switch (it.second.getType()) {
            case cocos2d::Value::Type::STRING:
                jniGetInstance.env->CallObjectMethod(hashMapObj, hashMapId,
                                                     jniGetInstance.env->NewStringUTF(
                                                             it.first.c_str()),
                                                     jniGetInstance.env->NewStringUTF(
                                                             it.second.asString().c_str()));
                break;

            case cocos2d::Value::Type::BOOLEAN: {
                jclass jBooleanClass = jniGetInstance.env->FindClass("java/lang/Boolean");
                jmethodID jValueOfId = jniGetInstance.env->GetStaticMethodID(jBooleanClass,
                                                                             "valueOf",
                                                                             "(Z)Ljava/lang/Boolean;");
                jobject jBoolValue = (jobject) jniGetInstance.env->CallStaticObjectMethod(
                        jBooleanClass, jValueOfId, it.second.asBool());

                jniGetInstance.env->CallObjectMethod(hashMapObj, hashMapId,
                                                     jniGetInstance.env->NewStringUTF(
                                                             it.first.c_str()), jBoolValue);

                jniGetInstance.env->DeleteLocalRef(jBoolValue);
            }
                break;
            case cocos2d::Value::Type::INTEGER: {
                jclass jIntegerClass = jniGetInstance.env->FindClass("java/lang/Integer");
                jmethodID jValueOfId = jniGetInstance.env->GetStaticMethodID(jIntegerClass,
                                                                             "valueOf",
                                                                             "(I)Ljava/lang/Integer;");
                jobject jIntegerValue = (jobject) jniGetInstance.env->CallStaticObjectMethod(
                        jIntegerClass, jValueOfId, it.second.asInt());

                jniGetInstance.env->CallObjectMethod(hashMapObj, hashMapId,
                                                     jniGetInstance.env->NewStringUTF(
                                                             it.first.c_str()), jIntegerValue);

                jniGetInstance.env->DeleteLocalRef(jIntegerValue);
            }
                break;
            case cocos2d::Value::Type::DOUBLE: {
                jclass jDoubleClass = jniGetInstance.env->FindClass("java/lang/Double");
                jmethodID jValueOfId = jniGetInstance.env->GetStaticMethodID(jDoubleClass,
                                                                             "valueOf",
                                                                             "(D)Ljava/lang/Double;");
                jobject jDoubleValue = (jobject) jniGetInstance.env->CallStaticObjectMethod(
                        jDoubleClass, jValueOfId, it.second.asDouble());

                jniGetInstance.env->CallObjectMethod(hashMapObj, hashMapId,
                                                     jniGetInstance.env->NewStringUTF(
                                                             it.first.c_str()), jDoubleValue);

                jniGetInstance.env->DeleteLocalRef(jDoubleValue);
            }
                break;
            case cocos2d::Value::Type::VECTOR: {
                jclass jArrayListClass = jniGetInstance.env->FindClass("java/util/ArrayList");

                jobject obj = jniGetInstance.env->NewObject(jArrayListClass,
                                                            jniGetInstance.env->GetMethodID(
                                                                    jArrayListClass, "<init>",
                                                                    "()V"));

                cocos2d::ValueVector &vector = it.second.asValueVector();

                jclass jIntegerClass = jniGetInstance.env->FindClass("java/lang/Integer");
                jmethodID jValueOfInt = jniGetInstance.env->GetStaticMethodID(jIntegerClass,
                                                                             "valueOf",
                                                                             "(I)Ljava/lang/Integer;");

                jclass jDoubleClass = jniGetInstance.env->FindClass("java/lang/Double");
                jmethodID jValueOfDouble = jniGetInstance.env->GetStaticMethodID(jDoubleClass,
                                                                             "valueOf",
                                                                             "(D)Ljava/lang/Double;");

                jmethodID met = jniGetInstance.env->GetMethodID(
                        jArrayListClass, "add", "(Ljava/lang/Object;)Z");

                for (int n = 0; n < vector.size(); n++) {
                    if (vector[n].getType() == cocos2d::Value::Type::INTEGER) {
                        // Get Integer value
                        int integer_value = (vector[n]).asInt();

                        // Convert int to INTEGER
                        jobject jIntegerValue = (jobject) jniGetInstance.env->CallStaticObjectMethod(
                                jIntegerClass, jValueOfInt, integer_value);

                        // Call to "add" method
                        jniGetInstance.env->CallBooleanMethod(obj, met, jIntegerValue);


                    } else if (vector[n].getType() == cocos2d::Value::Type::DOUBLE) {
                        // Get Double value
                        double double_value = (vector[n]).asDouble();

                        // Convert double to DOUBLE
                        jobject jDoubleValue = (jobject) jniGetInstance.env->CallStaticObjectMethod(
                                jIntegerClass, jValueOfDouble, double_value);


                        // Call to "add" method
                        jniGetInstance.env->CallBooleanMethod(obj, met, jDoubleValue);
                    } else if (vector[n].getType() == cocos2d::Value::Type::STRING) {

                        jobject jstr =  jniGetInstance.env->NewStringUTF(vector[n].asString().c_str());

                        jniGetInstance.env->CallBooleanMethod(obj, met, jstr);
                    }
                }

                jniGetInstance.env->CallObjectMethod(hashMapObj, hashMapId,
                                                     jniGetInstance.env->NewStringUTF(
                                                             it.first.c_str()), obj);
                jniGetInstance.env->DeleteLocalRef(obj);
            }
                break;
            default:
                break;
        }
    }

    return hashMapObj;
}

// Initialize AppsFlyerLib with conversion Callback
void initConvertionCallback() {
    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    //AppsFlyerLib afLib instance
    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {

        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);

        cocos2d::JniMethodInfo jniInit;
        // get  'new AppsFlyer2dXConversionCallback()'
        if (!cocos2d::JniHelper::getMethodInfo(jniInit,
                                               "com/appsflyer/AppsFlyer2dXConversionCallback",
                                               "<init>", "()V")) {
            return;
        }

        cocos2d::JniMethodInfo jniGetContext;

        if (!cocos2d::JniHelper::getStaticMethodInfo(jniGetContext,
                                                     "org/cocos2dx/lib/Cocos2dxActivity",
                                                     "getContext",
                                                     "()Landroid/content/Context;")) {
            return;
        }

        jobject jContext = (jobject) jniGetContext.env->CallStaticObjectMethod(
                jniGetContext.classID, jniGetContext.methodID);



        jclass clsAppsFlyer2dXConversionCallback = jniInit.env->FindClass(
                "com/appsflyer/AppsFlyer2dXConversionCallback");


        jmethodID callbackMethodId = jniInit.env->GetMethodID(clsAppsFlyer2dXConversionCallback,
                                                              "<init>", "()V");
        // instance of 'new AppsFlyer2dXConversionCallback()'
        jobject jCallbackProxy = jniInit.env->NewObject(clsAppsFlyer2dXConversionCallback,
                                                        callbackMethodId);

        jmethodID initMethodId = jniGetInstance.env->GetMethodID(cls,
                                                                 "init",
                                                                 "(Ljava/lang/String;Lcom/appsflyer/AppsFlyerConversionListener;Landroid/content/Context;)Lcom/appsflyer/AppsFlyerLib;");

        jstring jAppsFlyerDevKey = jniGetInstance.env->NewStringUTF(afDevKey.c_str());

        // Conversion Callback is initialized
        isConveriosnListenerInitialized = true;

        // This is what we actually do: afLib.init(appsFlyerDevKey, conversionDataListener, context)
        jniGetInstance.env->CallObjectMethod(afInstance, initMethodId,
                                            jAppsFlyerDevKey, jCallbackProxy, jContext);

        jniInit.env->DeleteLocalRef(jCallbackProxy);
        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
    } else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
    }
}

void subscribeForDeepLink() {
    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    //AppsFlyerLib afLib instance
    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {

        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);

        cocos2d::JniMethodInfo jniInit;
        // get  'new AppsFlyer2dXConversionCallback()'
        if (!cocos2d::JniHelper::getMethodInfo(jniInit,
                                               "com/appsflyer/AppsFlyer2dXConversionCallback",
                                               "<init>", "()V")) {
            return;
        }

        cocos2d::JniMethodInfo jniGetContext;

        if (!cocos2d::JniHelper::getStaticMethodInfo(jniGetContext,
                                                     "org/cocos2dx/lib/Cocos2dxActivity",
                                                     "getContext",
                                                     "()Landroid/content/Context;")) {
            return;
        }

        jobject jContext = (jobject) jniGetContext.env->CallStaticObjectMethod(
                jniGetContext.classID, jniGetContext.methodID);



        jclass clsAppsFlyer2dXConversionCallback = jniInit.env->FindClass(
                "com/appsflyer/AppsFlyer2dXConversionCallback");


        jmethodID callbackMethodId = jniInit.env->GetMethodID(clsAppsFlyer2dXConversionCallback,
                                                              "<init>", "()V");
        // instance of 'new AppsFlyer2dXConversionCallback()'
        jobject jCallbackProxy = jniInit.env->NewObject(clsAppsFlyer2dXConversionCallback,
                                                        callbackMethodId);


        jmethodID subscribeMethodId = jniGetInstance.env->GetMethodID(cls,
                                                                      "subscribeForDeepLink",
                                                                      "(Lcom/appsflyer/deeplink/DeepLinkListener;)V");


        isSubscribedForDeepLink = true;
        // This is what we actually do: afLib.init(appsFlyerDevKey, conversionDataListener, context)
        jniGetInstance.env->CallVoidMethod(afInstance, subscribeMethodId,
                                           jCallbackProxy);


        jniInit.env->DeleteLocalRef(jCallbackProxy);
        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
    } else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
    }
}

    void AppsFlyerXAndroid::setOneLinkCustomDomains(std::vector<std::string> domains){
        cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

        jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
                jniGetInstance.classID, jniGetInstance.methodID);

        if (NULL != afInstance) {
            //CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");

            jclass cls = jniGetInstance.env->GetObjectClass(afInstance);


            cocos2d::JniMethodInfo jniGetContext;

            if (!cocos2d::JniHelper::getStaticMethodInfo(jniGetContext,
                                                         "org/cocos2dx/lib/Cocos2dxActivity",
                                                         "getContext",
                                                         "()Landroid/content/Context;")) {
                return;
            }


            jobjectArray result;
            result = (jobjectArray)jniGetInstance.env->NewObjectArray(domains.size(),jniGetInstance.env->FindClass("java/lang/String"),jniGetInstance.env->NewStringUTF(""));
            for(int i=0; i<domains.size(); i++) {
                jniGetInstance.env->SetObjectArrayElement(result,i,jniGetInstance.env->NewStringUTF(domains[i].c_str()));
            }

            jmethodID methodId = jniGetInstance.env->GetMethodID(cls, "setOneLinkCustomDomain",
                                                                 "([Ljava/lang/String;)V");


            jniGetInstance.env->CallVoidMethod(afInstance, methodId, result);

            jniGetInstance.env->DeleteLocalRef(result);
            jniGetInstance.env->DeleteLocalRef(afInstance);
            jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
        } else {
            CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
        }
}

    void AppsFlyerXAndroid::setDisableAdvertisingIdentifiers(bool disable){
        cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

        jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
                jniGetInstance.classID, jniGetInstance.methodID);

        if (NULL != afInstance) {
            CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");

            jclass cls = jniGetInstance.env->GetObjectClass(afInstance);

            jmethodID methodId = jniGetInstance.env->GetMethodID(cls, "setDisableAdvertisingIdentifiers", "(Z)V");

            cocos2d::JniMethodInfo miGetContext;
            if (!cocos2d::JniHelper::getStaticMethodInfo(miGetContext, "org/cocos2dx/lib/Cocos2dxActivity", "getContext", "()Landroid/content/Context;")) {
                return;
            }
            jobject jContext = (jobject)miGetContext.env->CallStaticObjectMethod(miGetContext.classID, miGetContext.methodID);


            jniGetInstance.env->CallVoidMethod(afInstance, methodId, disable, jContext);

            jniGetInstance.env->DeleteLocalRef(afInstance);
            jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
        } else {
            CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
        }
}

    void AppsFlyerXAndroid::setSharingFilterForPartners(std::vector<std::string> partners) {
        cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

        jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
                jniGetInstance.classID, jniGetInstance.methodID);

        if (NULL != afInstance) {
            //CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");

            jclass cls = jniGetInstance.env->GetObjectClass(afInstance);


            cocos2d::JniMethodInfo jniGetContext;

            if (!cocos2d::JniHelper::getStaticMethodInfo(jniGetContext,
                                                         "org/cocos2dx/lib/Cocos2dxActivity",
                                                         "getContext",
                                                         "()Landroid/content/Context;")) {
                return;
            }


            jobjectArray result;
            result = (jobjectArray)jniGetInstance.env->NewObjectArray(partners.size(),jniGetInstance.env->FindClass("java/lang/String"),jniGetInstance.env->NewStringUTF(""));
            for(int i=0; i<partners.size(); i++) {
                jniGetInstance.env->SetObjectArrayElement(result,i,jniGetInstance.env->NewStringUTF(partners[i].c_str()));
            }

            jmethodID methodId = jniGetInstance.env->GetMethodID(cls, "setSharingFilterForPartners",
                                                                 "([Ljava/lang/String;)V");


            jniGetInstance.env->CallVoidMethod(afInstance, methodId, result);

            jniGetInstance.env->DeleteLocalRef(result);
            jniGetInstance.env->DeleteLocalRef(afInstance);
            jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
        } else {
            CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
        }
    }


void AppsFlyerXAndroid::setOnResponse(void(*callback)(std::string oneLinkURL)) {
    setCallbackOnResponse(callback);
}

void AppsFlyerXAndroid::setOnResponseError(void(*callback)(std::string message)) {
    setCallbackOnResponseError(callback);
}

void AppsFlyerXAndroid::generateInviteUrl(cocos2d::ValueMap parameter, void(*onResponse)(std::string url), void(*onResponseError)(std::string url)) {
    setOnResponse(onResponse);
    setOnResponseError(onResponseError);

    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();
    cocos2d::JniMethodInfo miGetContext;
    if (!cocos2d::JniHelper::getStaticMethodInfo(miGetContext, "org/cocos2dx/lib/Cocos2dxActivity", "getContext", "()Landroid/content/Context;")) {
        return;
    }
    jobject jContext = (jobject)miGetContext.env->CallStaticObjectMethod(miGetContext.classID, miGetContext.methodID);


    jclass clsAppsFlyer2dXConversionCallback = jniGetInstance.env->FindClass(
            "com/appsflyer/AppsFlyer2dXConversionCallback");


    jmethodID callbackMethodId = jniGetInstance.env->GetMethodID(clsAppsFlyer2dXConversionCallback,
                                                          "<init>", "()V");
    // instance of 'new AppsFlyer2dXConversionCallback()'
    jobject jCallbackProxy = jniGetInstance.env->NewObject(clsAppsFlyer2dXConversionCallback,
                                                    callbackMethodId);


    jclass classShareInviteHelper = jniGetInstance.env->FindClass("com/appsflyer/share/ShareInviteHelper");
    if (classShareInviteHelper == nullptr) {
        CCLOG("%s", "could not find classShareInviteHelper class");
    } else {
        jmethodID mid = jniGetInstance.env->GetStaticMethodID(classShareInviteHelper, "generateInviteUrl", "(Landroid/content/Context;)Lcom/appsflyer/share/LinkGenerator;");
       jobject  Generator = jniGetInstance.env->CallStaticObjectMethod(classShareInviteHelper, mid, jContext);
        jclass cls = jniGetInstance.env->GetObjectClass(Generator);
        if (cls == nullptr) {
            CCLOG("%s", "could not find class");
        } else {
            jmethodID methodId = jniGetInstance.env->GetMethodID(cls, "generateLink",
                                                  "(Landroid/content/Context;Lcom/appsflyer/CreateOneLinkHttpTask$ResponseListener;)V");
            jniGetInstance.env->CallVoidMethod(Generator,methodId, jContext,jCallbackProxy);
        }

    }
    jniGetInstance.env->DeleteLocalRef(jCallbackProxy);
    jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);

}


// Initialize AppsFlyerLib with conversion Callback
void initResponseListener() {
    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    //AppsFlyerLib afLib instance
    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {

        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);

        cocos2d::JniMethodInfo jniInit;
        // get  'new AppsFlyer2dXConversionCallback()'
        if (!cocos2d::JniHelper::getMethodInfo(jniInit,
                                               "com/appsflyer/AppsFlyer2dXConversionCallback",
                                               "<init>", "()V")) {
            return;
        }

        cocos2d::JniMethodInfo jniGetContext;

        if (!cocos2d::JniHelper::getStaticMethodInfo(jniGetContext,
                                                     "org/cocos2dx/lib/Cocos2dxActivity",
                                                     "getContext",
                                                     "()Landroid/content/Context;")) {
            return;
        }

        jobject jContext = (jobject) jniGetContext.env->CallStaticObjectMethod(
                jniGetContext.classID, jniGetContext.methodID);



        jclass clsAppsFlyer2dXConversionCallback = jniInit.env->FindClass(
                "com/appsflyer/AppsFlyer2dXConversionCallback");


        jmethodID callbackMethodId = jniInit.env->GetMethodID(clsAppsFlyer2dXConversionCallback,
                                                              "<init>", "()V");
        // instance of 'new AppsFlyer2dXConversionCallback()'
        jobject jCallbackProxy = jniInit.env->NewObject(clsAppsFlyer2dXConversionCallback,
                                                        callbackMethodId);

        jmethodID initMethodId = jniGetInstance.env->GetMethodID(cls,
                                                                 "init",
                                                                 "(Ljava/lang/String;Lcom/appsflyer/AppsFlyerConversionListener;Landroid/content/Context;)Lcom/appsflyer/AppsFlyerLib;");

        jstring jAppsFlyerDevKey = jniGetInstance.env->NewStringUTF(afDevKey.c_str());

        // Conversion Callback is initialized
        isConveriosnListenerInitialized = true;

        // This is what we actually do: afLib.init(appsFlyerDevKey, conversionDataListener, context)
        jniGetInstance.env->CallObjectMethod(afInstance, initMethodId,
                                             jAppsFlyerDevKey, jCallbackProxy, jContext);

        jniInit.env->DeleteLocalRef(jCallbackProxy);
        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
    } else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
    }
}


void AppsFlyerXAndroid::logInvite(const std::string& channel, cocos2d::ValueMap parameters) {
    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {
        //CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");

        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);


        cocos2d::JniMethodInfo jniGetContext;

        if (!cocos2d::JniHelper::getStaticMethodInfo(jniGetContext,
                                                     "org/cocos2dx/lib/Cocos2dxActivity",
                                                     "getContext",
                                                     "()Landroid/content/Context;")) {
            return;
        }
        jobject jContext = (jobject) jniGetContext.env->CallStaticObjectMethod(
                jniGetContext.classID, jniGetContext.methodID);
        jclass classShareInviteHelper = jniGetInstance.env->FindClass("com/appsflyer/share/ShareInviteHelper");
        if (classShareInviteHelper == nullptr) {
            CCLOG("%s", "could not find classShareInviteHelper class");
        } else {
            jmethodID mid = jniGetInstance.env->GetStaticMethodID(classShareInviteHelper, "logInvite",
                                                                  "(Landroid/content/Context;Ljava/lang/String;Ljava/util/Map;)V");
            jobject hashMapObj = valueMapToHashMap(jniGetInstance, parameters);



            jstring jChannel = jniGetInstance.env->NewStringUTF(channel.c_str());

            jniGetInstance.env->CallStaticVoidMethod(classShareInviteHelper, mid, jContext, jChannel, hashMapObj);

            jniGetInstance.env->DeleteLocalRef(hashMapObj);
            jniGetInstance.env->DeleteLocalRef(afInstance);
            jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
       }

    }
}

void AppsFlyerXAndroid::setDisableNetworkData(bool disable) {
    callVoidMethodWithBoolParam(disable, "setDisableNetworkData", "(Z)V");
}
#endif