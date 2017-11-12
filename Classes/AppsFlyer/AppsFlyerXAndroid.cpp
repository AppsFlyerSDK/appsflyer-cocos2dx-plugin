//
// Created by Maxim Shoustin on 10/9/17.
//

#include <jni.h>
#include "AppsFlyerXAndroid.h"
#include "AppsFlyerProxyX.h"
#include "../../cocos2d/external/flatbuffers/reflection_generated.h"

std::string afDevKey;
bool isConveriosnListenerInitialized = false;

// Headers
void callVoidMethodWithStringParam(const std::string &param, const char *method_name,
                                   const char *descriptor);
void initConvertionCallback();

std::string callStringMethod(const char *method_name, const char *descriptor);

void callVoidMethodWithBoolParam(bool param, const char *method_name, const char *descriptor);

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


void AppsFlyerXAndroid::setIsDebug(bool isDebug) {
    callVoidMethodWithBoolParam(isDebug, "setDebugLog", "(Z)V");
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

void AppsFlyerXAndroid::setAppInviteOneLink(std::string &appInviteOneLinkID) {
    callVoidMethodWithStringParam(appInviteOneLinkID, "setAppInviteOneLink",
                                  "(Ljava/lang/String;)V");
}

void AppsFlyerXAndroid::deviceTrackingDisabled(bool deviceTrackingDisabled) {
    callVoidMethodWithBoolParam(deviceTrackingDisabled, "setDeviceTrackingDisabled", "(Z)V");
}

void AppsFlyerXAndroid::setAdditionalData(cocos2d::ValueMap customData) {
    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();
    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {
        CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");

        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);

        // Get Java HashMap object
        jclass hashMapClass = jniGetInstance.env->FindClass("java/util/HashMap");
        jmethodID hashMapInit = jniGetInstance.env->GetMethodID(hashMapClass, "<init>", "(I)V");
        jobject hashMapObj = jniGetInstance.env->NewObject(hashMapClass, hashMapInit,
                                                           customData.size());

        jmethodID hashMapId = jniGetInstance.env->GetMethodID(hashMapClass, "put",
                                                              "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

        for (auto it : customData) {   //https://github.com/cocos2d/cocos2d-x/blob/v3/cocos/base/CCValue.h
            switch (it.second.getType()) {

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

                case cocos2d::Value::Type::STRING: {
                    jniGetInstance.env->CallObjectMethod(hashMapObj, hashMapId,
                                                         jniGetInstance.env->NewStringUTF(
                                                                 it.first.c_str()),
                                                         jniGetInstance.env->NewStringUTF(
                                                                 it.second.asString().c_str()));
                    break;
                }
                default:
                    break;
            }
        }


        jmethodID methodId = jniGetInstance.env->GetMethodID(cls, "setAdditionalData",
                                                             "(Ljava/util/HashMap;)V");

        jniGetInstance.env->CallVoidMethod(afInstance, methodId, hashMapObj);

        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
    } else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
    }
}

void AppsFlyerXAndroid::setHost(const std::string &host) {
    callVoidMethodWithStringParam(host, "setHostName", "(Ljava/lang/String;)V");
}

void AppsFlyerXAndroid::setOnConversionDataReceived(void(*callback)(cocos2d::ValueMap installData)) {

    if (afDevKey.empty()) {
        CCLOGWARN("%s", "AppsFlyer Dev Key is not provided");
        return;
    }

    setAttributionCallbackOnConversionDataReceived(callback);

    if (!isConveriosnListenerInitialized){
        initConvertionCallback();
    }
}

void AppsFlyerXAndroid::setOnAppOpenAttribution(void(*callback)(cocos2d::ValueMap attributionData)){
    if (afDevKey.empty()) {
        CCLOGWARN("%s", "AppsFlyer Dev Key is not provided");
        return;
    }

    setAttributionCallbackOnAppOpenAttribution(callback);

    if (!isConveriosnListenerInitialized){
        initConvertionCallback();
    }

}

void AppsFlyerXAndroid::startTracking() {

    if (afDevKey.empty()) {
        CCLOGWARN("%s", "AppsFlyer Dev Key is not provided");
        return;
    }

    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    //AppsFlyerLib afLib instance
    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {
        CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");

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

        //public void trackAppLaunch(Context ctx, String devKey)
        jmethodID startTrackingMethodId = jniGetInstance.env->GetMethodID(cls,
                                                                          "trackAppLaunch",
                                                                          "(Landroid/content/Context;Ljava/lang/String;)V");

        jstring jAppsFlyerDevKey = jniGetInstance.env->NewStringUTF(afDevKey.c_str());

        // This is what we actually do: afLib.startTracking((Application) context.getApplicationContext())
        jniGetInstance.env->CallVoidMethod(afInstance, startTrackingMethodId, jContext,
                                           jAppsFlyerDevKey);

        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
    } else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
    }
}

void AppsFlyerXAndroid::trackEvent(const std::string &eventName, const std::string &value) {

}

void AppsFlyerXAndroid::trackEvent(const std::string &eventName, cocos2d::ValueMap values) {

    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {
        CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");

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

                default:
                    break;
            }
        }

        jmethodID methodId = jniGetInstance.env->GetMethodID(cls, "trackEvent",
                                                             "(Landroid/content/Context;Ljava/lang/String;Ljava/util/Map;)V");

        jstring jEventName = jniGetInstance.env->NewStringUTF(eventName.c_str());

        //public void trackEvent(Context context, String eventName, Map<String,Object> eventValues)
        jniGetInstance.env->CallVoidMethod(afInstance, methodId, jContext, jEventName, hashMapObj);

        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
    } else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
    }
}

void AppsFlyerXAndroid::setUserEmails(std::vector<std::string> userEmails, EmailCryptTypeX type) {

}

void AppsFlyerXAndroid::setAppsFlyerDevKey(const std::string &appsFlyerDevKey) {
    afDevKey = appsFlyerDevKey;
}

std::string AppsFlyerXAndroid::appsFlyerDevKey() {
    return afDevKey;
}

std::string AppsFlyerXAndroid::getSDKVersion(){
    return callStringMethod("getSDKVersion", "()Ljava/lang/String;");
}

std::string AppsFlyerXAndroid::getHost(){
    return callStringMethod("getHost", "()Ljava/lang/String;");
}

//================== Utils ==================//

void callVoidMethodWithStringParam(const std::string &param, const char *method_name,
                                   const char *descriptor) {
    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {
        CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");

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

std::string callStringMethod(const char *method_name, const char *descriptor) {
    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    jobject afInstance = (jobject) jniGetInstance.env->CallStaticObjectMethod(
            jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {
        CCLOG("%s", "com/appsflyer/AppsFlyerLib is loaded");
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

// Initialize AppsFlyerLib with conversion Callback
void initConvertionCallback(){
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

        jclass clsAppsFlyer2dXConversionCallback = jniInit.env->FindClass(
                "com/appsflyer/AppsFlyer2dXConversionCallback");


        jmethodID callbackMethodId = jniInit.env->GetMethodID(clsAppsFlyer2dXConversionCallback,
                                                              "<init>", "()V");
        // instance of 'new AppsFlyer2dXConversionCallback()'
        jobject jCallbackProxy = jniInit.env->NewObject(clsAppsFlyer2dXConversionCallback,
                                                        callbackMethodId);

        jmethodID initMethodId = jniGetInstance.env->GetMethodID(cls,
                                                                 "init",
                                                                 "(Ljava/lang/String;Lcom/appsflyer/AppsFlyerConversionListener;)Lcom/appsflyer/AppsFlyerLib;");

        jstring jAppsFlyerDevKey = jniGetInstance.env->NewStringUTF(afDevKey.c_str());

        // This is what we actually do: afLib.init(appsFlyerDevKey, conversionDataListener)
        jniGetInstance.env->CallObjectMethod(afInstance, initMethodId, jAppsFlyerDevKey,
                                             jCallbackProxy);

        // Conversion Callback is initialized
        isConveriosnListenerInitialized = true;


        jniInit.env->DeleteLocalRef(jCallbackProxy);
        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
    } else {
        CCLOGERROR("%s", "'AppsFlyerLib' is not loaded");
    }
}














