<img src="https://massets.appsflyer.com/wp-content/uploads/2018/06/20092440/static-ziv_1TP.png"  width="400" > 

# Cocos2dX AppsFlyer plugin for Android and iOS.

> **SDK 7 (RPC):** The sample app and new integrations use schema-generated `AppsFlyerX` methods (`AppsFlyerTask`, `*Params` types under `Classes/AppsFlyer/generated/`). Much of this README still describes the **6.x** void API for reference. For init order, listener registration, and removed helpers (`AFSDKXPurchaseDetails`, `AFXAdRevenueData`, `AppsFlyerXConsent`, etc.), see **[`doc/migration-guide.md`](doc/migration-guide.md)**.

----------

🛠 In order for us to provide optimal support, please contact AppsFlyer support through the Customer Assistant Chatbot for assistance with troubleshooting issues or product guidance. </br>
To do so, please follow [this article](https://support.appsflyer.com/hc/en-us/articles/23583984402193-Using-the-Customer-Assistant-Chatbot)

----------

## Table of content

- [integration](#integration)
- [Usage](#usage)
- [Manual start mode](#manual-start)
- [API methods](#api-methods)
 - [setIsDebug](#setIsDebug)
 - [stop](#stopTracking) 
 - [sharingFilter](#sharingFilter) [Deprecated]
 - [sharingFilterForAllPartners](#sharingFilterForAllPartners) [Deprecated]
 - [logEvent](#trackEvent)
 - [getConversion Listener](#getConversionListener)
 - [setUserEmails](#setUserEmails)
 - [setCustomerUserID](#setCustomerUserID)
 - [setCurrencyCode](#setCurrencyCode)
 - [waitForATTUserAuthorizationWithTimeoutInterval](#waitForATTUserAuthorizationWithTimeoutInterval) *(ios only)*
 - [disableAdvertiserIdentifier](#disableAppleAdSupportTracking)
 - [setShouldCollectDeviceName](#setShouldCollectDeviceName)*(ios only)*
 - [setAppInviteOneLink](#setAppInviteOneLink)
 - [anonymizeUser opt-out](#deviceTrackingDisabled)
 - [disableCollectASA](#disableIAdTracking)*(ios only)*
 - [setUseReceiptValidationSandbox](#setUseReceiptValidationSandbox) *(ios olny)*
 - [setUseUninstallSandbox](#setUseUninstallSandbox) *(ios only)*
 - validateAndLogInAppPurchase
    - [Android](#validateAndTrackInAppPurchase-a)
    - [iOS](#validateAndTrackInAppPurchase-i)
  - [getAppsFlyerUID](#getAppsFlyerUID)
  - [handleOpenURL Deep-linking](#getAppsFlyerUID) *(ios only)*
  - Uninstall
    - [Android](#registerUninstallAndroid)
    - [iOS](#registerUninstallIOS) 
  - [setHost](#setHost)
  - [setMinTimeBetweenSessions](#setMinTimeBetweenSessions)
  - [deep linking Logging](#deep-linking-tracking)
  - generateInviteLink - TBD
  - logCrossPromotionImpression - TBD
  - logAndOpenStore - TBD
   - [Unified deep linking](#ddl)
   - [setPartnerData](#partnerData)
- [setOneLinkCustomDomain](#customDomains)
- [setCurrentDeviceLanguage](#currentLang) *(ios only)*
- [setSharingFilterForPartners](#SharingFilterForPartners) 
- [setDisableNetworkData](#disableNetworkID)  *(android only)*
- [Send consent for DMA compliance](#dma_support) 
- [validateAndLog](#validate_and_log_2.0) 
- [logAdrevenue](#logAdrevenue)


### <a id="plugin-build-for"> This plugin is built for

The **SDK 7 RPC** line (see `docs/iOS_README.md`, `docs/Android_README.md`, and `docs/SDK7_LIFECYCLE.md`) targets:

- Android: **af-android-sdk BOM 7.0.4**, **af-android-plugin-bridge 7.0.13**
- iOS: **AppsFlyerFramework 7.0.4**, **AppsFlyerRPC 7.0.41** (CocoaPods)

The API sections below describe the historical **SDK 6** `AppsFlyerX` surface; many calls are stubs on the RPC path until later delivery phases.


### <a id="integration"> Integration:
 - [Android integration Documents](docs/Android_README.md)
 - [iOS integration Documents](docs/iOS_README.md)

 



## <a id="usage"> Usage:

#### 1\. Set your  Dev_Key and enable AppsFlyer to detect installations, sessions (app opens) and updates.
> This is the minimum requirement to start measuring your app installs and is already implemented in this plugin. You **MUST** modify this call and provide:
**-devKey** - Your application devKey provided by AppsFlyer.
**-appId**  - (*For iOS only*) Your iTunes Application ID.



Add the following lines into `applicationDidFinishLaunching` and to `applicationWillEnterForeground` method to be able to initialize measuring with your own AppsFlyer dev key:


```cpp
#include "AppsFlyer/AppsFlyerX.h"

bool AppDelegate::applicationDidFinishLaunching() {
  AppsFlyerX::setAppsFlyerDevKey("YOUR_DEV_KEY");

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerX::setAppleAppID("942960987");
#endif
  
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AppsFlyerX::start();
#endif
}

void AppDelegate::applicationWillEnterForeground() {
   //..
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AppsFlyerX::start();
#endif
}
```

Also notify AppsFlyer SDK about background event:

```cpp
void AppDelegate::applicationDidEnterBackground() {

    //...
    AppsFlyerX::didEnterBackground();
}

```

## <a id="manual-start"> Manual mode:
Starting version 6.13.0, we support a manual mode to seperate the initialization of the AppsFlyer SDK and the start of the SDK. In this case, the AppsFlyer SDK won't start automatically, giving the developer more freedom when to start the AppsFlyer SDK. Please note that in manual mode, the developer is required to implement the API start() in order to start the SDK.
<br>If you are using CMP to collect consent data this feature is needed. See explanation [here](#dma_support).
### Examples:
 
```cpp
bool AppDelegate::applicationDidFinishLaunching() {
...
    AppsFlyerX::setAppsFlyerDevKey("devkey");

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    // In case you want to use manual mode. 
    AppsFlyerX::setManualStart(true);
    // 
    AppsFlyerX::setAppleAppID("appleAppId");
...
}
```
- See that we aren't calling any AppsFlyerX::start() in any case. 
The init function is called in the background in order to catch any deeplinking.


And to start the AppsFlyer SDK:

### Example:

```cpp
//In case you use manual mode
 #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
     AppsFlyerX::setManualStart(false);
 #endif
 AppsFlyerX::start();
 // 
 ```


## <a id="api-methods"> API Methods

---

##### <a id="setIsDebug"> **`setIsDebug(boolean);`**

 if `true` AppsFlyer SDK will run in debug mode


*Example:*

```cpp
AppsFlyerX::setIsDebug(true);
```

---

##### <a id="stopTracking"> **`stopTracking(boolean);`**

 if `true` AppsFlyer SDK will enter to pending mode, no activity
Be sure to set `stop(false)` to release the SDK from stop measuring events

*Example:*

```cpp
AppsFlyerX::stop(false); // or false
```
---
##### <a id="sharingFilter"> **`sharingFilter(partners);`**
 Use to prevent sharing data with some (one or more) networks/integrated partners.

| parameter   | type                        | description |
| ----------- |-----------------------------|--------------|
| `eventName` | `std::vector<std::string>`  | list of partners


*Example:*

```cpp
  std::vector<std::string> partners;
  partners.push_back("facebook_int");
  partners.push_back("googleadwords_int");
  AppsFlyerX::sharingFilter(partners);
```
---
##### <a id="sharingFilterForAllPartners"> **`sharingFilterForAllPartners();`**

Use to prevent sharing data with all networks/integrated partners.

```cpp
AppsFlyerX::sharingFilterForAllPartners();
```
___

##### <a id="validate_and_log_2.0"> **`Validate and log (SDK 7)`**

SDK 7 exposes a single RPC method. Build `ValidateAndLogInAppPurchaseParams` with typed **`InAppPurchaseDetails`** (`PurchaseType`, `productId`, and `purchaseToken` on Android or `transactionId` on iOS) plus optional **`additionalParameters`**. Codegen maps to native RPC wire (see [`doc/migration-guide.md`](doc/migration-guide.md)).

*Example (Android):*
```cpp
AppsFlyerX::ValidateAndLogInAppPurchaseParams params;
params.purchase.purchaseType = AppsFlyerX::PurchaseType::OneTimePurchase;
params.purchase.productId = "product_id";
params.purchase.purchaseToken = "purchase_token";
params.additionalParameters["key"] = "value";
AppsFlyerX::validateAndLogInAppPurchase(params).whenComplete([](const AppsFlyerResult& r) {
    if (!r.success) { CCLOG("Validation failed: %s", r.error.message.c_str()); return; }
    CCLOG("Validation response: %s", r.dataJson.c_str());
});
```

6.x `AFSDKXPurchaseDetails` and per-platform overloads were **removed** — see [`doc/migration-guide.md`](doc/migration-guide.md).

---


##### <a id="logAdrevenue"> **`Log AdRevenue (SDK 7)`**

Use **`LogAdRevenueParams`** with **`AppsFlyerX::MediationNetwork`** (`enum class`, generated in `AppsFlyerRpcSchema.generated.h` from the schema; encoded to native wire values per platform at RPC time).

*Example:*
```cpp
AppsFlyerX::LogAdRevenueParams params;
params.monetizationNetwork = "someVal";
params.mediationNetwork = AppsFlyerX::MediationNetwork::ApplovinMax;
params.currencyIso4217Code = "USD";
params.revenue = 7.99;
AppsFlyerX::logAdRevenue(params);
```

The 6.x **`AFXAdRevenueData`** helper class was removed — see [`doc/migration-guide.md`](doc/migration-guide.md).


---

##### <a id="logEvent"> **`trackEvent(eventName, eventValues): void`** (optional)


- These in-app events help you track how loyal users discover your app, and attribute them to specific
campaigns/media-sources. Please take the time define the event/s you want to measure to allow you
to track ROI (Return on Investment) and LTV (Lifetime Value).
- The `logEvent` method allows you to send in-app events to AppsFlyer analytics. This method allows you to add events dynamically by adding them directly to the application code.


| parameter   | type                        | description |
| ----------- |-----------------------------|--------------|
| `eventName` | `string`                    | custom event name, is presented in your dashboard.  See the Event list [HERE](https://github.com/AppsFlyerSDK/AppsFlyerCocos2dX/blob/dev/RD-8680/Android-docs/Classes/AppsFlyer/AppsFlyerXMacro.h)  |
| `eventValues` | `cocos2d::ValueMap`                    | event details |

*Example:*

```cpp
//basic implementation
AppsFlyerX::logEvent(AFEventPurchase, {{ "key1", cocos2d::Value("value1")},
                                         { "key2", cocos2d::Value("value2")}});

//rich in-app-event implementation:
AppsFlyerX::logEvent(AFEventPurchase, {
                        { AFEventParamContentId, Value({Value("12344"), Value("98844"), Value("39944")})},
                        { AFEventParamCurrency, Value({Value(20), Value(11), Value(61)})},
                        { AFEventParamPrice, Value({Value(25), Value(50), Value(10)})},
                        { AFEventParamContentType, Value("ELECTRONIC")},
                        { AFEventParamCurrency, Value("USD")},
                        {AFEventParamRevenue, cocos2d::Value("10.67")}
                });
```
---

*More info about rich in-app-events you can find* [HERE](https://support.appsflyer.com/hc/en-us/articles/115005544169#multiple-items)

##### <a id="getConversionListener"> **`Attribution Data callback`**

AppsFlyer allows you to access the user attribution data in real-time directly at SDK level. It enables you to customize the landing page a user sees on the very first app open after a fresh app install.

To access AppsFlyer's conversion data from the Android SDK implement the ConversionDataListener callback:

*Examples:*

```cpp

static void onConversionDataReceived(cocos2d::ValueMap installData) {}

static void onConversionDataRequestFailure(cocos2d::ValueMap map) {
/*has signature {
                 {"status": "failure"},
                 {"data", "errorMessage"}
                 }*/
}

static void onAppOpenAttribution(cocos2d::ValueMap map) {}

static void onAppOpenAttributionFailure(cocos2d::ValueMap map) {
/*has signature {
                 {"status": "failure"},
                 {"data", "errorMessage"}
                 }*/
}

AppsFlyerX::setOnConversionDataReceived(onConversionDataReceived);
AppsFlyerX::setOnConversionDataRequestFailure(onConversionDataRequestFailure);
AppsFlyerX::setOnAppOpenAttribution(onAppOpenAttribution);
AppsFlyerX::setOnAppOpenAttributionFailure(onAppOpenAttributionFailure);
```
---

##### <a id="setUserEmails"> **`setUserEmail(email)` (SDK 7)**

SDK 7 exposes **`setUserEmail`** with a single email per call. The 6.x **`setUserEmails`** / **`EmailCryptTypeX`** API was removed. Check the native SDK and [developer docs](https://dev.appsflyer.com/) for hashing requirements before calling.

*Example:*

```cpp
AppsFlyerX::setUserEmail("user@example.com");
```

---

##### <a id="setCustomerUserID"> **`setCustomerUserID(customerUserId): void`**


Setting your own Custom ID enables you to cross-reference your own unique ID with AppsFlyer’s user ID and the other devices’ IDs. This ID is available in AppsFlyer CSV reports along with postbacks APIs for cross-referencing with you internal IDs.

**Note:** The ID must be set during the first launch of the app at the SDK initialization. The best practice is to call this API during the `deviceready` event, where possible.


| parameter   | type                        | description |
| ----------- |-----------------------------|--------------|
| `customerUserId`   | `const std::string&`                      | |

*Example:*

```cpp
AppsFlyerX::customerUserId("<USER_ID>");
```
---

##### <a id="setCurrencyCode"> **`setCurrencyCode(currencyId): void`**


| parameter   | type                  | Default     | description |
| ----------- |-----------------------|-------------|-------------|
| `currencyId`| `const std::string&`              |   `USD`     |  [ISO 4217 Currency Codes](http://www.xe.com/iso4217.php)           |

*Examples:*

```cpp
AppsFlyerX::setCurrencyCode("<CURRENCY_ID>");
```

---
##### <a id="waitForATTUserAuthorizationWithTimeoutInterval"> **`waitForATTUserAuthorizationWithTimeoutInterval(double): void`** *(ios only)*
| parameter   | type                  | Default     | description |
| ----------- |-----------------------|-------------|-------------|
| `timeoutInterval`| `double`         |             |  See additional info [HERE](https://support.appsflyer.com/hc/en-us/articles/207032066-iOS-SDK-V6-X-integration-guide-for-developers#api-reference-waitforattuserauthorization) |

---

##### <a id="disableAdvertiserIdentifier"> **`disableAdvertiserIdentifier(bool): void`**


| parameter   | type                  | Default     | description |
| ----------- |-----------------------|-------------|-------------|
| `flag`| `bool`              |   `false`     | disables AdvertiserIdentifier|

*Examples:*

```cpp
AppsFlyerX::disableAdvertiserIdentifier(true);
```

---

##### <a id="setShouldCollectDeviceName"> **`setShouldCollectDeviceName(bool): void`** *(ios only)*


| parameter   | type                  | Default     | description |
| ----------- |-----------------------|-------------|-------------|
| `flag`| `bool`              |   `false`     | enables/disables device name tracking|

*Examples:*

```cpp
AppsFlyerX::setShouldCollectDeviceName(true);
```

---

##### <a id="setAppInviteOneLink"> **`setAppInviteOneLink(string): void`** 


| parameter   | type                  | Default     | description |
| ----------- |-----------------------|-------------|-------------|
| `appInviteOneLinkID`| `std::string&`              |       | Before calling start in your app, set the OneLink which is invoked according to the OneLink ID. See additional info [HERE](https://support.appsflyer.com/hc/en-us/articles/115004480866-User-Invite-Tracking)|

*Examples:*

```cpp
AppsFlyerX::setAppInviteOneLink("8eOw");
```

---

##### <a id="anonymizeUser"> **`deviceTrackingDisabled(bool): void`**  


| parameter   | type                  | Default     | description |
| ----------- |-----------------------|-------------|-------------|
| `flag`| `bool`              |   `false`     | AppsFlyer provides you a method to opt-out specific users from AppsFlyer analytics. This method complies with the latest privacy requirements and complies with Facebook data and privacy policies. Default is false. **Warning** Opting out users SEVERELY hurts your attribution information. Use this option ONLY on regions which legally bind you from collecting your users' information.|

*Examples:*

```cpp
AppsFlyerX::anonymizeUser(true);
```

---

##### <a id="disableCollectASA"> **`disableIAdTracking(bool): void`** *(ios only)*


| parameter   | type                  | Default     | description |
| ----------- |-----------------------|-------------|-------------|
| `flag`| `bool`              |   `false`     | enables/disables collect ASA|

*Examples:*

```cpp
AppsFlyerX::disableCollectASA(true);
```

---



##### <a id="setUseReceiptValidationSandbox"> **`setUseReceiptValidationSandbox(bool): void`** *(ios only)*


| parameter   | type                  | Default     | description |
| ----------- |-----------------------|-------------|-------------|
| `flag`| `bool`              |   `false`     | enables Receipt Validation Sandbox|

*Examples:*

```cpp
AppsFlyerX::setUseReceiptValidationSandbox(true);
```

---

##### <a id="setUseUninstallSandbox"> **`setUseUninstallSandbox(bool): void`** *(ios only)*


| parameter   | type                  | Default     | description |
| ----------- |-----------------------|-------------|-------------|
| `flag`| `bool`              |   `false`     | enables Uninstall Sandbox mode|

*Examples:*

```cpp
AppsFlyerX::setUseUninstallSandbox(true);
```

---

##### <a id="validateAndLogInAppPurchase"> **`validateAndLogInAppPurchase` (SDK 7)**

Android and iOS share one method. Fill **`ValidateAndLogInAppPurchaseParams::purchase`** (`InAppPurchaseDetails`) per platform. See [Validate and log (SDK 7)](#validate_and_log_2.0) and [`doc/migration-guide.md`](doc/migration-guide.md). Legacy Android/iOS-specific overloads were removed.

---

##### <a id="getAppsFlyerUID"> **`getAppsFlyerUID(): string`**


*Examples:*

```cpp 
std::string appsflyerId = AppsFlyerX::getAppsFlyerUID();
```

---

##### <a id="handleOpenURL"> **`Deep-linking handleOpenURL(): void`** *(ios only)*

- `void AppsFlyerX::handleOpenURL(const std::string& url, const std::string& sourceApplication)`
- `void AppsFlyerX::handleOpenURL(std::string url, std::string sourceApplication, void* annotation)`
- `void AppsFlyerX::handleOpenURL(std::string url, cocos2d::ValueMap options)`

*Examples:*

```cpp 
TBD
```

---

##### <a id="registerUninstallAndroid"> **`registerUninstall(const std::string& token): void`** *(Android only)*

*Examples:*

```cpp
AppsFlyerX::registerUninstall("<TOKEN>");
```

For more Info see the [DOCs](https://support.appsflyer.com/hc/en-us/articles/208004986-Android-Uninstall-Tracking)





##### <a id="registerUninstallIOS"> **`registerUninstall(void* deviceToken, unsigned long length): void`** *(ios only)*

You can use the `registerUninstall(void* deviceToken, unsigned long length)` API.

 *or*
 
 On SDK 7, register for push in `AppController.mm` as in the sample project, then forward the device token through the RPC bridge when `registerUninstall` is wired in the sample app. See `docs/SDK7_LIFECYCLE.md`.



---

##### <a id="setHost"> **`setHost(host): void`** 


| parameter   | type                  | Default     | description |
| ----------- |-----------------------|-------------|-------------|
| `host`| `std::string&`              |       | host name that should override `appsflyer.com`|
*Examples:*

```cpp
AppsFlyerX::setHost("mynewhost.com");
```

---

##### <a id="setMinTimeBetweenSessions"> **`setMinTimeBetweenSessions(long): void`** 


| parameter   | type                  | Default     | description |
| ----------- |-----------------------|-------------|-------------|
| `minTimeBetweenSessions`| `unsigned long`              |       | host name that should override `appsflyer.com`|
*Examples:*

```cpp
AppsFlyerX::setMinTimeBetweenSessions(9);
```

---



##### <a id="updateServerUninstallToken"> **`updateServerUninstallToken("token"): void`**

Allows to pass GCM/FCM Tokens that where collected by third party plugins to the AppsFlyer server.
Can be used for Uninstall measuring.


| parameter   | type                        | description |
| ----------- |-----------------------------|--------------|
| `token`   | `String`                      | GCM/FCM Token|


---






### <a id="deep-linking-tracking"> Deep linking Tracking

#### <a id="dl-android"> Android
In ver. >4.2.5 deeplinking metadata (scheme/host) is sent automatically
TBD

#### <a id="dl-ios"> iOS URL Types
TBD
 
 ### <a id="ddl"> Unified deep linking
```cpp
// Register before init/start — see docs/SDK7_LIFECYCLE.md (SDK 7 sample AppDelegate).
AppsFlyerX::setDidResolveDeepLink(didResolveDeepLink);

static void didResolveDeepLink(appsflyer::DeepLinkData result){
    CCLOG("%s", "AppDelegate.cpp got ddl!");
    std::string ddl = "Deep link data is \n";
    switch (result.status) {
    case appsflyer::DeepLinkStatus::NotFound:
            CCLOG("deep link not found");
            break;
        case appsflyer::DeepLinkStatus::Found:
            if (!result.deepLink.empty()){
                const std::string mediaSource = result.deepLinkStringField("media_source");
                if (!mediaSource.empty()) {
                    CCLOG("Media source is %s", mediaSource.c_str());
                }
                for (auto &t : result.deepLink){
                    CCLOG("%s - %s", t.first.c_str(), t.second.asString().c_str());
                    ddl.append(t.first.c_str());
                    ddl.append(" : ");
                    ddl.append(t.second.asString().c_str());
                    ddl.append("\n");
                }
            }
    }
    }
}
```
--- 
 
##### <a id="partnerData"> **`setPartnerData("partnerId", data): void`**

Partners and advertisers can add more data in SDK events.


| parameter   | type                        | description |
| ----------- |-----------------------------|--------------|
| `partnerId`   | `String`                  | partner identifier|
| `data`        | `cocos2d::ValueMap`       | data to add|

```cpp 
  ValueMap data;
    data["thePartnerId"] = "abcd";
    data["Item_id"] = 1;
    data["isLegacy"] = false;
    AppsFlyerX::setPartnerData("partnerID", data);
```

---

##### <a id="customDomains"> **`setOneLinkCustomDomain(domains);`**
 Use to get conversion data with Branded links. 

| parameter   | type                        | description |
| ----------- |-----------------------------|--------------|
| `domains` | `std::vector<std::string>`  | list of custom domains


*Example:*

```cpp
  std::vector<std::string> domains;
  domains.push_back("test.domain.com");
  domains.push_back("myDomain.com");
  AppsFlyerX::setOneLinkCustomDomain(domains);
```

---

##### <a id="currentLang"> **`setCurrentDeviceLanguage(language);`**
 Use to get conversion data with Branded links. 

| parameter   | type                        | description |
| ----------- |-----------------------------|--------------|
| `language` | `std::string`  | Language to set


*Example:*

```cpp
  AppsFlyerX::setCurrentDeviceLanguage("English");
```

---

##### <a id="SharingFilterForPartners"> **`setSharingFilterForPartners(partners);`**
Used by advertisers to set some (one or more) networks/integrated partners to exclude from getting data.

| parameter   | type                        | description |
| ----------- |-----------------------------|--------------|
| `partners` | `std::vector<std::string>`  | partners to exclude from getting data


*Example:*

```cpp
  std::vector<std::string> partners;
  partners.push_back("partners_1");
  AppsFlyerX::setSharingFilterForPartners(partners); // Single partner

  std::vector<std::string> partners;
  partners.push_back("partners_1");
  partners.push_back("partners_2");
  AppsFlyerX::setSharingFilterForPartners(partners); // Multiple partner

  std::vector<std::string> partners;
  partners.push_back("all");
  AppsFlyerX::setSharingFilterForPartners(partners); // All partners

  std::vector<std::string> partners;
  partners.push_back("");
  AppsFlyerX::setSharingFilterForPartners(partners); // Reset list (default)

  std::vector<std::string> partners;
  AppsFlyerX::setSharingFilterForPartners(partners); // Reset list (default)

```

---

##### <a id="SharingFilterForPartners"> **`setSharingFilterForPartners(partners);`**
Used by advertisers to set some (one or more) networks/integrated partners to exclude from getting data.

| parameter   | type                        | description |
| ----------- |-----------------------------|--------------|
| `partners` | `std::vector<std::string>`  | partners to exclude from getting data


*Example:*

```cpp

```

---



##### <a id="disableNetworkID"> **`setDisableNetworkData(disable);`**
Use to opt-out of collecting the network operator name (carrier) and sim operator name from the device.

| parameter   | type                        | description |
| ----------- |-----------------------------|--------------|
| `disable` | `bool`  | Defaults to false


*Example:*

```cpp
  AppsFlyerX::setDisableNetworkData(true);
```

---
 
 ## **Migration Guide to v6**
[Integration guide](https://support.appsflyer.com//hc/en-us/articles/207032066#introduction)
[Migration guide](https://support.appsflyer.com/hc/en-us/articles/360011571778)
In v6 of AppsFlyer SDK there are some api breaking changes: 

|Before v6   | v6  |
|---|---|
| trackAppLaunch | start|
| trackEvent  | logEvent  |
| trackLocation | logLocation|
| stopTracking  | stop  |
| validateAndTrackInAppPurchase  | validateAndLogInAppPurchase  |
| setDeviceTrackingDisabled | anonymizeUser|

### iOS
on iOS you need to implement IDFA request pop up and add AppTrackTransparency framework in order for the plugin to work


## <a id="dma_support"> Send consent for DMA compliance 

**SDK 7:** Pass **`SetConsentDataParams`** to **`AppsFlyerX::setConsentData`** (returns `AppsFlyerTask`). The **`AppsFlyerXConsent`** class was removed — see [`doc/migration-guide.md`](doc/migration-guide.md).

For a general introduction to DMA consent data, see [here](https://dev.appsflyer.com/hc/docs/send-consent-for-dma-compliance).<be> 
The SDK offers two alternative methods for gathering consent data:<br> 
- **Through a Consent Management Platform (CMP)**: If the app uses a CMP that complies with the [Transparency and Consent Framework (TCF) v2.2 protocol](https://iabeurope.eu/tcf-supporting-resources/), the SDK can automatically retrieve the consent details.<br> 
<br>OR<br><br> 
- **Through a dedicated SDK API**: Developers can pass Google's required consent data directly to the SDK using a specific API designed for this purpose. 
### Use CMP to collect consent data 
A CMP compatible with TCF v2.2 collects DMA consent data and stores it in <code>SharedPreferences</code>(Android) or <code>NSUserDefaults</code>(iOS). To enable the SDK to access this data and include it with every event, follow these steps:<br> 
<ol> 
  <li> Call <code>AppsFlyerX::enableTCFDataCollection(true)</code> to instruct the SDK to collect the TCF data from the device. 
  <li> Set the the adapter to be manual(see (#manual-start)[manual mode]). <br> This will allow us to delay the Conversion call to provide the SDK with the user consent. 
  <li> Use the CMP to decide if you need the consent dialog in the current session.
  <li> If needed, show the consent dialog, using the CMP, to capture the user consent decision. Otherwise, go to step 6. 
  <li> Get confirmation from the CMP that the user has made their consent decision, and the data is available in <code>SharedPreferences</code> or <code>NSUserDefaults</code>.
  <li> Call start the following way:
   
```
 #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
     AppsFlyerX::setManualStart(false);
 #endif
 AppsFlyerX::start();
```
</ol> 
 
 #### AppDelegate
``` cpp
bool AppDelegate::applicationDidFinishLaunching() {

    AppsFlyerX::stop(false);
    AppsFlyerX::enableTCFDataCollection(true);
    AppsFlyerX::setIsDebug(true);
    AppsFlyerX::setAppsFlyerDevKey("devkey");

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    // In case you want to use manual mode. 
    AppsFlyerX::setManualStart(true);
    // 
    AppsFlyerX::setAppleAppID("appleAppId");
``` 
#### Scene class
- after getting CMP results
```cpp
 #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
     AppsFlyerX::setManualStart(false);
 #endif
 AppsFlyerX::start();
```

 
### Manually collect consent data 
If your app does not use a CMP compatible with TCF v2.2, use the SDK API detailed below to provide the consent data directly to the SDK. 
<ol> 
  <li> Initialize <code>AppsFlyerX</code> using manual mode. This will allow us to delay the Conversion call in order to provide the SDK with the user consent. 
  <li> Determine whether the GDPR applies or not to the user.<br> 
  - If GDPR applies to the user, perform the following:  
      <ol> 
        <li> Given that GDPR is applicable to the user, determine whether the consent data is already stored for this session. 
            <ol> 
              <li> If there is no consent data stored, show the consent dialog to capture the user consent decision. 
              <li> If there is consent data stored continue to the next step. 
            </ol> 
        <li> Set consent on the SDK (SDK 7 example):<br>
<pre>
AppsFlyerX::SetConsentDataParams consent;
consent.isUserSubjectToGDPR = true;
consent.hasConsentForDataUsage = true;
consent.hasConsentForAdsPersonalization = true;
AppsFlyerX::setConsentData(consent);
</pre>
        <li> Call start:
<code>
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  AppsFlyerX::setManualStart(false);
#endif
AppsFlyerX::start();
</code>
      </ol><br> 
    - If GDPR doesn’t apply to the user perform the following: 
      <ol> 
        <li> Set non-GDPR consent (SDK 7): <code>SetConsentDataParams</code> with <code>isUserSubjectToGDPR = false</code>, then <code>setConsentData</code>.
        <li> Call start:
<code>
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  AppsFlyerX::setManualStart(false);
#endif
AppsFlyerX::start();
</code>
      </ol> 
</ol> 
