# Phase 1 POC — SDK 7 RPC spine

**Epic:** [DELIVERY-132096](https://appsflyer.atlassian.net/browse/DELIVERY-132096) · **Feature branch:** `DELIVERY-132096-sdk7-rpc` · **Phase 1 branch:** `DELIVERY-132096-sdk7-rpc-phase1-poc`  
**Status:** Work in progress (uncommitted)  
**Scope:** Prove RPC spine from cocos C++ on **Android and iOS** (Kotlin / Swift → native SDK 7 RPC).  
**Native policy:** new bridge code is **Kotlin (Android) + Swift (iOS)** only; C++ is interop + public API.

**Build check (local):** Android `./gradlew :app:testDebugUnitTest :app:assembleDebug` — **SUCCESS**; iOS simulator `xcodebuild -workspace build-ios-rpc/AppsFlyerCocos2dX.xcworkspace -scheme AppsFlyerCocos2dX -sdk iphonesimulator` — **SUCCESS** (after RpcClient include + Sdk7 header order fix).

## What was built

| Area | Path |
| --- | --- |
| Kotlin RPC bridge | `proj.android-studio/app/src/main/kotlin/com/appsflyer/cocos2dx/rpc/` |
| Swift RPC bridge | `proj.ios_mac/ios/rpc/` (+ CocoaPods `AppsFlyerRPC` / `AppsFlyerFramework`) |
| C++ stack | **App → `AppsFlyerX` → `appsflyer` (`AppsFlyerXCore`) → `AppsFlyerRpcClient` → native bridge → SDK 7 RPC** |
| RPC schema (1.2.0) | `schema/appsflyer-plugins-rpc-schema.json`, `schema/rpc-schema.version`, `scripts/download_rpc_schema_from_gar.sh` |
| Sample hook | Mobile builds use SDK 7 RPC via `AppsFlyerXCore.cpp` only; legacy `AppsFlyerXAndroid.cpp` / `AppsFlyerXApple.mm` not linked |
| Activity context | `AppActivity` → `AppsFlyerCocosRpcBridge.onActivityCreated` |
| Unit test | `BlockingRpcClassificationTest` (executor routing rules) |

### Sample startup (Android, via `AppsFlyerX` in `AppDelegate`)

1. Register legacy-style callbacks (`setDidResolveDeepLink`, conversion setters).
2. `registerSessionReadyListener` → app calls `start()` from that callback.
3. `initSdk()` on a worker: native listener RPCs → `setPluginInfo` → `init`.
4. `onSessionReady` events → cocos thread → session-ready callback → `start()` RPC.

**Crash fix:** SDK 6 JNI referenced `com.appsflyer.internal.platform_extension.PluginInfo`, which is absent on SDK 7. Mobile CMake targets link `AppsFlyerXCore.cpp` + Swift/Kotlin RPC (no legacy platform `.cpp`/`.mm`). `AppDelegate` uses the same SDK 7 RPC bootstrap on Android and iOS.

## How to run (iOS)

1. Configure CMake iOS build dir (example): `cmake -B build-ios-rpc -GXcode -DCMAKE_SYSTEM_NAME=iOS …` (see repo CMake / cocos iOS docs).
2. `./scripts/setup-ios-rpc-pods.sh build-ios-rpc` (or `build-device` for a physical device). If CocoaPods cannot resolve `AppsFlyerRPC/Main`, run `./scripts/setup-ios-rpc-pods.sh <dir> --fresh` or `cd <dir> && pod install --repo-update`.
3. Open **`build-ios-rpc/AppsFlyerCocos2dX.xcworkspace`** (required — `import AppsFlyerRPC` comes from CocoaPods). Do **not** open `proj.ios_mac/AppsFlyerCocos2dX.xcodeproj` or the bare `.xcodeproj` under `build-*`; those builds miss Pods and fail with “Unable to find module dependency: AppsFlyerRPC”.

## How to run (Android)

1. `cocos2d/` present, `local.properties` with SDK/NDK.
2. Build debug APK from `proj.android-studio`. Reconfigure native after CMake changes: `./gradlew :app:clean` or delete `app/.cxx`.
3. Filter logcat: `[AF-SDK7]`, `AppsFlyer_Cocos2dX_RPC`.

```bash
cd proj.android-studio
./gradlew :app:testDebugUnitTest :app:assembleDebug
```

## Gaps found / fixes needed

| ID | Gap | Severity | Phase 1 action | Follow-up |
| --- | --- | --- | --- | --- |
| GAP-001 | ~~Legacy + RPC coexist~~ | — | **Resolved:** Android CMake always uses SDK 7 JNI stub; bootstrap in `AppDelegate` | Phase 2: replace `AppsFlyerX` stub with generated RPC API |
| GAP-002 | ~~iOS static SDK 6.x~~ | — | **Resolved:** Swift `AppsFlyerCocosRpcBridge`; CocoaPods via `scripts/setup-ios-rpc-pods.sh` | iOS attribution / deep-link queue in follow-up |
| GAP-003 | **`setPluginInfo` plugin id** — `cocos2dx` may need bridge enum `Plugin.*` approval | Medium | Using string `cocos2dx` | Confirm with Android/iOS bridge teams |
| GAP-004 | **JSON parsing in C++** — substring check for `"success":true` | Low | Acceptable for POC | Phase 2: use cocos `rapidjson` or generated types |
| GAP-005 | **Dual SDK dependency story** | Medium | Android + iOS samples are SDK 7 + RPC bridge only | Remove leftover `libAppsFlyer` references in docs/macOS target if any |
| GAP-006 | ~~Schema JSON not committed~~ | — | **Resolved:** `schema/appsflyer-plugins-rpc-schema.json` @ 1.2.0 | Phase 2 codegen from schema |
| GAP-007 | **No Gradle module split** — RPC Kotlin lives in sample `app` module, not reusable AAR | Medium | OK for POC | Extract `appsflyer-cocos2dx-bridge` library module |
| GAP-008 | **Blocking JNI from wrong thread** — `executeRpcBlocking` must not be called from GL thread for slow RPC | High | Spike uses `std::thread` | Phase 2: completions on game thread ([`PHASE2-SYNC-DESIGN-DRAFT.md`](./PHASE2-SYNC-DESIGN-DRAFT.md)) |
| GAP-013 | **RPC synchronization to the game** — no per-call native listener RPC, no completion/error/rollback on register* / init / start / logEvent | High | Documented Phase 1 §8.1; POC batches listeners in `initSdk()` | Phase 2: [`PHASE2-TECH-DESIGN.md`](./PHASE2-TECH-DESIGN.md) — `AppsFlyerTask` + immediate native calls |
| GAP-009 | **Xcode / CMake Swift** | — | **Resolved:** Swift RPC sources listed in `CMakeLists.txt` (IOS); `scripts/setup-ios-rpc-pods.sh` for Pods | Re-run cmake + pod install after RPC file changes |
| GAP-010 | **Deep link / attribution** — no Swift `AppsFlyerAttribution` queue in POC | Medium | Legacy ObjC++ path still active on iOS | Port lifecycle from SDK 7 plugin pattern in Phase 1 PR |
| GAP-011 | **Gradle `sourceSets.main`** — `java.srcDir "src"` pulled `src/test/kotlin` into main compile | Fixed in POC | Use explicit `java.srcDirs` for main vs test | Keep in Phase 1 PR |
| GAP-012 | **JVM unit tests** need `org.json:json` on test classpath for `JSONObject` | Fixed in POC | `testImplementation org.json:json` | Optional Robolectric later for handler integration tests |

## Resolved in POC (threading)

- RPC handler runs on **dedicated executors**, not JNI/GL thread.
- Events hop **main looper → JNI → cocos scheduler**.
- Classification test for `isBlockingRpc` / `awaitResponse`.

## Next steps toward Phase 1 PR

1. Validate APK on device/emulator; capture logcat for spike + events.
2. Confirm `setPluginInfo` / `init` with bridge team.
3. ~~Disable legacy Android init~~ — Android path is RPC-only in CMake.
4. iOS: run on simulator/device; filter console for `[AF-SDK7]` (same flow as Android).
5. Extract bridge to library module; add `internal-docs/THREADING.md`.
6. Commit on feature branch when green (user requested no commit during POC session).

## Related docs

- **Phase 1 tech design (authoritative):** [`PHASE1-TECH-DESIGN.md`](./PHASE1-TECH-DESIGN.md)
- [Phase 1 — SDK 7 + RPC spine (PR1) (Notion)](https://app.notion.com/p/appsflyerrnd/Phase-1-SDK-7-RPC-spine-PR1-40cb7b13af5f81378541ed80412139fa) — published copy after approval
