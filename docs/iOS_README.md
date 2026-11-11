<img src="https://www.appsflyer.com/wp-content/uploads/2016/11/logo-1.svg"  width="200">

# Cocos2dX iOS Integration Guide

For startup order, threading, and when RPC runs relative to Cocos2d-x, see [SDK7_LIFECYCLE.md](SDK7_LIFECYCLE.md).

## Sample project

iOS sample sources live under `proj.ios_mac/ios/`. Generate the app with CMake (`scripts/build-ios-cmake.sh`), then run `scripts/setup-ios-rpc-pods.sh` and open the `.xcworkspace` in Xcode.

## SDK 7 (RPC)

Add the cross-platform C++ sources under `Classes/AppsFlyer/` (including `AppsFlyerXCore`, `rpc/`) and the iOS RPC bridge under `proj.ios_mac/ios/rpc/` (`AppsFlyerCocosRpcBridge.swift`, `.mm`, `.h`, plus `AppsFlyerCocos2dX-Bridging-Header.h`). Link **AppsFlyerRPC** and **AppsFlyerFramework** via CocoaPods (see repo `Podfile` and setup script). Keep `proj.ios_mac/ios/SwiftBridge.swift` in the app target so the Swift runtime links.

## Privacy / IDFA

Regular builds link **Main** subspecs via the repo `Podfile` (copied into your CMake build dir by `scripts/setup-ios-rpc-pods.sh`).

### Strict mode (kids / ad-ID-less apps)

Opt in at **`pod install`** time.

1. Edit the `Podfile` in your CMake build directory (or change the repo-root `Podfile` before re-running the setup script).
2. Uncomment `$AppsFlyerStrictMode = true` at the top of the target section.
3. Run `./scripts/setup-ios-rpc-pods.sh <your-build-dir>` (or `pod install` in that directory).

That resolves `AppsFlyerRPC/Strict` and `AppsFlyerFramework/Strict` and sets `AFSDK_NO_IDFA=1` on the app target so C++ sources match the Strict binary. Do not add extra `AppsFlyerFramework/Strict` pods by hand — use only the conditional block in the template Podfile.

Runtime `setDisableAdvertisingIdentifier` does not replace Strict mode; it configures behavior inside whichever native binary is linked. See [AppsFlyer Strict mode docs](https://dev.appsflyer.com/hc/docs/install-ios-sdk#strict-mode-sdk).
