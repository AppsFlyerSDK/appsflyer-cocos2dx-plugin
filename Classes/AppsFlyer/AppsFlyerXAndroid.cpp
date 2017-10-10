//
// Created by Maxim Shoustin on 10/9/17.
//

#include <jni.h>
#include "AppsFlyerXAndroid.h"
#include "../../cocos2d/cocos/platform/CCPlatformMacros.h"
#include "AppsFlyerProxyX.h"

 std::string afDevKey;


/* Null, because instance will be initialized on demand. */
//AppsFlyerXAndroid* AppsFlyerXAndroid::instance = 0;

//AppsFlyerXAndroid* AppsFlyerXAndroid::getInstance() {
//    if (instance == 0) {
//        instance = new AppsFlyerXAndroid();
//    }
//
//    return instance;
//}
//
AppsFlyerXAndroid::AppsFlyerXAndroid() {}


/*
 * AppsFlyerLib.getInstance()
 *  */
cocos2d::JniMethodInfo getAppsFlyerInstance(){

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

    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    jobject afInstance = (jobject)jniGetInstance.env->CallStaticObjectMethod(jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {
        CCLOG("%s","com/appsflyer/AppsFlyerLib is loaded");

        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);

        jmethodID methodId = jniGetInstance.env->GetMethodID(cls, "setDebugLog","(Z)V");
        jniGetInstance.env->CallVoidMethod(afInstance, methodId, isDebug);

        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
    }
    else{
        CCLOGERROR("%s","'AppsFlyerLib' is not loaded");
    }
}



 void AppsFlyerXAndroid::setOnConversionDataReceived(void(*callback)(cocos2d::ValueMap installData)) {

    if(afDevKey.empty()){
        CCLOGWARN("%s","AppsFlyer Dev Key is not provided");
        return;
    }

    setAttributionCallbackMethod(callback);

    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    //AppsFlyerLib afLib instance
    jobject afInstance = (jobject)jniGetInstance.env->CallStaticObjectMethod(jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {

        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);

        cocos2d::JniMethodInfo jniInit;
        // get  'new AppsFlyer2dXConversionCallback()'
        if (!cocos2d::JniHelper::getMethodInfo(jniInit, "com/appsflyer/AppsFlyer2dXConversionCallback", "<init>", "()V")) {
            return;
        }

        jclass clsAppsFlyer2dXConversionCallback = jniInit.env->FindClass("com/appsflyer/AppsFlyer2dXConversionCallback");


        jmethodID callbackMethodId = jniInit.env->GetMethodID(clsAppsFlyer2dXConversionCallback, "<init>", "()V");

        jobject jCallbackProxy = jniInit.env->NewObject(clsAppsFlyer2dXConversionCallback, callbackMethodId);

        jmethodID initMethodId = jniGetInstance.env->GetMethodID(cls,
                                                                 "init",
                                                                 "(Ljava/lang/String;Lcom/appsflyer/AppsFlyerConversionListener;)Lcom/appsflyer/AppsFlyerLib;");

        jstring jAppsFlyerDevKey = jniGetInstance.env->NewStringUTF(afDevKey.c_str());

        // This is what we actually do: afLib.init(appsFlyerDevKey, conversionDataListener)
        jniGetInstance.env->CallObjectMethod(afInstance, initMethodId, jAppsFlyerDevKey, jCallbackProxy);


        jniInit.env->DeleteLocalRef(jCallbackProxy);
        jniGetInstance.env->DeleteLocalRef(afInstance);
        jniGetInstance.env->DeleteLocalRef(jniGetInstance.classID);
    }
    else{
        CCLOGERROR("%s","'AppsFlyerLib' is not loaded");
    }
}


void AppsFlyerXAndroid::startTracking() {

    if(afDevKey.empty()){
        CCLOGWARN("%s","AppsFlyer Dev Key is not provided");
        return;
    }

    cocos2d::JniMethodInfo jniGetInstance = getAppsFlyerInstance();

    //AppsFlyerLib afLib instance
    jobject afInstance = (jobject)jniGetInstance.env->CallStaticObjectMethod(jniGetInstance.classID, jniGetInstance.methodID);

    if (NULL != afInstance) {
        CCLOG("%s","com/appsflyer/AppsFlyerLib is loaded");

        jclass cls = jniGetInstance.env->GetObjectClass(afInstance);

        cocos2d::JniMethodInfo miGetContext;

        if (!cocos2d::JniHelper::getStaticMethodInfo(miGetContext,
                                                     "org/cocos2dx/lib/Cocos2dxActivity",
                                                     "getContext",
                                                     "()Landroid/content/Context;")) {
            return;
        }


        jobject jContext = (jobject)miGetContext.env->CallStaticObjectMethod(miGetContext.classID, miGetContext.methodID);

        //public void trackAppLaunch(Context ctx, String devKey)
        jmethodID startTrackingMethodId = jniGetInstance.env->GetMethodID(cls,
                                                                 "trackAppLaunch",
                                                                 "(Landroid/content/Context;Ljava/lang/String;)V");

        jstring jAppsFlyerDevKey = jniGetInstance.env->NewStringUTF(afDevKey.c_str());

        // This is what we actually do: afLib.startTracking((Application) context.getApplicationContext())
        jniGetInstance.env->CallVoidMethod(afInstance, startTrackingMethodId, jContext, jAppsFlyerDevKey);

        //jniGetInstance.env->DeleteLocalRef(jniGetInstance);
    }
    else{
        CCLOGERROR("%s","'AppsFlyerLib' is not loaded");
    }
}

void AppsFlyerXAndroid::trackEvent(const std::string &eventName, const std::string &value) {

}

void AppsFlyerXAndroid::setUserEmails(std::vector<std::string> userEmails, EmailCryptTypeX type) {

}

void AppsFlyerXAndroid::setAppsFlyerDevKey(const std::string &appsFlyerDevKey) {
    afDevKey = appsFlyerDevKey;
}















