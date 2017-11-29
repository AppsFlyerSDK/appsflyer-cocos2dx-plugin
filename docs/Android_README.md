<img src="https://www.appsflyer.com/wp-content/uploads/2016/11/logo-1.svg"  width="200">

# Cocos2dX Android Integration Guide


## Table of content

- [integration](#integration)

- [Usage](#usage)
- [API methods](#api-methods)
 - [setIsDebug](#setIsDebug)
 - [trackEvent](#trackEvent)




## <a id="integration"> Integration:
How to integrate the AppsFlyer SDK into your Cocos2d-x Android project.

###1.  Add the SDK to your project
Throughout this guide, we assume that you are using `Android Studio` for development.

Take the files from the `AppsFlyer` [folder](/Classes/AppsFlyer) and add them to your Android project under `Classes` folder.

![add-to-android-mk](https://github.com/AppsFlyerSDK/AppsFlyerCocos2dX/blob/development/Resources/add-android-files.png?raw=true)

###2.  Add the C++ source file definitions
Add the paths of the AppsFlyer C++ files to the `LOCAL_SRC_FILES` section in your `Android.mk` file.
```mk
../../../Classes/AppsFlyer/AppsFlyerXAndroid.cpp \
../../../Classes/AppsFlyer/AppsFlyerProxyX.cpp \
../../../Classes/AppsFlyer/AppsFlyerX.cpp
```

![add-android-files](https://github.com/AppsFlyerSDK/AppsFlyerCocos2dX/blob/development/Resources/add-to-android-mk.png?raw=true)

###3. Add the AppsFlyer library to your project

Take the latest `appsflyer-SDK.jar` library and copy it to your project's `libs` folder.

###4. Add permissions

In the Package Explorer, open the `AndroidManifest.xml` file of your Android project. Add the `uses-permission` tag for `INTERNET` if it's not already present.

```xml
<uses-permission android:name="android.permission.INTERNET" />
```


## <a id="usage"> Usage:

#### 1\. Set your  Dev_Key and enable AppsFlyer to detect installations, sessions (app opens) and updates.
> This is the minimum requirement to start tracking your app installs and is already implemented in this plugin. You **MUST** modify this call and provide:
**-devKey** - Your application devKey provided by AppsFlyer.




Add the following lines into `applicationDidFinishLaunching` and to `applicationWillEnterForeground` method to be able to initialize tracking with your own AppsFlyer dev key:


```cpp
#include "AppsFlyer/AppsFlyerX.h"

bool AppDelegate::applicationDidFinishLaunching() {
  AppsFlyerX::setAppsFlyerDevKey("YOUR_DEV_KEY");
  
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AppsFlyerX::trackAppLaunch();
#endif
}

void AppDelegate::applicationWillEnterForeground() {
   //..
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AppsFlyerX::trackAppLaunch();
#endif
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
#####  **`trackEvent(eventName, eventValue): void`** (optional)
and
##### <a id="trackEvent"> **`trackEvent(eventName, eventValues): void`** (optional)


- These in-app events help you track how loyal users discover your app, and attribute them to specific
campaigns/media-sources. Please take the time define the event/s you want to measure to allow you
to track ROI (Return on Investment) and LTV (Lifetime Value).
- The `trackEvent` method allows you to send in-app events to AppsFlyer analytics. This method allows you to add events dynamically by adding them directly to the application code.


| parameter   | type                        | description |
| ----------- |-----------------------------|--------------|
| `eventName` | `string`                    | custom event name, is presented in your dashboard.  See the Event list [HERE](https://github.com/AppsFlyerSDK/AppsFlyerCocos2dX/blob/dev/RD-8680/Android-docs/Classes/AppsFlyer/AppsFlyerXMacro.h)  |
| `eventValues` | `cocos2d::ValueMap`                    | event details |

*Example:*

```cpp
AppsFlyerX::trackEvent(AFEventPurchase, {{ "key1", cocos2d::Value("value1")},
                                         { "key2", cocos2d::Value("value2")}});
```
---

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

##### <a id="setCurrencyCode"> **`setCurrencyCode(currencyId): void`**


| parameter   | type                  | Default     | description |
| ----------- |-----------------------|-------------|-------------|
| `currencyId`| `String`              |   `USD`     |  [ISO 4217 Currency Codes](http://www.xe.com/iso4217.php)           |

*Examples:*

```javascript
window.plugins.appsFlyer.setCurrencyCode("USD");
window.plugins.appsFlyer.setCurrencyCode("GBP"); // British Pound
```

---

##### <a id="setAppUserId"> **`setAppUserId(customerUserId): void`**


Setting your own Custom ID enables you to cross-reference your own unique ID with AppsFlyer’s user ID and the other devices’ IDs. This ID is available in AppsFlyer CSV reports along with postbacks APIs for cross-referencing with you internal IDs.

**Note:** The ID must be set during the first launch of the app at the SDK initialization. The best practice is to call this API during the `deviceready` event, where possible.


| parameter   | type                        | description |
| ----------- |-----------------------------|--------------|
| `customerUserId`   | `String`                      | |

*Example:*

```javascript
window.plugins.appsFlyer.setAppUserId(userId);
```
---




##### <a id="enableUninstallTracking"> **`enableUninstallTracking(token, onSuccess, onError): void`**

Enables app uninstall tracking.
<a href="https://support.appsflyer.com/hc/en-us/articles/211211963-iOS-Uninstall-Tracking">More Information</a>

| parameter   | type                        | description |
| ----------- |-----------------------------|--------------|
| `FCM/GCM ProjectNumber`   | `String`    | GCM/FCM ProjectNumber |
| `onSuccess` | `(message: string)=>void` | Success callback - called after successfull register uninstall. (optional)|
| `onError`   | `(message: string)=>void` | Error callback - called when error occurs during register uninstall. (optional)|


---

##### <a id="setGCMProjectID"> **`setGCMProjectID(GCMProjectNumber): void`** *Deprecated*

AppsFlyer requires a Google Project Number to enable uninstall tracking.
<a href="https://support.appsflyer.com/hc/en-us/articles/208004986-Android-Uninstall-Tracking">More Information</a>


| parameter   | type                        | description |
| ----------- |-----------------------------|--------------|
| `GCMProjectNumber`   | `String`           | GCM ProjectNumber |


---

##### <a id="updateServerUninstallToken"> **`updateServerUninstallToken("token"): void`**

Allows to pass GCM/FCM Tokens that where collected by third party plugins to the AppsFlyer server.
Can be used for Uninstall Tracking.


| parameter   | type                        | description |
| ----------- |-----------------------------|--------------|
| `token`   | `String`                      | GCM/FCM Token|


---

##### <a id="getAppsFlyerUID"> **`getAppsFlyerUID(successCB): void`**  (Advanced)

Get AppsFlyer’s proprietary Device ID. The AppsFlyer Device ID is the main ID used by AppsFlyer in Reports and APIs.

```javascript
function getUserIdCallbackFn(id){/* ... */}
window.plugins.appsFlyer.getAppsFlyerUID(getUserIdCallbackFn);
```
*Example:*

```javascript
var getUserIdCallbackFn = function(id) {
alert('received id is: ' + id);
}
window.plugins.appsFlyer.getAppsFlyerUID(getUserIdCallbackFn);
```

| parameter   | type                        | description |
| ----------- |-----------------------------|--------------|
| `getUserIdCallbackFn` | `() => void`                | Success callback |


---

##### <a id="setAppInviteOneLinkID"> **`setAppInviteOneLinkID(OneLinkID): void`**  (User Invite / Cross Promotion)

Set AppsFlyer’s OneLink ID. Setting a valid OneLink ID will result in shortened User Invite links, when one is generated. The OneLink ID can be obtained on the AppsFlyer Dashboard.

*Example:*
```javascript
window.plugins.appsFlyer.setAppInviteOneLinkID("Ab1C");
```

| parameter   | type                        | description |
| ----------- |-----------------------------|--------------|
| `OneLinkID` | `String`                    | OneLink ID |


---

##### <a id="generateInviteLink"> **`generateInviteLink(options, onSuccess, onError): void`**  (User Invite)

Allowing your existing users to invite their friends and contacts as new users to your app can be a key growth factor for your app. AppsFlyer allows you to track and attribute new installs originating from user invites within your app.

*Example:*
```javascript
var inviteOptions {
channel: "gmail",
campaign: "myCampaign",
customerID: "1234",

userParams {
myParam : "newUser",
anotherParam : "fromWeb",
amount : 1
}
};

var onInviteLinkSuccess = function(link) {
console.log(link); // Handle Generated Link Here
}

function onInviteLinkError(err) {
console.log(err);
}

window.plugins.appsFlyer.generateInviteLink(inviteOptions, onInviteLinkSuccess, onInviteLinkError);
```

| parameter   | type                        | description |
| ----------- |-----------------------------|--------------|
| `inviteOptions` | `Object`                    |Parameters for Invite link  |
| `onInviteLinkSuccess` | `() => void`                | Success callback (generated link) |
| `onInviteLinkError` | `() => void`                | Error callback |

A complete list of supported parameters is available <a href="https://support.appsflyer.com/hc/en-us/articles/115004480866-User-Invite-Tracking">here</a>.
Custom parameters can be passed using a `userParams{}` nested object, as in the example above.

---

##### <a id="trackCrossPromotionImpression"> **`trackCrossPromotionImpression("appID", "campaign"): void`**  (Cross Promotion)

Use this call to track an impression use the following API call. Make sure to use the promoted App ID as it appears within the AppsFlyer dashboard.

*Example:*
```javascript
window.plugins.appsFlyer.trackCrossPromotionImpression("com.myandroid.app", "myCampaign");
```

| parameter   | type                        | description |
| ----------- |-----------------------------|--------------|
| `appID` | `String`                    | Promoted Application ID |
| `campaign` | `String`                    | Promoted Campaign |

For more details about Cross-Promotion tracking please see <a href="https://support.appsflyer.com/hc/en-us/articles/115004481946-Cross-Promotion-Tracking">here</a>.

---

##### <a id="trackAndOpenStore"> **`trackAndOpenStore("appID","campaign", options): void`**  (Cross Promotion)

Use this call to track the click and launch the app store's app page (via Browser)

*Example:*
```javascript
var crossPromOptions {
customerID: "1234",
myCustomParameter: "newUser"
};

window.plugins.appsFlyer.trackAndOpenStore("com.myandroid.app", "myCampaign", crossPromOptions);
```

| parameter   | type                        | description |
| ----------- |-----------------------------|--------------|
| `appID` | `String`                    | Promoted Application ID |
| `campaign` | `String`                    | Promoted Campaign |
| `options` | `Object`                    | Additional Parameters to track |

For more details about Cross-Promotion tracking please see <a href="https://support.appsflyer.com/hc/en-us/articles/115004481946-Cross-Promotion-Tracking">here</a>.

---

### <a id="deep-linking-tracking"> Deep linking Tracking

#### <a id="dl-android"> Android
In ver. >4.2.5 deeplinking metadata (scheme/host) is sent automatically

#### <a id="dl-ios"> iOS URL Types
Add the following lines to your code to be able to track deeplinks with AppsFlyer attribution data:

for pure Cordova - add a function 'handleOpenUrl' to your root, and call our SDK as shown:
```javascript
window.plugins.appsFlyer.handleOpenUrl(url);
```
It appears as follows:

```javascript
var handleOpenURL = function(url) {
window.plugins.appsFlyer.handleOpenUrl(url);
}
```
