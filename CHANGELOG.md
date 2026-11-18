 # Versions

## Unreleased (SDK 7)
- **BREAKING:** macOS desktop is no longer supported. The plugin targets Android and iOS only with SDK 7 and AppsFlyer RPC.
- Remove vendored SDK 6 `libAppsFlyer` static library and headers, legacy `Classes/Strict` tree, unused `AppsFlyerProxyX`, Swift compatibility stubs for the static lib, and the manual `proj.ios_mac/AppsFlyerCocos2dX.xcodeproj` (use CMake-generated Xcode + SwiftPM).
- **iOS:** Swift Package Manager for **AppsFlyerRPC** + **AppsFlyerLib-Static** (iOS **15.0+**; pins in `version/ios-spm-versions.json`). Sample/CI: `./scripts/setup-ios-spm.sh` after `cmake`, build with `-derivedDataPath` (see `docs/Migration-Guide.md`).
- Drop CI workflow that validated vendored iOS header imports.
- **BREAKING:** `AppsFlyerX` public API is schema-generated: methods return **`AppsFlyerTask`** with optional **`whenComplete`**. Removed **`initSdk()`** batching, legacy void listener setters, and hand-maintained `AppsFlyerXCore`. Use **`InitParams`**, **`register*Listener`**, and explicit init order (see `docs/Migration-Guide.md`).
- Schema-driven codegen: `tools/af-schema-compiler`, **79** Cocos RPC methods, pinned `schema/appsflyer-plugins-rpc-schema.json`.
- Inbound `rpcEvent` payloads parsed into `ValueMap` / `appsflyer::DeepLinkData` where `data` is present.
- **BREAKING:** Replace `AppsFlyerXDeepLinkResult` with header-only `appsflyer::DeepLinkData` (js-core / Flutter–aligned status normalization; use `deepLink` map fields such as `deep_link_value`, `media_source`).
- Refresh `AppsFlyerXMacro.h` against iOS `AppsFlyerLib.h` and Android `AFInAppEventType` / `AFInAppEventParameterName` (7.x); fix duplicate/typo macros; omit ad-revenue param keys (use `logAdRevenue`).
- Remove unused SDK 6 helper types (`AFSDKXPurchaseDetails`, `AFSDKXValidateAndLogResult`, `AFXAdRevenueData`, `AppsFlyerXConsent`, `EmailCryptTypeX`). Use schema `*Params` structs from `Classes/AppsFlyer/generated/` (see `docs/Migration-Guide.md`).
- Schema-first `logAdRevenue.mediationNetwork`: `enum class MediationNetwork` and platform `valueMap` resolution in `AppsFlyerRpcSchema.generated.*` (aligned with Cordova/js-core).
- Schema transforms: `setLogLevel` → `androidLogLevel`; `validateAndLogInAppPurchase` → `PurchaseType`, `InAppPurchaseDetails`, and Android/iOS RPC wire encoding (`androidPurchaseType` / `iosPurchaseType`).
- Consolidate RPC bridge into `Classes/AppsFlyer/AppsFlyerRpc.{h,cpp}` (replaces former `rpc/` transport split and related headers).
- Canonical plugin semver for internal `setPluginInfo`: `version/plugin-version.json` → `AppsFlyerPluginMetadata.generated.h` (Codegen `af-schema-compiler`).

## 6.18.0
    - update Android SDK to 6.18.0
    - update iOS SDK to 6.18.0

## 6.10.3
    - update Android SDK to 6.10.3
    - update iOS SDK to 6.10.1

## 6.9.4
    - update Android SDK to 6.9.4
    - update iOS SDK to 6.9.1

## 6.8.2
    - update Android SDK to 6.8.2

## 6.8.0
    - update iOS SDK to 6.8.1
    - update Android SDK to 6.8.0
    - new android API setDisableNetworkData 

## 6.5.2
    - update iOS SDK to 6.5.2
    - update Android SDK to 6.5.2

## 6.4.4
    - update iOS SDK to 6.4.4
    - update Android SDK to 6.4.3
    - new API generateUserInvite
    - new API logInvite

## 6.4.0
    - update iOS SDK to 6.4.0
    - update Android SDK to 6.4.0
    - new API setSharingFilterForPartners
    - Deprecated API setSharingFilterForAllPartners and setSharingFilter

## 6.3.5
    - update iOS SDK to 6.3.5
    - new API for iOS setCurrentDeviceLanguage

## 6.3.2
    - iOS SDK 6.3.2
    - Android SDK 6.3.2
    -disableAdvertiserIdentifier support for Android added

## 6.3.0
    - iOS SDK 6.3.0
    - Android SDK 6.3.0
    -new API setOneLinkCustomDomain

## 6.2.6
    - iOS SDK 6.2.6
    - Android SDK 6.2.3

## 6.2.41
    - fix

## 6.2.4
    - iOS SDK 6.2.4
    - fixes

## 6.2.3
    - iOS SDK 6.2.3 

## 6.2.1
    -fix 

## 6.2.0
    - iOS SDK 6.2.0
    - Android SDK 6.2.0
    - new API setPartnerData

## 6.1.4
    - iOS SDK 6.1.3
    - Android SDK 6.1.4
    - new API for unified deep linking
   
----------

## 6.0.3
    - iOS SDK 6.0.3
    - APIs renamed:
    |Before v6   | v6  |
|---|---|
| trackAppLaunch | start|
| trackEvent  | logEvent  |
| trackLocation | logLocation|
| stopTracking  | stop  |
| validateAndTrackInAppPurchase  | validateAndLogInAppPurchase  |
| setDeviceTrackingDisabled | anonymizeUser|

----------

## 5.4.1

Release date: Aug 5, 2020
Release type: Major / **Minor** / Hotfix

Android SDK is updated to v5.4.1
iOS SDK is updated to v5.4.1

 **Technical Story**

Cocos2dx >> Support for "sharingFilter"
Cocos2dx >> Support for "sharingFilterForAllPartners"

----------

## 1.1.0

Release date: Jul 15, 2019
Release type: Major / **Minor** / Hotfix

Android SDK is updated to v4.10.1
iOS SDK is updated to v4.10.2

 **Technical Story**

Cocos2dx >> Support for "stopTracking"

----------


## 1.0.2

Release date: Apr 4, 2019
Release type: Major / Minor / **Hotfix**

 **Technical Story**
- Cocos: gradle + install referrer
- Cocos2dx plugin >> Docs update

**Bugs**
 - Cocos2dx >> SDK is not able to send events to AF without using `setOnConversionDataReceived`


----------
