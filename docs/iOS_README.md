<img src="https://www.appsflyer.com/wp-content/uploads/2016/11/logo-1.svg"  width="200">

# Cocos2dX iOS Integration Guide

## Sample project

iOS sample sources live under `proj.ios_mac/ios/`. Generate the app with CMake (`scripts/build-ios-cmake.sh`), then run `scripts/setup-ios-rpc-pods.sh` and open the `.xcworkspace` in Xcode.

## SDK 7 (RPC)

Add the cross-platform C++ sources under `Classes/AppsFlyer/` (including `AppsFlyerXCore`, `rpc/`) and the iOS RPC bridge under `proj.ios_mac/ios/rpc/`. Link **AppsFlyerRPC** and **AppsFlyerFramework** via CocoaPods (see repo `Podfile` and setup script).

## Strict mode (no IDFA)

Use the AppsFlyer Strict subspecs in your Podfile and define `AFSDK_NO_IDFA=1` in Xcode build settings if required by your app.

![](https://raw.githubusercontent.com/AppsFlyerSDK/appsflyer-cocos2dx-plugin/master/Resources/preprocessor_macro.png)
