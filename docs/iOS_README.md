<img src="https://www.appsflyer.com/wp-content/uploads/2016/11/logo-1.svg"  width="200">

# Cocos2dX iOS Integration Guide

For startup order, threading, and when RPC runs relative to Cocos2d-x, see [SDK7_LIFECYCLE.md](SDK7_LIFECYCLE.md).

## Sample project

iOS sample sources live under `proj.ios_mac/ios/`. Generate the app with CMake (`scripts/build-ios-cmake.sh`), then run `scripts/setup-ios-rpc-pods.sh` and open the `.xcworkspace` in Xcode.

## SDK 7 (RPC)

Add the cross-platform C++ sources under `Classes/AppsFlyer/` (including `AppsFlyerXCore`, `rpc/`) and the iOS RPC bridge under `proj.ios_mac/ios/rpc/` (`AppsFlyerCocosRpcBridge.swift`, `.mm`, `.h`, plus `AppsFlyerCocos2dX-Bridging-Header.h`). Link **AppsFlyerRPC** and **AppsFlyerFramework** via CocoaPods (see repo `Podfile` and setup script). Keep `proj.ios_mac/ios/SwiftBridge.swift` in the app target so the Swift runtime links.

## Privacy / IDFA

Configure attribution privacy through the native SDK 7 stack linked by CocoaPods (`AppsFlyerFramework` / `AppsFlyerRPC`). Cocos-specific Strict-mode sources are not bundled with the SDK 7 RPC plugin.
