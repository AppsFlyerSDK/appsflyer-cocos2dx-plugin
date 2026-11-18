# Migration guide — AppsFlyer Cocos2d-x 6.x → SDK 7 (RPC)

This guide covers:

1. **How the plugin is packaged** and how you pull in native SDK dependencies (iOS uses **Swift Package Manager**).
2. **SDK 7** API changes on **`AppsFlyerX`** (schema-generated methods in `Classes/AppsFlyer/generated/`; transport in `AppsFlyerRpc.{h,cpp}`).

| Topic | Document |
| --- | --- |
| **iOS — step-by-step + checklist** | [iOS-README.md](iOS-README.md) |
| **Init order, threading, bridges** | [SDK7-Lifecycle.md](SDK7-Lifecycle.md) |
| **Android Gradle** | [Android-README.md](Android-README.md), `proj.android-studio/app/build.gradle` |

## How the plugin is distributed (game developers)

The Cocos2d-x AppsFlyer plugin has **never been a standalone registry package** (no npm, CocoaPods spec, or Maven artifact for the bridge itself). You **copy source trees into your game** and wire your **existing** Android Studio / Xcode projects.

### SDK 6.x (what shipped on GitHub until 6.18.x)

| Piece | What you did |
| --- | --- |
| **Plugin bridge (C++)** | Copy `Classes/AppsFlyer/` into the game; add files to `CMakeLists.txt`, `Android.mk`, or the IDE project. |
| **iOS native SDK** | **Vendored inside the plugin repo**: prebuilt `libAppsFlyer` + public headers (and an optional **`Classes/Strict`** tree for kids / ad-ID-less variants). You linked the static library in Xcode and included Obj‑C++/C++ glue from the plugin. |
| **iOS project** | Sample used a **checked-in** `proj.ios_mac/…xcodeproj` or your own Xcode target; no separate package manager for AppsFlyer on iOS in the published flow. |
| **Android native SDK** | **Your** `build.gradle` / `implementation` line (Maven) or a copied `.jar` — the plugin did not ship the AAR inside `Classes/`. |
| **macOS desktop** | Supported in older lines; **removed in SDK 7**. |

### SDK 7 (this line)

| Piece | What you do now |
| --- | --- |
| **Plugin bridge** | Still **manual copy**: `Classes/AppsFlyer/` (incl. `generated/`, `AppsFlyerRpc.*`, `AppsFlyerTask.*`) + platform bridges. |
| **iOS native SDK** | **Not vendored.** Add **Swift Package Manager** dependencies in Xcode on the **app** target (see below). Minimum **iOS 15.0** (sample CMake uses **15.6**). |
| **Android native SDK** | **Maven** BOM + RPC bridge artifact in Gradle (see sample `proj.android-studio/app/build.gradle`), same pattern as other AppsFlyer mobile plugins. |
| **Codegen** | Optional maintainer flow: `./scripts/codegen/run.sh` after schema changes. Games normally consume committed `generated/` output. |

### CMake vs non-CMake (iOS)

- **CMake + Xcode generator** — what **this repository’s sample** uses: `cmake -B build-device -GXcode …` produces `AppsFlyerCocos2dX.xcodeproj`. Run `./scripts/setup-ios-spm.sh` to inject SwiftPM refs and fix CMake’s legacy build-dir settings, then build with `./scripts/build-ios-cmake.sh` or Xcode.
- **Non-CMake** — **most production Cocos games**: you already have an Xcode project from Cocos Creator, `cocos compile`, or a hand-maintained `proj.ios_mac` tree. You **do not** need to adopt CMake. Follow **[iOS-README.md](iOS-README.md)** (copy manifest + Xcode steps).

### Main vs Strict (iOS native binaries)

AppsFlyer ships the iOS SDK in two **compile-time** variants:

| Variant | Typical use | Cocos2d-x SDK 7 consumption |
| --- | --- | --- |
| **Main (regular)** | Standard apps; can use ATT / IDFA when allowed | Default SPM products **`AppsFlyerRPC`** + **`AppsFlyerLib-Static`** from the public GitHub packages (pins in [`version/ios-spm-versions.json`](../version/ios-spm-versions.json)). |
| **Strict** (user-data–private / kids binary) | Apps that must ship AppsFlyer’s **Strict** native binary and **privacy manifest** (IDFA-related APIs compiled out) | In Xcode, depend on the **Strict** Swift package products for the same RPC/Framework versions (not `AppsFlyerLib-Static` Main). Add `AFSDK_NO_IDFA=1` on the app target. Product names change with SDK releases — use the [iOS SDK install guide](https://dev.appsflyer.com/hc/docs/install-ios-sdk#strict-mode-sdk). |

**Runtime** APIs such as `setDisableAdvertisingIdentifier` only change behavior inside **whichever binary you linked**; they do **not** switch Main ↔ Strict. Pick the variant at **link time**.

Android does not use Main/Strict the same way as iOS; use Gradle BOM + bridge and follow Android SDK privacy docs.

## iOS (Swift Package Manager)

All step-by-step iOS work — **what to copy where**, integration cases (CMake vs manual Xcode), SPM pins, build settings, and troubleshooting — lives in **[iOS-README.md](iOS-README.md)**.

Summary only:

- Native SDKs: **`AppsFlyerRPC`** + **`AppsFlyerLib-Static`** via SwiftPM on the **app** target; exact versions in [`version/ios-spm-versions.json`](../version/ios-spm-versions.json).
- Repo sample: `./scripts/build-ios-cmake.sh` runs `cmake`, then `setup-ios-spm.sh`, then `xcodebuild` (pass `DEVELOPMENT_TEAM=<id>` as an extra xcodebuild setting when signing).
- After any manual `cmake -B`, run `./scripts/setup-ios-spm.sh` again before building.

## Breaking changes (summary)

| 6.x / transitional RPC batching | SDK 7 (`AppsFlyerX` task methods; internal transport in `Classes/AppsFlyer/AppsFlyerRpc.h`) |
| --- | --- |
| `void` methods, fire-and-forget worker | Schema-generated methods return **`AppsFlyerTask`** |
| Listener setters + batched `initSdk()` | **Immediate** native RPC per `register*Listener`; **`init`** is separate |
| Errors logged only (`[AppsFlyer]`) | **`AppsFlyerResult`** / **`AppsFlyerError`** via **`whenComplete`** |
| Fixed `awaitResponse: false` on start/logEvent | **`StartParams` / `LogEventParams`** pass **`awaitResponse`** through |
| `AppsFlyerXDeepLinkResult` + legacy getters | **`appsflyer::DeepLinkData`** (`DeepLinkStatus`, `error`, `deepLink` map — same contract as js-core **`DeepLinkData`**) |

Public names do **not** include `Rpc`. Internal transport lives in `AppsFlyerRpc.cpp` (`executeRpc` / `executeBlocking`).

## Recommended init sequence (SDK 7)

1. **`registerDeepLinkListener`** → on success → **`enableDebug`** → **`init(InitParams)`** with `devKey` / `appId` (iOS).
2. On init success → **`registerConversionListener`** → **`registerSessionReadyListener`**
3. In session-ready callback → **`start(awaitResponse)`** → **`logEvent`**
4. Internal **`setPluginInfo`** is sent by the plugin immediately before the first **`init`** wire — do not call it from game code.

### Before (batched `initSdk()`)

```cpp
AppsFlyerX::setDidResolveDeepLink(didResolveDeepLink);
AppsFlyerX::registerSessionReadyListener(onSessionReadyStartSdk);
AppsFlyerX::initSdk(); // batched listener RPCs + init
```

### After (SDK 7 — `whenComplete` chain)

```cpp
AppsFlyerX::registerDeepLinkListener([](appsflyer::DeepLinkData data) { /* data.status, data.deepLink */ })
    .whenComplete([](const AppsFlyerResult& r) {
        if (!r.success) return;
        AppsFlyerX::enableDebug(true).whenComplete([](const AppsFlyerResult& rDbg) {
            if (!rDbg.success) return;
            AppsFlyerX::InitParams p;
            p.devKey = "...";
            p.appId = "..."; // iOS
            AppsFlyerX::init(p).whenComplete([](const AppsFlyerResult& r2) {
                if (!r2.success) return;
                AppsFlyerX::registerSessionReadyListener(onSessionReadyStartSdk);
            });
        });
    });
```

See `Classes/AppDelegate.cpp` for the full sample chain.

## Legacy `AppsFlyerX::initSdk()`

Still present for transitional callers but **no longer registers listeners**. Prefer `AppsFlyerX::init` and explicit listener tasks.

## Removed SDK 6 helper classes

These types were removed from the plugin tree; use generated parameter structs on `AppsFlyerX` instead.

| Removed (6.x) | SDK 7 replacement |
| --- | --- |
| `AFSDKXPurchaseDetails`, platform-specific `validateAndLogInAppPurchase` overloads | `ValidateAndLogInAppPurchaseParams` + `validateAndLogInAppPurchase(…).whenComplete(…)` |
| `AFXAdRevenueData`, `AppsFlyerXAdRevenueMediationNetworkType` | `LogAdRevenueParams` + `AppsFlyerX::MediationNetwork::*` schema keys (wire mapping is codegen’d from schema `valueMap`) |
| `AppsFlyerXConsent`, `forGDPRUser` / `forNonGDPRUser` | `SetConsentDataParams` + `setConsentData(…)` |
| `setUserEmails(vector, EmailCryptTypeX)` | `setUserEmail(const std::string& email)` (single email per call) |

### Purchase validation (SDK 7)

Use **`AppsFlyerX::PurchaseType`** (public schema keys `oneTimePurchase` / `subscription`) and platform-shaped **`InAppPurchaseDetails`** — codegen maps to Android flat RPC or iOS `product`/`transaction` wire (same transforms as Flutter/RN `AFPurchaseType`).

```cpp
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
AppsFlyerX::ValidateAndLogInAppPurchaseParams params;
params.purchase.purchaseType = AppsFlyerX::PurchaseType::OneTimePurchase;
params.purchase.productId = "product_id";
params.purchase.purchaseToken = "purchase_token";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
AppsFlyerX::ValidateAndLogInAppPurchaseParams params;
params.purchase.purchaseType = AppsFlyerX::PurchaseType::Subscription;
params.purchase.productId = "product_id";
params.purchase.transactionId = "transaction_id";
#endif
params.additionalParameters["key"] = "value";
AppsFlyerX::validateAndLogInAppPurchase(params).whenComplete([](const AppsFlyerResult& r) {
    if (!r.success) { /* r.error */ return; }
    /* r.dataJson */
});
```

### Android log level (SDK 7)

`setLogLevel` accepts **`AppsFlyerX::LogLevel`** (lowercase public keys). On Android, codegen applies schema transform **`androidLogLevel`** (`debug` → `DEBUG`, etc.) before RPC.

### Ad revenue (SDK 7)

Use the schema-generated **`enum class MediationNetwork`** in `AppsFlyerRpcSchema.generated.h` (`AppsFlyerX::MediationNetwork`). Codegen maps enum → schema key → native wire string per platform (same `valueMap` as Cordova/js-core).

```cpp
AppsFlyerX::LogAdRevenueParams params;
params.monetizationNetwork = "myNetwork";
params.mediationNetwork = AppsFlyerX::MediationNetwork::ApplovinMax;
params.currencyIso4217Code = "USD";
params.revenue = 7.99;
AppsFlyerX::logAdRevenue(params);
```

### Consent (SDK 7)

```cpp
AppsFlyerX::SetConsentDataParams consent;
consent.isUserSubjectToGDPR = true;
consent.hasConsentForDataUsage = true;
consent.hasConsentForAdsPersonalization = true;
AppsFlyerX::setConsentData(consent);
```

## Testing locally

```bash
./scripts/run-af-task-tests.sh
```

Android smoke build:

```bash
cd proj.android-studio && ./gradlew :app:testDebugUnitTest :app:assembleDebug
```