<img src="https://www.appsflyer.com/wp-content/uploads/2016/11/logo-1.svg"  width="200">

# Cocos2dX Android integration

Native AppsFlyer Android SDKs are **not** vendored in this repo. You add them with **Gradle** (Maven BOM + RPC bridge AAR) and ship the plugin’s **C++ bridge** plus a small **Kotlin RPC adapter** in your app module.

C++ API and examples: [API.md](API.md). Init order and threading: [SDK7-Lifecycle.md](SDK7-Lifecycle.md).

### Prerequisites

| Requirement | Sample in this repo |
| --- | --- |
| **Android Studio** | Ladybug or newer recommended |
| **AGP** | `8.13.1` (`proj.android-studio/build.gradle`) |
| **Gradle + JDK** | Gradle wrapper in `proj.android-studio/`; **JDK 17** |
| **Kotlin** | `2.0.21` (Gradle plugin) |
| **`compileSdk`** | Set to a current API level your toolchain supports (sample uses `33`) |
| **`minSdkVersion`** | **≥ API 21** — AppsFlyer Android SDK minimum ([`version/android-sdk-versions.json`](../version/android-sdk-versions.json)); your game may target higher |
| **NDK** | Match your Cocos2d-x template (sample uses `27.0.12077973`) |
| **Cocos2d-x** | Android game with `libcocos2dx` (or equivalent) + ndk-build **or** CMake native build |

### At a glance

1. **Copy** [`Classes/AppsFlyer/`](../Classes/AppsFlyer/) (include `generated/`) into your game.
2. **Copy** [`AppsFlyerCocosRpcBridge.kt`](../proj.android-studio/app/src/main/kotlin/com/appsflyer/cocos2dx/rpc/AppsFlyerCocosRpcBridge.kt) into the **app** module (keep package `com.appsflyer.cocos2dx.rpc`).
3. **Compile** RPC C++ sources in `Android.mk` or `CMakeLists.txt` ([Native build](#native-build-ndk-build-or-cmake)).
4. **Gradle:** BOM + `af-android-sdk` (no version) + RPC bridge + Install Referrer ([Gradle dependencies](#gradle-dependencies)).
5. **`AppActivity`:** call `AppsFlyerCocosRpcBridge.onActivityCreated` **before** `super.onCreate()` ([Native entry hook](#native-entry-hook)).
6. **C++:** listener → `init` → `start` in `AppDelegate.cpp` ([C++ startup](#c-startup-sdk-7)).

| More detail | Document |
| --- | --- |
| Migrating from 6.x → 7 (packaging & API) | [Migration-Guide.md](Migration-Guide.md) |
| Init order & threading | [SDK7-Lifecycle.md](SDK7-Lifecycle.md) |
| Method reference | [API.md](API.md) |
| Version pins | [`version/android-sdk-versions.json`](../version/android-sdk-versions.json) (BOM, bridge, **minimum API 21**) |

**On this page:** [Checklist](#integration-checklist) · [Two entry files](#cocos2d-x-on-android-two-entry-files) · [Copy manifest](#copy-manifest) · [Native build](#native-build-ndk-build-or-cmake) · [Gradle](#gradle-dependencies) · [Activity hook](#native-entry-hook) · [Manifest & deep links](#androidmanifest-permissions-and-deep-links) · [Privacy](#privacy-ad_id-and-proguard) · [Build sample](#build-this-repos-sample) · [Troubleshooting](#troubleshooting)

---

## Integration checklist

- [ ] **1. Copy** [`Classes/AppsFlyer/`](../Classes/AppsFlyer/) → `<game>/Classes/AppsFlyer/` (all files, including `generated/`).
- [ ] **2. Copy** [`AppsFlyerCocosRpcBridge.kt`](../proj.android-studio/app/src/main/kotlin/com/appsflyer/cocos2dx/rpc/AppsFlyerCocosRpcBridge.kt) → `app/src/main/kotlin/com/appsflyer/cocos2dx/rpc/`.
- [ ] **3. Native sources** — add RPC `.cpp` files to ndk-build or CMake ([Native build](#native-build-ndk-build-or-cmake)).
- [ ] **4. Gradle** — BOM + bridge + Install Referrer ([Gradle dependencies](#gradle-dependencies)); enable Kotlin on the app module if needed.
- [ ] **5. `AppActivity`** (or your `Cocos2dxActivity` subclass) — `onActivityCreated`, **`onNewIntent`**, `onActivityDestroyed` ([hook](#native-entry-hook)).
- [ ] **6. `AndroidManifest.xml`** — `INTERNET`; deep-link intent filters if you use OneLink / App Links ([Manifest](#androidmanifest-permissions-and-deep-links)).
- [ ] **7. Release builds** — ProGuard keep rules if minify is on ([Privacy](#privacy-ad_id-and-proguard)).
- [ ] **8. `AppDelegate.cpp`** — SDK 7 chain ([C++ startup](#c-startup-sdk-7)); `onPause` in `applicationDidEnterBackground` ([API.md](API.md)).
- [ ] **9. Build & run** — verify [success criteria](#success-criteria).

### Success criteria

| # | Check | How |
| --- | --- | --- |
| 1 | Project compiles | No missing `AppsFlyerRpc.cpp` / `generated/*.cpp`; Kotlin bridge on classpath |
| 2 | Context ready | `onActivityCreated` runs before `super.onCreate()` |
| 3 | `init` succeeds | `AppsFlyerX::init(…).whenComplete` → `success == true` |
| 4 | Session ready | Session-ready callback fires; then `start` completes |
| 5 | Optional smoke | Logcat tag `AppsFlyer_Cocos2dX_RPC`; `enableDebug(true)` → AppsFlyer SDK logs; `logEvent` after `start` |

---

## Cocos2d-x on Android: two entry files

| File | Runs on | Your AppsFlyer responsibility |
| --- | --- | --- |
| **`AppActivity.java`** (or Kotlin `Cocos2dxActivity` subclass) | Android UI thread, **before** native `applicationDidFinishLaunching` | `AppsFlyerCocosRpcBridge.onActivityCreated(this)` **before** `super.onCreate()` — provides `Context` for `AppsFlyerRpcHandler` |
| **`AppDelegate.cpp`** | Cocos **game thread** after GL init | `registerDeepLinkListener` → `init` → listeners → `start` ([SDK7-Lifecycle](SDK7-Lifecycle.md)) |

If `onActivityCreated` is missing or runs after `super.onCreate()`, the first RPC calls may fail with **“RPC context not ready”**.

Outbound RPC from C++ uses JNI → `AppsFlyerCocosRpcBridge.executeRpcBlocking`. Inbound listener events use `nativeDeliverRpcEvent` → C++ `AppsFlyerRpc` on the game thread.

---

## Integration paths

| Case | Your setup | Follow |
| --- | --- | --- |
| **A** | Clone **this repo** and build the sample | [Build this repo’s sample](#build-this-repos-sample) |
| **B** | Classic Cocos2d-x `proj.android-studio/` in your game | [Copy manifest](#copy-manifest) + checklist |
| **C** | Cocos Creator / custom `runtime-src` layout | Same **files** as B; adjust paths in Gradle and native build scripts |

The Kotlin bridge is **not** published as a separate Maven artifact ([SDK7-Lifecycle — GAP-007](SDK7-Lifecycle.md#5-limitations)). Copy it into your **app** module.

---

## Copy manifest

| Source (this repo) | Destination (your game) |
| --- | --- |
| [`Classes/AppsFlyer/`](../Classes/AppsFlyer/) | `<game>/Classes/AppsFlyer/` |
| [`AppsFlyerCocosRpcBridge.kt`](../proj.android-studio/app/src/main/kotlin/com/appsflyer/cocos2dx/rpc/AppsFlyerCocosRpcBridge.kt) | `app/src/main/kotlin/com/appsflyer/cocos2dx/rpc/AppsFlyerCocosRpcBridge.kt` |
| Sample [`AppActivity.java`](../proj.android-studio/app/src/org/cocos2dx/cpp/AppActivity.java) | Your launcher `Cocos2dxActivity` (`onActivityCreated`, `onNewIntent`, `onActivityDestroyed`) |

**Compile** from `Classes/AppsFlyer/`:

- `AppsFlyerRpc.cpp`, `AppsFlyerTask.cpp`
- `generated/AppsFlyerRpcSchema.generated.cpp`
- Headers: `AppsFlyerX.h`, `AppsFlyerRpc.h`, `AppsFlyerTask*.h`, `AppsFlyerResult.h`, `AppsFlyerDeepLinkData.h`, `AppsFlyerXMacro.h`, `generated/*`

---

## Native build (ndk-build or CMake)

Add the RPC sources to the **game** shared library (the `.so` named in `android.app.lib_name` meta-data).

### ndk-build (`Android.mk`)

Reference: [`proj.android-studio/app/jni/Android.mk`](../proj.android-studio/app/jni/Android.mk).

```makefile
LOCAL_SRC_FILES := ... \
    $(LOCAL_PATH)/../../../Classes/AppsFlyer/AppsFlyerRpc.cpp \
    $(LOCAL_PATH)/../../../Classes/AppsFlyer/AppsFlyerTask.cpp \
    $(LOCAL_PATH)/../../../Classes/AppsFlyer/generated/AppsFlyerRpcSchema.generated.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../Classes
```

Set `PROP_BUILD_TYPE=ndk-build` in `gradle.properties` when using this path.

### CMake

Reference: root [`CMakeLists.txt`](../CMakeLists.txt) (`GAME_SOURCE` list).

```cmake
list(APPEND GAME_SOURCE
    Classes/AppsFlyer/AppsFlyerRpc.cpp
    Classes/AppsFlyer/AppsFlyerTask.cpp
    Classes/AppsFlyer/generated/AppsFlyerRpcSchema.generated.cpp
    # ... your game sources ...
)
```

Set `PROP_BUILD_TYPE=cmake` in `gradle.properties` (default in this repo’s sample).

Ensure `LOCAL_C_INCLUDES` / `target_include_directories` exposes `Classes/` so `#include "AppsFlyer/AppsFlyerX.h"` resolves.

---

## Gradle dependencies

Set `minSdkVersion` to at least **21** (or higher if your product requires it). In your **app** module `build.gradle` / `build.gradle.kts`, align dependency versions with [`version/android-sdk-versions.json`](../version/android-sdk-versions.json) and the sample [`proj.android-studio/app/build.gradle`](../proj.android-studio/app/build.gradle):

```gradle
dependencies {
    implementation platform('com.appsflyer:af-android-sdk-bom:7.0.4')
    implementation 'com.appsflyer:af-android-sdk'
    implementation 'com.appsflyer:af-android-plugin-bridge:7.0.13'
    implementation 'com.android.installreferrer:installreferrer:2.2'
}
```

Pin **BOM**, **RPC bridge**, and **Install Referrer** to the versions in [`version/android-sdk-versions.json`](../version/android-sdk-versions.json) (add `installReferrer` there when you bump). The **`af-android-sdk`** line has **no version** — the BOM selects it.

| Dependency | Version in Gradle |
| --- | --- |
| `com.appsflyer:af-android-sdk-bom` | Explicit (e.g. `7.0.4`) |
| `com.appsflyer:af-android-sdk` | **Omitted** — managed by BOM |
| `com.appsflyer:af-android-plugin-bridge` | Explicit (e.g. `7.0.13`) — `AppsFlyerRpcHandler` for `AppsFlyerCocosRpcBridge.kt` |
| `com.android.installreferrer:installreferrer` | Explicit (e.g. `2.2`) |

Do not add another `af-android-sdk` line with its own version; that overrides the BOM.

### Toolchain (sample)

Top-level [`proj.android-studio/build.gradle`](../proj.android-studio/build.gradle):

- Android Gradle Plugin **8.13.1**
- Kotlin Gradle plugin **2.0.21**
- `compileOptions` / `kotlinOptions` **Java 17**

Your game may use slightly different AGP versions; stay on AGP **8.9+**, Gradle **8.11+**, and JDK **17** if you hit Kotlin or dependency metadata issues.

### Optional: FCM / uninstall measurement

The sample under `proj.android-studio` does **not** ship Firebase or `google-services.json`. For uninstall tracking, add your own FCM setup (Firebase project, `google-services.json`, `firebase-messaging`, and `com.google.gms.google-services`) and wire uninstall APIs (`updateServerUninstallToken`, `FirebaseMessagingServiceListener` if you use AppsFlyer’s listener pattern). See [Android uninstall tracking](https://support.appsflyer.com/hc/en-us/articles/208004986-Android-Uninstall-Tracking).

---

## Native entry hook

In your `Cocos2dxActivity` subclass:

```java
import com.appsflyer.cocos2dx.rpc.AppsFlyerCocosRpcBridge;

@Override
protected void onCreate(Bundle savedInstanceState) {
    AppsFlyerCocosRpcBridge.onActivityCreated(this);
    super.onCreate(savedInstanceState);
}

@Override
protected void onNewIntent(Intent intent) {
    super.onNewIntent(intent);
    setIntent(intent); // see comment in sample AppActivity.java
}

@Override
protected void onDestroy() {
    AppsFlyerCocosRpcBridge.onActivityDestroyed();
    super.onDestroy();
}
```

Override **`onNewIntent`** and call **`setIntent(intent)`** when the launcher uses **`singleTask`** / **`singleTop`** so `getIntent()` reflects the latest link before the native SDK runs (sample includes a short comment; no `performDeepLinking` in this hook).

Full sample: [`proj.android-studio/app/src/org/cocos2dx/cpp/AppActivity.java`](../proj.android-studio/app/src/org/cocos2dx/cpp/AppActivity.java).

**Order matters:** `onActivityCreated` must run **before** `super.onCreate()` so context is available when Cocos loads the native library and `applicationDidFinishLaunching` fires RPCs.

---

## C++ startup (SDK 7)

Use the same listener → `init` → `start` chain as iOS. On Android, `InitParams` requires **`devKey`**; `appId` is not used on Android RPC init (safe to omit).

- Minimal pattern: [API.md — Quick start](API.md#quick-start)
- Threading and listener order: [SDK7-Lifecycle.md](SDK7-Lifecycle.md)
- Full sample: [`Classes/AppDelegate.cpp`](../Classes/AppDelegate.cpp)

**Lifecycle:** call `AppsFlyerX::onPause()` from `applicationDidEnterBackground` (or equivalent pause hook) ([API.md](API.md)).

---

## `AndroidManifest`, permissions, and deep links

### Permissions

```xml
<uses-permission android:name="android.permission.INTERNET" />
```

### Native library name

Match your `.so` module name:

```xml
<meta-data android:name="android.app.lib_name"
           android:value="YourGameSoName" />
```

### Deep linking (sample)

The sample adds an `https` App Link / VIEW intent filter on the launcher activity — adjust host and scheme for your OneLink domain:

```xml
<intent-filter>
    <action android:name="android.intent.action.VIEW" />
    <category android:name="android.intent.category.DEFAULT" />
    <category android:name="android.intent.category.BROWSABLE" />
    <data android:scheme="https" android:host="your-subdomain.onelink.me" />
</intent-filter>
```

Register **`registerDeepLinkListener`** in C++ for UDL callbacks. **Warm-start:** `onNewIntent` + **`setIntent`** (above). **Cold-start** may still need **`performDeepLinking`** from C++ with the launch URL after `init` and listeners are registered.

Reference: [`proj.android-studio/app/AndroidManifest.xml`](../proj.android-studio/app/AndroidManifest.xml).

---

## Privacy, AD_ID, and ProGuard

### Google Play `AD_ID` permission

AppsFlyer’s Android SDK manifest may merge `com.google.android.gms.permission.AD_ID` for advertising ID on apps targeting API 33+. Apps directed at children may need to remove or revoke that permission per [Google Play policy](https://support.appsflyer.com/hc/en-us/articles/7569900844689) and [AppsFlyer guidance](https://dev.appsflyer.com/hc/docs/install-android-sdk).

Runtime toggles such as `setDisableAdvertisingIdentifiers` / `disableAppSetId` complement manifest policy but do not replace store declarations.

### ProGuard / R8

If `minifyEnabled true`, keep AppsFlyer rules as needed. Sample [`proguard-rules.pro`](../proj.android-studio/app/proguard-rules.pro):

```proguard
-dontwarn com.appsflyer.**
```

Add official AppsFlyer ProGuard snippets from their docs when you enable full shrinking.

---

## Build this repo’s sample

From the repository root (with `cocos2d/` submodule or tree present as in upstream Cocos samples):

```bash
cd proj.android-studio
./gradlew :AppsFlyerCocos2dX:assembleDebug
./gradlew :AppsFlyerCocos2dX:testDebugUnitTest
```

Module name **`AppsFlyerCocos2dX`** maps to the `app` folder via [`settings.gradle`](../proj.android-studio/settings.gradle).

Native build type is controlled by `PROP_BUILD_TYPE` (`cmake` vs `ndk-build`) in [`gradle.properties`](../proj.android-studio/gradle.properties).

---

## Troubleshooting

| Symptom | What to do |
| --- | --- |
| `AppsFlyerCocosRpcBridge JNI entry not found` | Kotlin bridge missing from app module, wrong package, or ProGuard stripped `@JvmStatic` methods |
| `RPC context not ready (attach Activity before calling SDK)` | Call `onActivityCreated` **before** `super.onCreate()`; delay C++ `init` until after Activity exists |
| `executeRpcBlocking` timeout (504) | Blocking RPC on main thread; heavy `init` / `validateAndLogInAppPurchase` run on background executor in bridge — avoid calling from UI thread from custom Java |
| Duplicate / mismatched SDK versions | BOM + unversioned `af-android-sdk` + explicit bridge; remove extra versioned `af-android-sdk` lines |
| Undefined references to generated symbols | Compile `AppsFlyerRpcSchema.generated.cpp` in your `.so` |
| No conversion / deep-link callbacks | Confirm listener registration **before** `init`; check logcat for `AppsFlyer_Cocos2dX_RPC` errors |

Platform gaps (bridge not shipped as AAR): [SDK7-Lifecycle.md — Limitations](SDK7-Lifecycle.md#5-limitations).

---

## Maintainer reference

| Item | Location |
| --- | --- |
| Sample Gradle pins | [`proj.android-studio/app/build.gradle`](../proj.android-studio/app/build.gradle) |
| Kotlin RPC bridge | [`AppsFlyerCocosRpcBridge.kt`](../proj.android-studio/app/src/main/kotlin/com/appsflyer/cocos2dx/rpc/AppsFlyerCocosRpcBridge.kt) |
| JNI outbound / inbound | [`Classes/AppsFlyer/AppsFlyerRpc.cpp`](../Classes/AppsFlyer/AppsFlyerRpc.cpp) |
| Blocking RPC classification tests | [`BlockingRpcClassificationTest.kt`](../proj.android-studio/app/src/test/kotlin/com/appsflyer/cocos2dx/rpc/BlockingRpcClassificationTest.kt) |