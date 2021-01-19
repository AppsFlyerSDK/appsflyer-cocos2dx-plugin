#include <memory>

#include <android/log.h>
#include <jni.h>

#include "AppDelegate.h"

#define  LOG_TAG    "main"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

namespace {
std::unique_ptr<AppDelegate> appDelegate;
}

#if COCOS2D_VERSION <= 0x00030600
void cocos_android_app_init(JNIEnv* env, jobject thiz)
#else
void cocos_android_app_init(JNIEnv* env)
#endif
 {
    LOGD("cocos_android_app_init");
    appDelegate.reset(new AppDelegate());
}
