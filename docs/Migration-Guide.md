# Migration guide — AppsFlyer Cocos2d-x 6.x → SDK 7 (RPC)

For games already on the **Cocos2d-x plugin 6.x** line (AppsFlyer native SDK **6.x**), this guide explains what changes when you move to **SDK 7**. New integrations can use the same material as the canonical SDK 7 description.

This guide covers:

1. **Packaging** — how native SDK dependencies are pulled in on SDK 7 (iOS: **Swift Package Manager**; Android: **Maven BOM + RPC bridge**).
2. **`AppsFlyerX` on SDK 7** — schema-generated methods in `Classes/AppsFlyer/generated/`; transport in `AppsFlyerRpc.{h,cpp}`.

| Topic | Document |
| --- | --- |
| **`AppsFlyerX` API reference** | [API.md](API.md) |
| **iOS — step-by-step + checklist** | [iOS-README.md](iOS-README.md) |
| **Init order, threading, bridges** | [SDK7-Lifecycle.md](SDK7-Lifecycle.md) |
| **Android — Gradle, copy manifest, Activity hook** | [Android-README.md](Android-README.md) |

## How the plugin is distributed (game developers)

The Cocos2d-x AppsFlyer plugin is **not** a standalone registry package (no npm, CocoaPods spec, or Maven artifact for the bridge itself). You **copy source trees into your game** and wire your **existing** Android Studio / Xcode projects.

### Migrating from plugin 6.x — packaging (high level)

| Area | Plugin 6.x (typical) | SDK 7 |
| --- | --- | --- |
| **iOS native AppsFlyer SDK** | Static library / headers shipped with or alongside the old plugin tree | **SPM**: `AppsFlyerRPC` + `AppsFlyerLib-Static` on the app target ([iOS-README.md](iOS-README.md)) |
| **Android native AppsFlyer SDK** | Single Maven `af-android-sdk` coordinate or local JAR in `libs/` | **BOM** + unversioned **`af-android-sdk`** + **`af-android-plugin-bridge`** + Install Referrer ([Android-README.md](Android-README.md)) |
| **C++ bridge sources** | Hand-maintained `AppsFlyerX` implementation in the game tree | `AppsFlyerRpc` / `AppsFlyerTask` + `generated/` (refresh `Classes/AppsFlyer/` from this repo) |
| **Platform glue** | Obj‑C++/Java patterns from older samples | Kotlin `AppsFlyerCocosRpcBridge` (Android), `proj.ios_mac/ios/rpc/*` (iOS) |
| **macOS** | Supported on some 6.x lines | **Not supported** |

### SDK 7 layout (reference)

| Piece | What you do on SDK 7 |
| --- | --- |
| **Plugin bridge (C++)** | Copy `Classes/AppsFlyer/` (incl. `generated/`, `AppsFlyerRpc.*`, `AppsFlyerTask.*`) into the game; add sources to `CMakeLists.txt`, `Android.mk`, or the IDE project. |
| **iOS native SDK** | **Swift Package Manager** on the **app** target — not bundled in this repo. Minimum **iOS** = `minimumIos` in [`version/ios-spm-versions.json`](../version/ios-spm-versions.json). |
| **Android native SDK** | **Maven** BOM + RPC bridge in Gradle ([`version/android-sdk-versions.json`](../version/android-sdk-versions.json)); **minSdk ≥ API 21** on the app target. |
| **Platform bridges** | Kotlin RPC adapter (Android) and Obj‑C++/Swift bridge (iOS) — see platform READMEs. |
| **Codegen** | Maintainers: `./scripts/codegen/run.sh` after schema changes. Games normally consume committed `generated/` output. |
| **macOS desktop** | **Not supported** on SDK 7. |

### CMake vs non-CMake (iOS)

- **CMake + Xcode generator** — what **this repository’s sample** uses: `cmake -B build-device -GXcode …` produces `AppsFlyerCocos2dX.xcodeproj`. Run `./scripts/setup-ios-spm.sh` to inject SwiftPM refs and fix CMake’s legacy build-dir settings, then build with `./scripts/build-ios-cmake.sh` or Xcode.
- **Non-CMake** — **most production Cocos games**: you already have an Xcode project from Cocos Creator, `cocos compile`, or a hand-maintained `proj.ios_mac` tree. You **do not** need to adopt CMake. Follow **[iOS-README.md](iOS-README.md)** (copy manifest + Xcode steps).

### Main vs Strict (iOS native binaries)

AppsFlyer ships the iOS SDK in two **compile-time** variants:

| Variant | Typical use | Cocos2d-x SDK 7 consumption |
| --- | --- | --- |
| **Main (regular)** | Standard apps; can use ATT / IDFA when allowed | Default SPM products **`AppsFlyerRPC`** + **`AppsFlyerLib-Static`** from the public GitHub packages (pins in [`version/ios-spm-versions.json`](../version/ios-spm-versions.json)). |
| **Strict** (user-data–private / kids binary) | Apps that must ship AppsFlyer’s **Strict** native binary and **privacy manifest** (IDFA-related APIs compiled out) | In Xcode, depend on the **Strict** Swift package products for the same RPC/Framework versions (not `AppsFlyerLib-Static` Main). Add `AFSDK_NO_IDFA=1` on the app target. Product names change with SDK releases — use the [iOS SDK install guide](https://dev.appsflyer.com/hc/docs/install-ios-sdk#strict-mode-sdk). |

**Runtime** APIs such as `setDisableAdvertisingIdentifiers` only change behavior inside **whichever binary you linked**; they do **not** switch Main ↔ Strict. Pick the variant at **link time**.

Android does not use Main/Strict the same way as iOS; use Gradle BOM + bridge and follow Android SDK privacy docs.

## iOS (Swift Package Manager)

All step-by-step iOS work — **what to copy where**, integration cases (CMake vs manual Xcode), SPM pins, build settings, and troubleshooting — lives in **[iOS-README.md](iOS-README.md)**.

Summary only:

- Native SDKs: **`AppsFlyerRPC`** + **`AppsFlyerLib-Static`** via SwiftPM on the **app** target; exact versions in [`version/ios-spm-versions.json`](../version/ios-spm-versions.json).
- Repo sample: `./scripts/build-ios-cmake.sh` runs `cmake`, then `setup-ios-spm.sh`, then `xcodebuild` (pass `DEVELOPMENT_TEAM=<id>` as an extra xcodebuild setting when signing).
- After any manual `cmake -B`, run `./scripts/setup-ios-spm.sh` again before building.

## Migrating from plugin 6.x — API model

| Topic | Plugin 6.x (typical) | SDK 7 |
| --- | --- | --- |
| Calls | Synchronous-style `void` methods on `AppsFlyerX` | Methods return **`AppsFlyerTask`**; use **`whenComplete`** → **`AppsFlyerResult`** |
| Listeners | Setter functions on `AppsFlyerX` | **`registerDeepLinkListener`**, **`registerConversionListener`**, **`registerSessionReadyListener`** (separate RPCs) |
| Initialization | Dev key / app ID setters + implicit or batched startup | **`init(InitParams)`** after listener registration; **`start`** from session-ready callback |
| Deep links | Platform-specific result types in older samples | **`appsflyer::DeepLinkData`** in the deep-link listener |
| Purchases, ad revenue, consent | Dedicated helper types in the old tree | Schema **`…Params`** structs (`ValidateAndLogInAppPurchaseParams`, `LogAdRevenueParams`, `SetConsentDataParams`, …) — see [API.md](API.md) |

### SDK 7 API model (detail)

| Topic | SDK 7 behavior |
| --- | --- |
| Method shape | Schema-generated methods return **`AppsFlyerTask`**; handle results with **`whenComplete`** → **`AppsFlyerResult`** / **`AppsFlyerError`**. |
| Listeners | **`registerDeepLinkListener`**, **`registerConversionListener`**, **`registerSessionReadyListener`** — each is its own RPC; register **before** **`init`**. |
| Init | **`init(InitParams)`** with `devKey` and `appId` (iOS). |
| Start / events | **`start(awaitResponse)`**, **`logEvent(LogEventParams)`** with configurable **`awaitResponse`**. |
| Deep links | **`appsflyer::DeepLinkData`** in the deep-link listener callback. |

Public names do **not** include `Rpc`. Internal transport lives in `AppsFlyerRpc.cpp` (`executeRpc` / `executeBlocking`).

Full method list: [API.md](API.md).

## Recommended init sequence (SDK 7)

1. **`registerDeepLinkListener`** → on success → **`enableDebug`** → **`init(InitParams)`** with `devKey` / `appId` (iOS).
2. On init success → **`registerConversionListener`** → **`registerSessionReadyListener`**
3. In session-ready callback → **`start(awaitResponse)`** → **`logEvent`**
4. Internal **`setPluginInfo`** is sent by the plugin immediately before the first **`init`** wire — do not call it from game code.

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

See `Classes/AppDelegate.cpp` for the full sample chain and [SDK7-Lifecycle.md](SDK7-Lifecycle.md) for ordering rules.

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
cd proj.android-studio && ./gradlew :AppsFlyerCocos2dX:assembleDebug :AppsFlyerCocos2dX:testDebugUnitTest
```