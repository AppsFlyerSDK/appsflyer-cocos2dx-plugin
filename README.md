<img src="https://www.appsflyer.com/wp-content/uploads/2016/11/logo-1.svg"  width="200">

# Cocos2dX AppsFlyer plugin for Android and iOS.

[![GitHub tag](https://img.shields.io/github/v/release/AppsFlyerSDK/appsflyer-unity-plugin)](https://img.shields.io/github/v/release/AppsFlyerSDK/appsflyer-unity-plugin)
----------


In order for us to provide optimal support, we would kindly ask you to submit any issues to support@appsflyer.com

*When submitting an issue please specify your AppsFlyer sign-up (account) email , your app ID , production steps, logs, code snippets and any additional relevant information.*

----------

## Table of content

- [integration](#integration)
- [Usage](#usage)
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


### <a id="plugin-build-for"> This plugin is built for

- Android AppsFlyer SDK **v6.4.0** 
- iOS AppsFlyer SDK **v6.4.0**


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


##<a id="api-methods"> API Methods


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

*Example:*

```cpp
AppsFlyerX::sharingFilterForAllPartners();
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
