<img src="https://massets.appsflyer.com/wp-content/uploads/2018/06/20092440/static-ziv_1TP.png"  width="400" > 

# Cocos2dX AppsFlyer plugin for Android and iOS.

----------


In order for us to provide optimal support, we would kindly ask you to submit any issues to support@appsflyer.com

*When submitting an issue please specify your AppsFlyer sign-up (account) email , your app ID , production steps, logs, code snippets and any additional relevant information.*

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

- Android AppsFlyer SDK **v6.17.5** 
- iOS AppsFlyer SDK **v6.17.8**


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

##### <a id="validate_and_log_2.0"> **`Validate and log 2.0 API`**

| Property      | Type                           | Description                                                       | Platform     |
| ------------- | ------------------------------ | ----------------------------------------------------------------- | ------------ |
| purchaseType  | `AFXPurchaseType`              | Purchase type of the event. For Apple implementation, use `APPLE`  | Android only |
| purchaseToken | `std::string`                  | Field required to validate the purchase                           | Android only |
| transactionId | `std::string`                  | Transaction ID of the Apple purchase                              | iOS only     |
| productId     | `std::string`                  | ID of the purchased product                                       | Android/iOS  |
| price         | `std::string`                  | Price of the product                                              | Android/iOS  |
| currency      | `std::string`                  | Currency according to the ISO format (ISO 4217)                   | Android/iOS  |

*Description:*
The `validateAndLogInAppPurchase` method is part of the receipt validation flow, which enables your app to validate in-app purchase events generated by Google Play and Apple.


`AFSDXPurchaseDetails` encapsulates all purchase necessary data for Android and iOS. 
* `AFXPurchaseType` is used for Android only, so for iOS, be sure to use `APPLE` option, which is empty string and will be omitted by the implementation. 
* `purchaseToken` is the Android propperty, for Apple impl, please use empty string;
* The `transactionId`is used by Apple implementation, Android omits this propperty;

*Example:*
```cpp
ValueMap params; // Additional parameters you want to pass to the API
AFSDKXPurchaseDetails details = AFSDKXPurchaseDetails(AFXPurchaseType::SUBSCRIPTION, "", "customId", "6.99", "USD", "transactionId");

AppsFlyerX::validateAndLogInAppPurchase(details, params, [](const AFSDKXValidateAndLogResult& result) { 
  // Your implementation of the completion/callback block;
});
```

**Note:** For Android SDK, the callback block does not return anything for now, it will be supported in the next update; 

---


##### <a id="logAdrevenue"> **`Log AdRevenue API`**

*Description:*

When an impression with revenue occurs, invoke the logAdRevenue method with the revenue details of the impression.

`AFXAdRevenueData` object encapsulates all data related to the AdRevenue event: 

| Property                | Type                                        | Description                                       |
| ----------------------- | ------------------------------------------- | ------------------------------------------------- |
| monetizationNetwork      | `std::string`                               | The name of the monetization network               |
| mediationNetwork         | `AppsFlyerXAdRevenueMediationNetworkType`   | The type of mediation network being used           |
| currencyIso4217Code      | `std::string`                               | Currency code according to the ISO format (ISO 4217) |
| eventRevenue            | `double`                                    | Revenue generated from the ad event                |



*Example:*
```cpp
ValueMap params; // Additional parameters you want to pass to the API
AFXAdRevenueData data = AFXAdRevenueData("someVal", AppsFlyerXAdRevenueMediationNetworkType::ApplovinMax, "USD", 7.99);

AppsFlyerX::logAdRevenue(data, params);
```


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

##### <a id="setUserEmails"> **`setUserEmails(userEmails, type): void`** (optional)

AppsFlyer enables you to report one or more of the device’s associated email addresses. You must collect the email addresses and report it to AppsFlyer according to your required encryption method.
The following encryption methods are available: SHA1, MD5, SHA256 and plain.


| parameter   | type                  | Default     | description |
| ----------- |-----------------------|-------------|-------------|
| `userEmails`| `std::vector<std::string>`              |   ---     |  list of emails |
| `type`| `EmailCryptTypeX`              |   ---     |  [crypt type](https://github.com/AppsFlyerSDK/AppsFlyerCocos2dX/blob/dev/RD-8680/Android-docs/Classes/AppsFlyer/EmailCryptTypeX.h): None, sha1, sha256, md5 |

*Examples:*

```cpp
AppsFlyerX::setUserEmails({"kinzer.appsf@gmail.com"}, XEmailCryptTypeSHA256);
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

##### <a id="validateAndLogInAppPurchase"> **`validateAndTrackInAppPurchase`**

##### <a id="validateAndLogInAppPurchase-a)"> **Android**


| parameter   | type                  | Default     | description |
| ----------- |-----------------------|-------------|-------------|
| `publicKey`| `const std::string&`              |        |  |
| `signature`| `const std::string&`              |        |  |
| `purchaseData`| `const std::string&`              |       |  |
| `price`| `const std::string&`              |        |  |
| `currency`| `const std::string&`              |        |  |
| `additionalParameters`| `cocos2d::ValueMap`              |      |  |

*Examples:*

```cpp
ValueMap test_map;
test_map["key1"] = "value1";
std::string base64EncodedPublicKey = "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA3dkBTr2pD2YSqSK2ewlEWwH9Llu0iA4PkwgVOyNRxOsHfrOlqi0Cm51qdNS0aqh/SMZkuQTAroqH3pAr9gVFOiejKRw+ymTaL5wB9+5n1mAbdeO2tv2FDsbawDvp7u6fIBejYt7Dtmih+kcu707fEO58HZWqgzx9qSHmrkMZr6yvHCtAhdBLwSBBjyhPHy7RAwKA+PE+HYVV2UNb5urqIZ9eI1dAv3RHX/xxHVHRJcjnTyMAqBmfFM+o31tp8/1CxGIazVN8HpVk8Qi2uqSS5HdKUu6VnIK8VuAHQbXQn4bG6GXx5Tp0SX1fKrejo7hupNUCgOlqsYHFYxsRkEOi0QIDAQAB";
std::string signature = "OaIdwQOcmrJrMKUx+URVy1I6aeKYiYzflkk1zIKVSs+dDv691neCbR+jlDDzVi3jfSkfirxQISxo7Pe1uzoYbpq9wBk/pMgVjjSbpvCojhA4d/Mwsf4mtAH2LJcVNjhMQdSWvGJlzva3OSt+KQ+9/pRJ15aYT2gFn3SpGSPxNxJmHPIOlM1Lr74MejVu9rnbcSjCB/oI0W4O58p9UWSt5MgmlpqlrK5YqTi1a1VnttY9r1IXFeltwZvmPbcWcYwRHFvemwYGX86huSOFBOYRfaYo9f+DinpoUoXKQEo0JrvKD2/dzFkbUTto1d2OPo1ddaYllgsb2UEV5wwFZFnemg==";
std::string purchaseData = "{\"orderId\":\"\",\"packageName\":\"com.appsflyer.testapp\",\"productId\":\"consumable\",\"purchaseTime\":1497531638107,\"purchaseState\":0,\"developerPayload\":\"2497525891514-5765886608164763986\",\"purchaseToken\":\"pfkalmpnnimamdllmincaida.AO-J1OymunlPCkDQZTf8bPcesoB0n1_ND3WFynoU91-v_R1Px46m3Q-DdRKNlxMVsP2pCTKpo1et1w1IpNVXQ8-zNpRo6a2nXP7a5fQWiDv2asL1dwJPCV8NghDHbstO084IlKo6xcgy\"}";
    
AppsFlyerX::validateAndLogInAppPurchase(base64EncodedPublicKey, 
                                          signature, 
                        "3.00",
                      "ILS",
                      test_map);
```

---


##### <a id="validateAndLogInAppPurchase-i)"> **iOS**

| parameter   | type                  | Default     | description |
| ----------- |-----------------------|-------------|-------------|
| `productIdentifier`| `const std::string&`              |        |  |
| `price`| `const std::string&`              |        |  |
| `currency`| `const std::string&`              |       |  |
| `tranactionId`| `const std::string&`              |        |  |
| `params`| `cocos2d::ValueMap`              |        |  |
| `successBlock`| `std::function<void(cocos2d::ValueMap)>`              |      |  |
| `failureBlock`| `std::function<void(cocos2d::ValueMap)>`              |      |  |

*Examples:*

```cpp
ValueMap test_map;
test_map["key1"] = "value1";
std::string productIdentifier = "com.mycomp.inapppurchase.cons";
std::string tr_id = "1000000256672208";
  
AppsFlyerX::validateAndLogInAppPurchase(productIdentifier, 
                                          "1.99",
                                          "USD",
                                          tr_id
                      test_map, 
                      successBlock,
                      failureBlock);
```

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
 
 As alternative way use directly native API:
Open your Xcode project and locate the file `AppController.mm` under the iOS folder inside your project. Add the following code snippet under `didFinishLaunchingWithOptions` :

```cpp 
#import "AppsFlyerLib.h"

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
//...
UIUserNotificationType userNotificationTypes = (UIUserNotificationTypeAlert |
                                                    UIUserNotificationTypeBadge |
                                                    UIUserNotificationTypeSound);
    UIUserNotificationSettings *settings = [UIUserNotificationSettings settingsForTypes:userNotificationTypes
                                                                             categories:nil];
    [application registerUserNotificationSettings:settings];
    [application registerForRemoteNotifications];
//..
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {    
    [[AppsFlyerLib shared] registerUninstall:deviceToken];
}
```
and method implementation:

```cpp 
- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {    
    [[AppsFlyerLib shared] registerUninstall:deviceToken];
}
```



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
#import "AppsFlyerLib.h"

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
//...
 AppsFlyerX::setDidResolveDeepLink(didResolveDeepLink);
//..
}
static void didResolveDeepLink(AppsFlyerXDeepLinkResult result){
    CCLOG("%s", "AppDelegate.cpp got ddl!");
    std::string ddl = "Deep link data is \n";
    switch (result.status) {
    case NOTFOUND:
            CCLOG("deep link not found");
            break;
        case FOUND:
            if (!result.deepLink.empty()){
                if (!result.getMediaSource().empty()) {
                    CCLOG("Media source is %s", result.getMediaSource().c_str());
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
        <li> To transfer the consent data to the SDK create an object called <code>AppsFlyerXConsent</code> using the <code>forGDPRUser()</code> method with the following parameters:<br> 
          - <code>hasConsentForDataUsage</code> - Indicates whether the user has consented to use their data for advertising purposes.<br>
          - <code>hasConsentForAdsPersonalization</code> - Indicates whether the user has consented to use their data for personalized advertising purposes.
        <li> Call <code>AppsFlyerX::setConsentData()</code> with the <code>AppsFlyerXConsent</code> object.    
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
        <li> Create an <code>AppsFlyerXConsent</code> object using the <code>forNonGDPRUser()</code> method. This method doesn’t accept any parameters.
        <li> Call <code>AppsFlyerX::setConsentData()</code> with the <code>AppsFlyerXConsent</code> object.  
        <li> Call start:
<code>
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  AppsFlyerX::setManualStart(false);
#endif
AppsFlyerX::start();
</code>
      </ol> 
</ol> 
