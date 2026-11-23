# AppsFlyer Cocos2d-x — API reference (SDK 7)

Public surface: **`AppsFlyerX`** in [`Classes/AppsFlyer/AppsFlyerX.h`](../Classes/AppsFlyer/AppsFlyerX.h). Methods and parameter types are **schema-generated** in [`Classes/AppsFlyer/generated/`](../Classes/AppsFlyer/generated/). Native RPC is internal (`AppsFlyerRpc`).

| Related docs | |
| --- | --- |
| Init order, threading, bridges | [SDK7-Lifecycle.md](SDK7-Lifecycle.md) |
| Migrating from plugin 6.x → SDK 7 | [Migration-Guide.md](Migration-Guide.md) |
| Sample chain | [`Classes/AppDelegate.cpp`](../Classes/AppDelegate.cpp), [`snippets/ios-minimal-init.cpp`](snippets/ios-minimal-init.cpp) |

## Calling convention

- Every API returns **`AppsFlyerTask`**. Attach a completion handler with **`whenComplete`** or pass **`AppsFlyerCompletion`** as the last argument.
- Completions receive **`AppsFlyerResult`**: `success`, optional `dataJson` (RPC payload), and **`AppsFlyerError`** (`code`, `message`) on failure.
- Completions run on the **Cocos game thread** (see [SDK7-Lifecycle.md](SDK7-Lifecycle.md#3-threading)).
- Optional trailing `onComplete` on each method is equivalent to chaining `.whenComplete(...)`.

```cpp
#include "AppsFlyer/AppsFlyerX.h"

AppsFlyerX::enableDebug(true).whenComplete([](const AppsFlyerResult& r) {
    if (!r.success) {
        CCLOG("[AppsFlyer] enableDebug failed: %s", r.error.message.c_str());
        return;
    }
    // r.dataJson may contain RPC response body when the native SDK returns data
});
```

Parameter struct types (`InitParams`, `LogEventParams`, …) are nested on **`AppsFlyerX`** (codegen aliases from `appsflyer::` in `AppsFlyerRpcSchema.generated.h`).

## Types and enums

| Type | Purpose |
| --- | --- |
| `AppsFlyerX::InitParams` | `devKey` (required), `appId` (required on iOS — numeric App Store id) |
| `AppsFlyerX::LogEventParams` | `eventName`, `eventValues` (`cocos2d::ValueMap`), `awaitResponse` |
| `AppsFlyerX::LogAdRevenueParams` | mediation network, monetization network, revenue, currency, optional map |
| `AppsFlyerX::ValidateAndLogInAppPurchaseParams` | `InAppPurchaseDetails` + `additionalParameters` |
| `AppsFlyerX::SetConsentDataParams` | GDPR / DMA consent flags |
| `appsflyer::DeepLinkData` | Unified deep-link payload for `registerDeepLinkListener` (`status`, `error`, `deepLink` map) |

Generated enums (use as `AppsFlyerX::…`):

- **`PurchaseType`**: `OneTimePurchase`, `Subscription`
- **`MediationNetwork`**: e.g. `GoogleAdMob`, `ApplovinMax`, `IronSource`, … (full list in `AppsFlyerRpcSchema.generated.h`)
- **`LogLevel`**: `None`, `Error`, `Warning`, `Info`, `Debug`, `Verbose` (Android wire transform is applied by codegen)

**In-app event name constants** (e.g. `AFEventPurchase`) live in [`Classes/AppsFlyer/AppsFlyerXMacro.h`](../Classes/AppsFlyer/AppsFlyerXMacro.h).

## Quick start

Follow [SDK7-Lifecycle.md](SDK7-Lifecycle.md) for native hooks (Android `AppActivity`, iOS `installEventPipeline`). On the game thread in `applicationDidFinishLaunching()`:

1. `registerDeepLinkListener` → on success → optional `enableDebug` → **`init(InitParams)`**
2. `registerConversionListener` → **`registerSessionReadyListener`**
3. In the session-ready callback → **`start(awaitResponse)`** → `logEvent`, etc.

Minimal iOS-oriented chain (also works on Android — omit `appId` on Android if unused by your build):

```cpp
#include "AppsFlyer/AppsFlyerX.h"
#include "AppsFlyer/AppsFlyerDeepLinkData.h"

static void onSessionReady() {
    AppsFlyerX::start(false).whenComplete([](const AppsFlyerResult& r) {
        if (!r.success) return;
        AppsFlyerX::LogEventParams ev;
        ev.eventName = AFEventPurchase;
        ev.awaitResponse = false;
        ev.eventValues["af_revenue"] = 9.99;
        ev.eventValues["af_currency"] = "USD";
        AppsFlyerX::logEvent(ev);
    });
}

bool AppDelegate::applicationDidFinishLaunching() {
    AppsFlyerX::InitParams init;
    init.devKey = "YOUR_DEV_KEY";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    init.appId = "YOUR_APPLE_APP_ID";
#endif

    AppsFlyerX::registerDeepLinkListener([](appsflyer::DeepLinkData data) {
        if (data.status == appsflyer::DeepLinkStatus::Found) {
            auto value = data.deepLinkStringField("deep_link_value");
            CCLOG("UDL value: %s", value.c_str());
        }
    }).whenComplete([init](const AppsFlyerResult& rDl) {
        if (!rDl.success) return;
        AppsFlyerX::init(init).whenComplete([](const AppsFlyerResult& rInit) {
            if (!rInit.success) return;
            AppsFlyerX::registerConversionListener(
                [](cocos2d::ValueMap map) { /* attribution */ },
                [](cocos2d::ValueMap map) { /* failure */ })
                .whenComplete([](const AppsFlyerResult& rConv) {
                    if (!rConv.success) return;
                    AppsFlyerX::registerSessionReadyListener(onSessionReady);
                });
        });
    });

    // ... Cocos director setup ...
    return true;
}
```

**Delayed start (consent / CMP):** complete consent (`setConsentData` or `enableTCFDataCollection` + CMP UI), then call **`start`** from `registerSessionReadyListener` — do not call `start` until consent is ready. There is no separate `setManualStart` in SDK 7; you control when `start` runs in your session-ready handler.

### Android foreground / background

Call **`start`** when the app becomes active (e.g. `applicationWillEnterForeground`). On background:

```cpp
void AppDelegate::applicationDidEnterBackground() {
    AppsFlyerX::onPause();
}
```

`logSession` is available when you need an explicit session RPC outside the normal start flow.

## Listeners

| Method | Callback | Unregister |
| --- | --- | --- |
| `registerDeepLinkListener` | `std::function<void(appsflyer::DeepLinkData)>` | `unregisterDeepLinkListener` |
| `registerConversionListener` | success / fail `void (*)(cocos2d::ValueMap)` | `unregisterConversionListener` |
| `registerSessionReadyListener` | `void (*)()` | `unregisterSessionReadyListener` |

Register listeners **before** `init` (see sample). Each `register*` issues its own native RPC.

`unregister*Listener` clears the C++ callback **synchronously on the calling thread before** any native unsubscribe RPC runs. After the returned `AppsFlyerTask` completes successfully, your handler will not be invoked again — including events already queued for the game thread. On iOS there is no native unsubscribe for deep link / conversion listeners; clearing the C++ registry is what stops callbacks.

```cpp
AppsFlyerX::registerConversionListener(
    [](cocos2d::ValueMap data) { CCLOG("conversion: ok"); },
    [](cocos2d::ValueMap err) { CCLOG("conversion: fail"); })
    .whenComplete([](const AppsFlyerResult& r) { /* registration RPC result */ });
```

## Events and ad revenue

### `logEvent`

```cpp
#include "AppsFlyer/AppsFlyerXMacro.h"

AppsFlyerX::LogEventParams params;
params.eventName = AFEventPurchase;
params.awaitResponse = false; // or true to block until native SDK acknowledges
params.eventValues["af_revenue"] = 19.99;
params.eventValues["af_currency"] = "USD";
AppsFlyerX::logEvent(params).whenComplete([](const AppsFlyerResult& r) {
    if (!r.success) CCLOG("logEvent error: %s", r.error.message.c_str());
});
```

### `logAdRevenue`

```cpp
AppsFlyerX::LogAdRevenueParams params;
params.monetizationNetwork = "myNetwork";
params.mediationNetwork = AppsFlyerX::MediationNetwork::ApplovinMax;
params.currencyIso4217Code = "USD";
params.revenue = 7.99;
params.additionalParameters["custom_key"] = "custom_value";
AppsFlyerX::logAdRevenue(params);
```

### Cross-promote and invites

- **`logCrossPromoteImpression`**, **`logAndOpenStore`** — `LogCrossPromoteImpressionParams` / `LogAndOpenStoreParams`
- **`logInvite`**, **`generateInviteLink`** — channel / parameters in generated structs

## Deep linking

- **Unified Deep Linking:** use **`registerDeepLinkListener`**; inspect `DeepLinkData::status` and `deepLink` map (`deep_link_value`, `media_source`, …).
- **Open URL from game code:** `PerformDeepLinkingParams` with `url` and optional `shouldTriggerSession` → **`performDeepLinking`**
- **Deferred:** **`performDeferredDeepLinking`**
- **Configuration:** `setDeepLinkTimeout`, `setResolveDeepLinkURLs`, `setOneLinkCustomDomain`, `appendParametersToDeepLinkingURL`, `addPushNotificationDeepLinkPath`, `disableAutomaticDeferredDeepLink`, `enableFacebookDeferredApplinks`, `setFacebookDeferredAppLink`

**iOS URL / universal-link forwarding** from UIKit into the RPC bridge is still an integrator responsibility where required — see [SDK7-Lifecycle.md](SDK7-Lifecycle.md#5-limitations).

```cpp
AppsFlyerX::PerformDeepLinkingParams p;
p.url = "https://your.onelink.me/...";
p.shouldTriggerSession = true;
AppsFlyerX::performDeepLinking(p);
```

## In-app purchase validation

Platform-specific purchase identifiers are on **`InAppPurchaseDetails`** (codegen selects fields per platform).

```cpp
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
AppsFlyerX::ValidateAndLogInAppPurchaseParams params;
params.purchase.purchaseType = AppsFlyerX::PurchaseType::OneTimePurchase;
params.purchase.productId = "sku";
params.purchase.purchaseToken = "token";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
AppsFlyerX::ValidateAndLogInAppPurchaseParams params;
params.purchase.purchaseType = AppsFlyerX::PurchaseType::Subscription;
params.purchase.productId = "sku";
params.purchase.transactionId = "txn_id";
#endif
params.additionalParameters["key"] = "value";
AppsFlyerX::validateAndLogInAppPurchase(params).whenComplete([](const AppsFlyerResult& r) {
    if (r.success) CCLOG("validation: %s", r.dataJson.c_str());
});
```

iOS sandbox: **`setUseReceiptValidationSandbox`**. Uninstall measurement (iOS sandbox): **`setUseUninstallSandbox`**.

## Privacy, consent, and opt-out

### DMA / GDPR consent (manual)

```cpp
AppsFlyerX::SetConsentDataParams consent;
consent.isUserSubjectToGDPR = true;
consent.hasConsentForDataUsage = true;
consent.hasConsentForAdsPersonalization = true;
consent.hasConsentForAdStorage = true;
AppsFlyerX::setConsentData(consent);
```

Non-GDPR users: set `isUserSubjectToGDPR = false` and call `setConsentData` before `start`.

### TCF / CMP

```cpp
AppsFlyerX::enableTCFDataCollection(true);
// Show CMP, wait until consent is persisted, then start() from session-ready callback
```

See [AppsFlyer DMA consent documentation](https://dev.appsflyer.com/hc/docs/send-consent-for-dma-compliance).

### Other privacy helpers

`anonymizeUser`, `clearUserPii`, `setSharingFilterForPartners`, `setDisableAdvertisingIdentifiers`, `setDisableAppleAdsAttribution`, `setDisableCollectASA`, `setDisableSKAdNetwork`, `setDisableIDFVCollection`, `setDisableNetworkData` (Android), `disableAppSetId` (Android), `enableTCFDataCollection`.

## User identity and device data

| Method | Notes |
| --- | --- |
| `setCustomerUserId` | Preferred stable user id |
| `setUserEmail`, `setUserFirstName`, `setUserLastName`, `setUserFbLoginId`, `setUserPhone` | PII — use after consent |
| `setAdditionalData` | Attach custom `ValueMap` to payloads |
| `setCurrencyCode`, `setCurrentDeviceLanguage` | User / store context |
| `setImeiData`, `setOaidData`, `setAndroidIdData` | Android identifiers when permitted |
| `setCollectAndroidID` | Toggle Android ID collection |
| `setShouldCollectDeviceName` | iOS device name |
| `setPartnerData` | Partner-specific metadata map |

## SDK control and queries

| Method | Description |
| --- | --- |
| `init` | Initialize with `InitParams` |
| `start(awaitResponse)` | Start or resume measuring; `awaitResponse` mirrors native start contract |
| `stop(shouldStop)` | Pause / resume SDK activity (`shouldStop == true` pauses) |
| `enableDebug` | Verbose SDK logging |
| `setLogLevel` | Structured log level (`LogLevel`) |
| `fetchAppsFlyerUID` | AppsFlyer UID string in `dataJson` on success |
| `getSdkVersion`, `isSessionReady`, `isStopped`, `isPreInstalledApp` | Query helpers |
| `getAttributionId` | Android attribution id |
| `getHostName`, `getHostPrefix`, `getOutOfStore`, `setHost`, `setOutOfStore` | Custom host / store source |
| `setMinTimeBetweenSessions` | Minimum seconds between sessions |
| `setAppId` | Set Apple app id after init (iOS) when needed |
| `setInstallId`, `setIsUpdate` | Install / update hints |

## Push notifications

- **`handlePushNotification`** — `cocos2d::ValueMap` payload
- **`sendPushNotificationData`** — campaign / pid / retargeting parameters
- **`updateServerUninstallToken`** — FCM/APNs token for uninstall tracking

## Android-only

`collectDataFromLauncherActivity`, `setDisableNetworkData`, `disableAppSetId`, `setCollectAndroidID`, `setImeiData`, `setOaidData`, `setAndroidIdData`, `getAttributionId`.

## iOS-only (typical)

`setDisableAppleAdsAttribution`, `setDisableCollectASA`, `setDisableSKAdNetwork`, `setDisableIDFVCollection`, `setShouldCollectDeviceName`, `setUseReceiptValidationSandbox`, `setUseUninstallSandbox`, `setAppId`, `setCurrentDeviceLanguage`.

Exact availability follows the [RPC schema](../schema/appsflyer-plugins-rpc-schema.json); a method with no native implementation on a platform fails the RPC with an error result.

## Full method index

All methods return **`AppsFlyerTask`** and accept an optional trailing **`AppsFlyerCompletion`**.

| Method | Summary |
| --- | --- |
| `addPushNotificationDeepLinkPath` | Deep-link path segments for push-opens |
| `anonymizeUser` | Opt out of personalized ads / anonymize |
| `appendParametersToDeepLinkingURL` | Append query params when URL contains substring |
| `clearUserPii` | Clear stored user PII |
| `collectDataFromLauncherActivity` | Android launcher intent data |
| `disableAppSetId` | Stop collecting App Set ID (Android) |
| `disableAutomaticDeferredDeepLink` | Disable automatic deferred deep link handling |
| `enableFacebookDeferredApplinks` | Enable Facebook deferred app links |
| `enableTCFDataCollection` | Read TCF consent from device storage |
| `generateInviteLink` | Create OneLink invite URL |
| `getAttributionId` | Android attribution identifier |
| `fetchAppsFlyerUID` | Get AppsFlyer unique device id |
| `getHostName` | Read custom hostname |
| `getHostPrefix` | Read custom host prefix |
| `getOutOfStore` | Read out-of-store source name |
| `getSdkVersion` | Native SDK version string |
| `handlePushNotification` | Report push notification open |
| `init` | Initialize SDK (`InitParams`) |
| `isPreInstalledApp` | Preinstall detection flag |
| `isSessionReady` | Whether session is ready for `start` |
| `isStopped` | Whether SDK is in stopped state |
| `logAdRevenue` | Monetization / mediation revenue event |
| `logAndOpenStore` | Cross-promote: log and open store listing |
| `logCrossPromoteImpression` | Cross-promote impression |
| `logEvent` | In-app event |
| `logInvite` | User invite event |
| `logLocation` | Geo location event |
| `logSession` | Explicit session log RPC |
| `onPause` | App moved to background (Android lifecycle) |
| `performDeepLinking` | Handle incoming deep link URL |
| `performDeferredDeepLinking` | Trigger deferred deep link resolution |
| `registerConversionListener` | Install / conversion callbacks |
| `registerDeepLinkListener` | UDL / deep link callback |
| `registerSessionReadyListener` | Session ready → call `start` here |
| `sendPushNotificationData` | Push-related attribution parameters |
| `setAdditionalData` | Global custom data map |
| `setAppId` | Apple App Store id (iOS) |
| `setAppInviteOneLink` | OneLink template id for invites |
| `setCollectAndroidID` | Toggle Android ID collection |
| `setConsentData` | GDPR / DMA consent payload |
| `setCurrencyCode` | Default currency |
| `setCurrentDeviceLanguage` | Device language (iOS) |
| `setCustomerUserId` | Customer user id |
| `enableDebug` | SDK debug mode |
| `setDeepLinkTimeout` | Deep link resolution timeout (ms) |
| `setDisableAdvertisingIdentifiers` | Disable advertising ID collection |
| `setDisableAppleAdsAttribution` | Disable Apple Search Ads (iOS) |
| `setDisableCollectASA` | Disable Apple ads attribution collection (iOS) |
| `setDisableIDFVCollection` | Disable IDFV (iOS) |
| `setDisableNetworkData` | Disable network operator data (Android) |
| `setDisableSKAdNetwork` | Disable SKAdNetwork (iOS) |
| `setFacebookDeferredAppLink` | Facebook deferred link URL |
| `setHost` | Custom attribution host |
| `setInstallId` | Custom install id |
| `setIsUpdate` | Mark update install |
| `setLogLevel` | SDK log level |
| `setMinTimeBetweenSessions` | Min seconds between sessions |
| `setOneLinkCustomDomain` | Custom OneLink domains |
| `setOutOfStore` | Alternate store source name |
| `setImeiData` | IMEI (Android, where allowed) |
| `setOaidData` | OAID (Android) |
| `setAndroidIdData` | Android ID |
| `setPartnerData` | Partner-specific data blob |
| `setPreinstallAttribution` | Preinstall campaign attribution |
| `setResolveDeepLinkURLs` | URLs to resolve for deep linking |
| `setSharingFilterForPartners` | Exclude data sharing for partners |
| `setShouldCollectDeviceName` | Collect device name (iOS) |
| `setUseReceiptValidationSandbox` | IAP validation sandbox (iOS) |
| `setUserEmail` | User email |
| `setUserFbLoginId` | Facebook login id |
| `setUserFirstName` / `setUserLastName` | User name |
| `setUserPhone` | Phone + country code |
| `setUseUninstallSandbox` | Uninstall sandbox (iOS) |
| `start` | Start SDK (`awaitResponse` flag) |
| `stop` | Pause or resume SDK |
| `unregisterConversionListener` | Remove conversion listener |
| `unregisterSessionReadyListener` | Remove session-ready listener |
| `unregisterDeepLinkListener` | Remove deep link listener |
| `updateServerUninstallToken` | Push token for uninstall |
| `validateAndLogInAppPurchase` | Receipt / token validation and event |
