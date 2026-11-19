<img src="https://www.appsflyer.com/wp-content/uploads/2016/11/logo-1.svg"  width="200">

# Cocos2dX iOS integration (SDK 7)

Native AppsFlyer iOS SDKs are **not** in this repo — you add them with **Swift Package Manager** on your Xcode **app** target. Minimum deployment target: **`minimumIos`** in [`version/ios-spm-versions.json`](../version/ios-spm-versions.json) (**15.0** today).

> **Deep links / cold start:** copy `AppsFlyerCocosAttribution.swift` and patch `AppController` as in the sample ([Native entry hook](#native-entry-hook)). UIKit may deliver a link before C++ `init()`; attribution buffers until `initialize` succeeds.

### Prerequisites

| Requirement | Notes |
| --- | --- |
| **macOS + Xcode 15+** | Swift 5.9+, SPM |
| **iOS Deployment Target** | **≥ `minimumIos`** in [`version/ios-spm-versions.json`](../version/ios-spm-versions.json) (App Store / Xcode app target and Cocos iOS binary) |
| **App target with Swift enabled** | SPM + `AppsFlyerCocosRpcBridge.swift` on the **app**, not lib-only |
| **Network** | First package resolve needs GitHub access |

### At a glance

1. **Copy** `Classes/AppsFlyer/` and the iOS bridge files under `proj.ios_mac/ios/` (see [Copy manifest](#copy-manifest)).
2. **Add** Swift packages **AppsFlyerRPC** + **AppsFlyerLib-Static** ([SPM](#swift-package-manager-app-target)).
3. **Patch** `AppController.mm` — event pipeline + URL lifecycle hooks before `Application::run()` ([Native entry hook](#native-entry-hook)).
4. **Init** AppsFlyer from C++ (`AppDelegate.cpp`) — [minimal snippet](snippets/ios-minimal-init.cpp) · [lifecycle diagram](SDK7-Lifecycle.md#ios-startup-timeline-cocos2d-x).

| More detail | Document |
| --- | --- |
| Migrating from 6.x → 7 (packaging & API) | [Migration-Guide.md](Migration-Guide.md) |
| Init order & threading | [SDK7-Lifecycle.md](SDK7-Lifecycle.md) |
| Version pins | [`version/ios-spm-versions.json`](../version/ios-spm-versions.json) |

**On this page:** [Integration checklist](#integration-checklist) · [Cocos2d-x on iOS](#cocos2d-x-on-ios-two-entry-files) · [Integration paths](#integration-paths) · [Case A](#case-a--cmake-sample) · [Copy manifest](#copy-manifest) · [Xcode](#xcode-configuration) · [ATT & privacy](#att-tracking-and-privacy-plist) · [Main vs Strict](#main-vs-strict) · [Troubleshooting](#troubleshooting)

---

## Integration checklist

Use this order for **manual Xcode** (Case B/C). CMake sample (Case A): run [`./scripts/build-ios-cmake.sh`](../scripts/build-ios-cmake.sh) — [Case A](#case-a--cmake-sample).

- [ ] **1. Copy** [`Classes/AppsFlyer/`](../Classes/AppsFlyer/) → `<game>/Classes/AppsFlyer/` (include `generated/`).
- [ ] **2. Copy** iOS bridge: `proj.ios_mac/ios/rpc/*` (including `AppsFlyerCocosAttribution.swift`), `SwiftBridge.swift`, bridging header → your iOS tree ([paths](#copy-manifest)).
- [ ] **3. Xcode — Compile sources** on app target: `AppsFlyerRpc.cpp`, `AppsFlyerTask.cpp`, `generated/AppsFlyerRpcSchema.generated.cpp`, bridge `.mm` + `.swift`.
- [ ] **4. Xcode — Header Search Paths:** your `Classes` folder; folder with `AppsFlyerCocosRpcBridge.h`.
- [ ] **5. SPM** on app target: **AppsFlyerRPC** + **AppsFlyerLib-Static** at exact versions from `version/ios-spm-versions.json` → **Resolve Package Versions** ([SPM](#swift-package-manager-app-target)).
- [ ] **6. Link** system frameworks: **AdSupport**, **AdServices**, **StoreKit**.
- [ ] **7. Build settings:** bridging header path, **Always Embed Swift Standard Libraries** = Yes, **Other Linker Flags** `-ObjC`, runpath `@executable_path/Frameworks` ([build settings](#build-settings-app-target)).
- [ ] **8. `AppController.mm`:** `installEventPipeline`, `handleLaunchOptions`, `openURL`, and `continueUserActivity` ([hook](#native-entry-hook)).
- [ ] **9. `AppDelegate.cpp`:** SDK 7 listener → `init` → `start` chain ([minimal snippet](snippets/ios-minimal-init.cpp), [full sample](../Classes/AppDelegate.cpp)).
- [ ] **10. `Info.plist`:** ATT / privacy keys if needed ([ATT & privacy](#att-tracking-and-privacy-plist)).
- [ ] **11. Build & run** — verify [success criteria](#success-criteria) below.

### Success criteria

After a debug build on device or simulator:

| # | Check | How |
| --- | --- | --- |
| 1 | Project compiles | No `No such module 'AppsFlyerRPC'` / missing `-Swift.h` |
| 2 | Event pipeline alive | Breakpoint or log: `installEventPipeline` runs before `run()` |
| 3 | `init` succeeds | `AppsFlyerX::init(…).whenComplete` → `success == true` (see snippet) |
| 4 | Session ready | Session-ready callback fires; then `start` completes |
| 5 | Optional smoke | `enableDebug(true)` → AppsFlyer logs in Xcode console; `logEvent` after `start` |

If something fails, see [Troubleshooting](#troubleshooting).

---

## Cocos2d-x on iOS: two entry files

| File | Runs on | Your AppsFlyer responsibility |
| --- | --- | --- |
| **`AppController.mm`** (`UIApplicationDelegate`) | UIKit / main, **before** the game loop | `installEventPipeline`, `handleLaunchOptions`, `openURL`, `continueUserActivity` — event pipeline + deep-link forwarding into `AppsFlyerLib` |
| **`AppDelegate.cpp`** (`cocos2d::Application`) | **Game thread** after `run()` | `registerDeepLinkListener` → `init` → conversion / session listeners → `start` ([SDK7-Lifecycle](SDK7-Lifecycle.md)) |

If `installEventPipeline` is missing, RPC may run but **listeners never fire**. If C++ `init` is missing, the pipeline has nothing to talk to.

---

## Integration paths

| Case | Your setup | Follow |
| --- | --- | --- |
| **A** | Building **this repo’s** sample with CMake | [Case A — CMake sample](#case-a--cmake-sample) |
| **B** | Classic Cocos2d-x tree (`Classes/` + `proj.ios_mac/ios/`) | [Copy manifest](#copy-manifest) + [Xcode configuration](#xcode-configuration) |
| **C** | Custom folders (Creator, `runtime-src`, etc.) | Same **files** as B; adjust paths in Xcode build settings ([note](#case-c--cocos-creator-and-other-layouts)) |
| **D** | Kids / Strict binary | Case B or C + [Main vs Strict](#main-vs-strict) |

macOS desktop is **not** supported on SDK 7.

### Case C — Cocos Creator and other layouts

The plugin does not ship Creator-specific project templates. Regardless of folder names:

1. Put the **C++ plugin** next to your other game `Classes/` (often `native/engine/common` or project root `Classes/`).
2. Put **`rpc/`**, **`SwiftBridge.swift`**, and the **bridging header** beside your iOS `AppController` sources.
3. Point **Header Search Paths** and the **bridging header** build setting at those real locations.

No Python/`setup-ios-spm.sh` is required unless you use this repo’s **CMake** sample (Case A). For manual Xcode, add SPM in the UI.

---

## Case A — CMake sample

There is no checked-in `.xcodeproj` under `proj.ios_mac/`. From the plugin repo root:

**One-shot build** (regenerates Xcode project, wires SPM, builds):

```bash
./scripts/build-ios-cmake.sh build DEVELOPMENT_TEAM=<Apple-team-id>
```

**Manual steps** (same result without compiling):

```bash
# CMAKE_OSX_DEPLOYMENT_TARGET must be >= minimumIos in version/ios-spm-versions.json.
# This repo's CMake cache may use cmakeDeploymentTarget (sample-only pin) — integrators can use 15.0+.
cmake -B build-device -GXcode \
  -DCMAKE_SYSTEM_NAME=iOS \
  -DCMAKE_OSX_SYSROOT=iphoneos \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=15.0
./scripts/setup-ios-spm.sh build-device
```

- Open **`build-device/AppsFlyerCocos2dX.xcodeproj`** (not a `.xcworkspace`).
- `setup-ios-spm.sh` injects SPM package references, fixes CMake **legacy build locations** (required for SwiftPM), and resolves packages into `build-device/SourcePackages`.
- Device binaries use `-derivedDataPath build-device/DerivedData` (see `scripts/build-ios-cmake.sh`).

**Simulator:** use `-DCMAKE_OSX_SYSROOT=iphonesimulator` when running `cmake`, then the same SPM setup.

**Important:** every `cmake -B …` **regenerates** `project.pbxproj` and **removes** SPM. Run `./scripts/setup-ios-spm.sh build-device` again after cmake (or use `./scripts/build-ios-cmake.sh`, which does this for you).

---

## Copy manifest

Copy from **this plugin repository** into **your game repository**. The right column is the usual Cocos2d-x layout; if you use Case C, keep the same **grouping** and point Xcode at your real folders.

### 1. C++ plugin (required on iOS and Android)

| Plugin repo | Your project (typical) |
| --- | --- |
| `Classes/AppsFlyer/` **(entire folder)** | `<game-root>/Classes/AppsFlyer/` |

**Include the whole tree**, especially `generated/`. Do not omit files unless you regenerate schema yourself.

| File | Purpose |
| --- | --- |
| `AppsFlyerX.h` | Public API |
| `AppsFlyerRpc.{h,cpp}` | RPC transport |
| `AppsFlyerTask.{h,cpp}`, `AppsFlyerTaskState.h`, `AppsFlyerResult.h` | Async tasks / results |
| `AppsFlyerDeepLinkData.h`, `AppsFlyerXMacro.h` | Deep links, event constants |
| `generated/AppsFlyerRpcSchema.generated.{h,cpp}` | Schema wire encoding — **`.cpp` must compile** |
| `generated/AppsFlyerXSchemaMethods.generated.h` | Generated methods |
| `generated/AppsFlyerPluginMetadata.generated.h` | `setPluginInfo` version metadata |

**Must compile on iOS** (add to app target or CMake `GAME_SOURCE`):

- `AppsFlyerRpc.cpp`
- `AppsFlyerTask.cpp`
- `generated/AppsFlyerRpcSchema.generated.cpp`

**Header search:** add `<game-root>/Classes` so game code can `#include "AppsFlyer/AppsFlyerX.h"`. Reference: [`CMakeLists.txt`](../CMakeLists.txt) AppsFlyer entries.

---

### 2. iOS RPC bridge

| Plugin repo | Your project (typical) |
| --- | --- |
| `proj.ios_mac/ios/rpc/AppsFlyerCocosRpcBridge.h` | `…/ios/rpc/AppsFlyerCocosRpcBridge.h` |
| `proj.ios_mac/ios/rpc/AppsFlyerCocosRpcBridge.mm` | `…/ios/rpc/AppsFlyerCocosRpcBridge.mm` |
| `proj.ios_mac/ios/rpc/AppsFlyerCocosRpcBridge.swift` | `…/ios/rpc/AppsFlyerCocosRpcBridge.swift` |
| `proj.ios_mac/ios/SwiftBridge.swift` | `…/ios/SwiftBridge.swift` |
| `proj.ios_mac/ios/AppsFlyerCocos2dX-Bridging-Header.h` | `…/ios/<YourApp>-Bridging-Header.h` (rename to match target) |

**Bridging header** (`proj.ios_mac/ios/AppsFlyerCocos2dX-Bridging-Header.h` — rename when copying). Contents:

```objc
#import "rpc/AppsFlyerCocosRpcBridge.h"
```

If `rpc/` is not next to the bridging header, change the import to a path Xcode can resolve.

**Target membership:** `.mm`, both `.swift` files, and the three plugin `.cpp` files above must belong to the **iOS application target** (Swift must link into the app that ships).

---

### 3. Files you change (not copied from the plugin)

| Your file | What to do |
| --- | --- |
| `AppController.mm` (or app delegate) | [Native entry hook](#native-entry-hook) |
| `AppDelegate.cpp` | SDK 7 init chain — see sample `Classes/AppDelegate.cpp` and [SDK7-Lifecycle.md](SDK7-Lifecycle.md) |
| `Info.plist` | [ATT & privacy](#att-tracking-and-privacy-plist); URL types if you handle links yourself |

### Example layout (Case B)

```
YourGame/
├── Classes/
│   ├── AppDelegate.cpp
│   └── AppsFlyer/          ← full copy from plugin
├── proj.ios_mac/ios/
│   ├── AppController.mm    ← patch
│   ├── SwiftBridge.swift
│   ├── YourGame-Bridging-Header.h
│   └── rpc/
│       ├── AppsFlyerCocosAttribution.swift
│       ├── AppsFlyerCocosRpcBridge.h
│       ├── AppsFlyerCocosRpcBridge.mm
│       └── AppsFlyerCocosRpcBridge.swift
```

---

## Xcode configuration

Applies to **Case B** and **Case C** (manual Xcode). Case A gets most settings from CMake + `setup-ios-spm.sh`.

### Swift Package Manager (app target)

**File → Add Package Dependencies…**

| Package | URL | Product | Version |
| --- | --- | --- | --- |
| AppsFlyer RPC | `https://github.com/AppsFlyerSDK/appsflyer-apple-rpc.git` | **AppsFlyerRPC** | Exact: `appsflyerAppleRpc` in `version/ios-spm-versions.json` |
| AppsFlyer Framework (static) | `https://github.com/AppsFlyerSDK/AppsFlyerFramework-Static.git` | **AppsFlyerLib-Static** | Exact: `appsflyerFrameworkStatic` |

Add both products under **Frameworks, Libraries, and Embedded Content** for the **app** target only.

### Build settings (app target)

| Setting | Value |
| --- | --- |
| iOS Deployment Target | **≥ `minimumIos`** ([`version/ios-spm-versions.json`](../version/ios-spm-versions.json)) |
| Swift Language Version | **5.9+** |
| Objective-C Bridging Header | e.g. `proj.ios_mac/ios/YourGame-Bridging-Header.h` |
| Product Module Name | Drives `<Module>-Swift.h` (sample: `AppsFlyerCocos2dX`) |
| Always Embed Swift Standard Libraries | **Yes** |
| Runpath Search Paths | `@executable_path/Frameworks` (and inherited) |
| Other Linker Flags | **`-ObjC`** when linking static Cocos/libs + ObjC/Swift RPC |
| Header Search Paths | Your `Classes` folder; folder containing `AppsFlyerCocosRpcBridge.h` |

### System frameworks

Link on the app target: **AdSupport**, **AdServices**, **StoreKit** (see [`CMakeLists.txt`](../CMakeLists.txt)).

### Native entry hook

At the top of `AppController.mm` (with other imports):

```objc
#if __has_include("YourModule-Swift.h")
#import "YourModule-Swift.h"
#endif
```

Replace `YourModule` with **Product Module Name** (sample: `AppsFlyerCocos2dX-Swift.h`).

Inside `application:didFinishLaunchingWithOptions:`, **before** `app->run()`:

```objc
#if __has_include("YourModule-Swift.h")
    [AppsFlyerCocosRpcBridge installEventPipeline];
    [[AppsFlyerCocosAttribution shared] handleLaunchOptions:launchOptions];
#endif
```

Also implement (same `#if` guard):

```objc
- (BOOL)application:(UIApplication *)app
            openURL:(NSURL *)url
            options:(NSDictionary<UIApplicationOpenURLOptionsKey, id> *)options {
    [[AppsFlyerCocosAttribution shared] handleOpen:url options:options];
    return YES;
}

- (BOOL)application:(UIApplication *)application
continueUserActivity:(NSUserActivity *)userActivity
  restorationHandler:(void (^)(NSArray<id<UIUserActivityRestoring>> *_Nullable))restorationHandler {
    [[AppsFlyerCocosAttribution shared] continueUserActivity:userActivity restorationHandler:nil];
    return YES;
}
```

`AppsFlyerCocosAttribution` buffers `openURL` / universal links until C++ `AppsFlyerX::init` completes successfully (native `initialize` RPC), then forwards to `AppsFlyerLib` — same pattern as Capacitor/React Native.

Reference implementation: [`proj.ios_mac/ios/AppController.mm`](../proj.ios_mac/ios/AppController.mm).

---

## ATT, tracking, and privacy (`Info.plist`)

| Topic | Guidance |
| --- | --- |
| **App Tracking Transparency** | Add **`NSUserTrackingUsageDescription`** before showing the ATT prompt (Apple requirement). Trigger timing follows your product + [AppsFlyer iOS SDK guidance](https://dev.appsflyer.com/hc/docs/integrate-ios-sdk). |
| **Disable IDFA collection (runtime)** | `AppsFlyerX::setDisableAdvertisingIdentifiers(true)` — does **not** replace linking a **Strict** binary ([Main vs Strict](#main-vs-strict)). |
| **Privacy manifest** | Shipped with the **AppsFlyer SPM** binary you link (Main vs Strict). |
| **SKAdNetwork** | Add required `SKAdNetworkItems` to `Info.plist` per AppsFlyer for your SDK version. |

This plugin does not replace Apple’s ATT UI — you implement `ATTrackingManager` in native code or a thin Obj‑C++ wrapper if your game needs a custom prompt flow.

---

## Main vs Strict

| | **Main (regular)** | **Strict (kids / privacy binary)** |
| --- | --- | --- |
| **SPM products (this repo)** | `AppsFlyerRPC` + `AppsFlyerLib-Static` — pins in `version/ios-spm-versions.json` | Product names **not** pinned here; read [Strict mode (iOS)](https://dev.appsflyer.com/hc/docs/install-ios-sdk#strict-mode-sdk) for your versions |
| **Sample** | Smoke-tested via CMake + `setup-ios-spm.sh` | **Not** smoke-tested in this repo — validate in your Xcode project |
| **Build flag** | Default | Add `AFSDK_NO_IDFA=1` on the app target when AppsFlyer docs require it for Strict |

Choose the variant when you **link** the SDK, not via `setDisableAdvertisingIdentifiers` alone. Overview: [Migration-Guide.md — Main vs Strict](Migration-Guide.md#main-vs-strict-ios-native-binaries).

---

## Troubleshooting

| Symptom | What to do |
| --- | --- |
| `No such module 'AppsFlyerRPC'` | SPM products on **app** target; **File → Packages → Resolve Package Versions** |
| `…-Swift.h` not found | Swift files in app target; check Product Module Name and bridging header path |
| `Packages are not supported when using legacy build locations` | **CMake:** re-run `setup-ios-spm.sh`. **Manual Xcode:** File → Project Settings → Build Location → **Default** |
| Undefined schema symbols | Compile `AppsFlyerRpcSchema.generated.cpp` |
| SPM missing after cmake | Re-run `./scripts/setup-ios-spm.sh` |
| RPC/Framework version mismatch | Update **both** pins in `version/ios-spm-versions.json` |

Known product gaps: [SDK7-Lifecycle.md — Limitations](SDK7-Lifecycle.md#5-limitations)

---

## Maintainer reference

| Item | Location |
| --- | --- |
| SPM pins | `version/ios-spm-versions.json` |
| SPM inject after CMake | `scripts/setup-ios-spm.sh`, `scripts/ios/inject_spm_into_xcodeproj.py` |
| Bridge source | `proj.ios_mac/ios/rpc/` |