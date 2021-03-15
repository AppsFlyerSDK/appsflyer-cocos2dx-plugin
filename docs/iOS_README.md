<img src="https://www.appsflyer.com/wp-content/uploads/2016/11/logo-1.svg"  width="200">

# Cocos2dX iOS Integration Guide

## Sample Project

There's a sample project in the `proj.ios_mac` directory.



## Installation

How to integrate the AppsFlyer SDK into your Cocos2dx Apple project.

1\. Download the [source files](https://github.com/AppsFlyerSDK/AppsFlyerCocos2dX/tree/master/Classes/AppsFlyer) and copy them to your project

 - `AppsFlyerX.cpp`
 - `AppsFlyerX.h`
 - `AppsFlyerX+AppController.h`
 - `AppsFlyerX+AppController.m`
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
 - `libAppsFlyer/AppsFlyerLib.h`
 - `libAppsFlyer/libAppsFlyerLib.a`

to `Classes/AppsFlyer`


2\. It looks like in the image below.

![add-iOS-files](https://github.com/AppsFlyerSDK/AppsFlyerCocos2dX/blob/development/Resources/preprocessor_macro.png?raw=true)

If you want to use [AppsFlyer Strict mode SDK](https://support.appsflyer.com/hc/en-us/articles/207032066-iOS-SDK-V6-X-integration-guide-for-developers#integration-strict-mode-sdk): <br>
1\. Download the source files from [here](https://github.com/AppsFlyerSDK/AppsFlyerCocos2dX/tree/master/Classes/Strict/AppsFlyer) and copy them to your project
2\. Inside Xcode, go to your target, then **Build Settings** and define the Preprocessor Macro `AFSDK_NO_IDFA=1` <br>
![preprocessor_macro](https://github.com/AppsFlyerSDK/AppsFlyerCocos2dX/blob/development/Resources/preprocessor_macro.png?raw=true)
