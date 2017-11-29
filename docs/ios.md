# iOS AppsFlyer Wrapper for AppsFlyer2dX

## Sample Project

There's a sample project in the `proj.ios_mac` directory.

## Project Status

This project is actively under development.

## Installation

How to integrate the AppsFlyer SDK into your Cocos2dx Apple project.

1\. Download the source files and copy them to your project.

 - `AppsFlyerProxyX.cpp`
 - `AppsFlyerProxyX.h`
 - `AppsFlyerX.cpp`
 - `AppsFlyerX.h`
 - `AppsFlyerX+AppController.h`
 - `AppsFlyerX+AppController.m`
 - `AppsFlyerXAndroid.cpp`
 - `AppsFlyerXAndroid.h`
 - `AppsFlyerXApple.h`
 - `AppsFlyerXApple.mm`
 - `AppsFlyerXAppleDelegate.h`
 - `AppsFlyerXAppleDelegate.mm`
 - `AppsFlyerXAppleHelper.h`
 - `AppsFlyerXAppleHelper.mm`
 - `AppsFlyerXMacro.h`
 - `EmailCryptTypeX.h`
 - `libAppsFlyer/AppsFlyerCrossPromotionHelper.h`
 - `libAppsFlyer/AppsFlyerLinkGenerator.h`
 - `libAppsFlyer/AppsFlyerShareInviteHelper.h`
 - `libAppsFlyer/AppsFlyerTracker.h`
 - `libAppsFlyer/libAppsFlyerLib.a`

to `Classes/AppsFlyer`

2\. It looks like in the image below.

![add-iOS-files](https://github.com/AppsFlyerSDK/AppsFlyerCocos2dX/blob/development/Resources/add-to-iOS-mk.png) <!-- .element height="50%" width="50%" -->

3\. Add the following lines to your code to be able to initialize tracking:

```
#include "AppsFlyer/AppsFlyerX.h"

bool AppDelegate::applicationDidFinishLaunching() {

  AppsFlyerX::setAppsFlyerDevKey("${MY_DEV_KEY}");
  AppsFlyerX::setAppleAppID("${MY_APP_ID}");

  ...
}
```
4\. Done!

## API Methods

### Debug mode

 Prints our messages to the log. This property should only be used in DEBUG mode. The default value is `false`
```
static void setIsDebug(bool isDebug);
```
Example:
```
bool AppDelegate::applicationDidFinishLaunching() {
    AppsFlyerX::setIsDebug(true);
    ...
}
```
### Get conversion data
To get conversion data - you should define two callbacks, which described below
```
static void setOnConversionDataReceived(void(*callback)(cocos2d::ValueMap installData));
static void setOnConversionDataRequestFailure(void(*callback)(cocos2d::ValueMap error));
```
Example:
```
static void onConversionDataReceived(cocos2d::ValueMap installData) {
    for (auto &t : installData){
        CCLOG("%s - %s", t.first.c_str(), t.second.asString().c_str());
    }
}

static void onConversionDataRequestFailure(cocos2d::ValueMap map) {
    for (auto &t : map){
        CCLOG("%s - %s", t.first.c_str(), t.second.asString().c_str());
    }
}    

bool AppDelegate::applicationDidFinishLaunching() {
    ...
    AppsFlyerX::setOnConversionDataReceived(onConversionDataReceived);
    AppsFlyerX::setOnConversionDataRequestFailure(onConversionDataRequestFailure);  
    ...
}
```
### Get attribution data
To get attribution data - you should define two callbacks, which described below
```
static void setOnAppOpenAttribution(void(*callback)(cocos2d::ValueMap attributionData));
static void setOnAppOpenAttributionFailure(void(*callback)(cocos2d::ValueMap error));
```
Example:
```
static void onAppOpenAttribution(cocos2d::ValueMap map) {
    for (auto &t : map) {
        CCLOG("%s - %s", t.first.c_str(), t.second.asString().c_str());
    }
}

static void onAppOpenAttributionFailure(cocos2d::ValueMap map) {
    for (auto &t : map) {
        CCLOG("%s - %s", t.first.c_str(), t.second.asString().c_str());
    }
}

bool AppDelegate::applicationDidFinishLaunching() {
	...
	AppsFlyerX::setOnAppOpenAttribution(onAppOpenAttribution);
	AppsFlyerX::setOnAppOpenAttributionFailure(onAppOpenAttributionFailure);
	...
}
```
### Track events
Use this method to track events in your app like purchases or user actions
```
static void trackEvent(const std::string& eventName, cocos2d::ValueMap values);
```
Example:
```
bool AppDelegate::applicationDidFinishLaunching() {
	...
  AppsFlyerX::trackEvent(AFEventPurchase, {{ "key1", cocos2d::Value("value1")},
                                           { "key2", cocos2d::Value("value2")}});
	...
}
```
### User Emails
Use this to send the User's emails
```
static void setUserEmails(std::vector<std::string> userEmails, EmailCryptTypeX type);
```
Example:
```
bool AppDelegate::applicationDidFinishLaunching() {
	...
  AppsFlyerX::setUserEmails({"kinzer.appsf@gmail.com"}, XEmailCryptTypeSHA256);
	...
}
```
