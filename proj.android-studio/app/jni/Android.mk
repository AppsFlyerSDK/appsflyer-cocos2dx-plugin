LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := MyGame

LOCAL_MODULE_FILENAME := libMyGame

LOCAL_SRC_FILES := $(LOCAL_PATH)/hellocpp/main.cpp \
                   $(LOCAL_PATH)/../../../Classes/AppDelegate.cpp \
                   $(LOCAL_PATH)/../../../Classes/HelloWorldScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/AppsFlyer/AppsFlyerXAndroid.cpp \
                   $(LOCAL_PATH)/../../../Classes/AppsFlyer/AppsFlyerProxyX.cpp \
                   $(LOCAL_PATH)/../../../Classes/AppsFlyer/AppsFlyerX.cpp \
                   $(LOCAL_PATH)/../../../Classes/AppsFlyer/AppsFlyerXDeepLinkResult.cpp \
                   $(LOCAL_PATH)/../../../Classes/AppsFlyer/AFSDKXValidateAndLogResult.cpp \
                   $(LOCAL_PATH)/../../../Classes/AppsFlyer/AFSDKXPurchaseDetails.cpp \
                   $(LOCAL_PATH)/../../../Classes/AppsFlyer/AFXAdRevenueData.cpp


LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../Classes

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cc_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module, cocos)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END