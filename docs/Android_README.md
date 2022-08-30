<img src="https://www.appsflyer.com/wp-content/uploads/2016/11/logo-1.svg"  width="200">

# Cocos2dX Android Integration Guide


##  Integration:
How to integrate the AppsFlyer SDK into your Cocos2d-x Android project.

### 1. Add the SDK to your project

Throughout this guide, we assume that you are using `Android Studio` for development.

Take the files from the `AppsFlyer` [folder](/Classes/AppsFlyer) and add them to your Android project under `Classes` folder.

![add-to-android-mk](https://github.com/AppsFlyerSDK/AppsFlyerCocos2dX/blob/development/Resources/add-android-files.png?raw=true)

### 2.  Add the C++ source file definitions
Add the paths of the AppsFlyer C++ files to the `LOCAL_SRC_FILES` section in your `Android.mk` file if you use `PROP_BUILD_TYPE=ndk-build`
```mk
../../../Classes/AppsFlyer/AppsFlyerXAndroid.cpp \
../../../Classes/AppsFlyer/AppsFlyerProxyX.cpp \
../../../Classes/AppsFlyer/AppsFlyerX.cpp
```

![add-android-files](https://github.com/AppsFlyerSDK/AppsFlyerCocos2dX/blob/development/Resources/add-to-android-mk.png?raw=true)

For `cmake` a.e. `PROP_BUILD_TYPE=cmake` use:

For example in `CMakeLists.txt`:

```mk
# add cross-platforms source files and header files 
list(APPEND GAME_SOURCE
     Classes/AppsFlyer/AppsFlyerX.cpp
     Classes/AppsFlyer/AppsFlyerXAndroid.cpp
     Classes/AppsFlyer/AppsFlyerProxyX.cpp
     Classes/AppDelegate.cpp
     Classes/HelloWorldScene.cpp
     )
list(APPEND GAME_HEADER
     Classes/AppsFlyer/AppsFlyerX.h
     Classes/AppsFlyer/AppsFlyerXAndroid.h
     Classes/AppsFlyer/AppsFlyerProxyX.h
     Classes/AppDelegate.h
     Classes/HelloWorldScene.h
     )
```

![add-android-files](https://github.com/AppsFlyerSDK/AppsFlyerCocos2dX/blob/development/Resources/add-to-android-cmake.png?raw=true)



### 3. Add the AppsFlyer library to your project
We prefer to use `gradle` - as the easiest way to stay up to date. Please add the following lines to your gradle app dependencies:

```
implementation 'com.appsflyer:af-android-sdk:6.8.2'
```

For Google Install Referrer, please, add also:

```
implementation 'com.android.installreferrer:installreferrer:2.1'
implementation 'com.android.support:appcompat-v7:28.0.0'
``` 


As alternative way, take the latest [`AF-Android-SDK.jar`](https://s3-eu-west-1.amazonaws.com/download.appsflyer.com/Android/AF-Android-SDK.jar) library and copy it to your project's `libs` folder.

### 4. Add permissions

In the Package Explorer, open the `AndroidManifest.xml` file of your Android project. Add the `uses-permission` tag for `INTERNET` if it's not already present.

```xml
<uses-permission android:name="android.permission.INTERNET" />
```

### The AD_ID permission for android apps

In v6.8.0 of the AppsFlyer SDK, we added the normal permission com.google.android.gms.permission.AD_ID to the SDK's AndroidManifest, 
to allow the SDK to collect the Android Advertising ID on apps targeting API 33.
If your app is targeting children, you may need to revoke this permission to comply with Google's Data policy.
You can read more about it [here](https://support.appsflyer.com/hc/en-us/articles/7569900844689).