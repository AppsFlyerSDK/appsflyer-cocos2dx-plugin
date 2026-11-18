# Cocos2d-x lifecycle and AppsFlyer SDK 7

Integrator flow for **schema-generated** `AppsFlyerX` (`AppsFlyerTask` + `whenComplete`). For migration from 6.x or batched `initSdk()`, see [migration-guide.md](migration-guide.md).

**Platforms:** Android and iOS only ([`CHANGELOG.md`](../CHANGELOG.md)).

---

## 1. Native entry hooks

| Platform | Hook |
|----------|------|
| **Android** | `AppActivity.onCreate` → `AppsFlyerCocosRpcBridge.onActivityCreated(this)` **before** `super.onCreate()` |
| **iOS** | `AppController` → `[AppsFlyerCocosRpcBridge installEventPipeline]` **before** `Application::run()` |

---

## 2. Recommended startup order (sample: `Classes/AppDelegate.cpp`)

On the **game thread** during `applicationDidFinishLaunching()`:

1. Optional schema calls (`stop`, `setPartnerData`, …).
2. **`registerDeepLinkListener`** → **`enableDebug(true)`** → **`init(InitParams)`** (nested `whenComplete`).
3. **`registerConversionListener`** → **`registerSessionReadyListener`**.
4. In session-ready callback → **`start(awaitResponse)`** / **`logEvent`**.

**Do not** call `initSdk()` or legacy void listener setters (`setOnConversionDataSuccess`, `setDidResolveDeepLink`). Use listener register tasks instead.

**Internal:** `setPluginInfo` runs automatically on the worker immediately before the first `init` / `initialize` RPC.

---

## 3. Threading

| Path | Rule |
|------|------|
| Outbound RPC | Worker `std::thread` → blocking native bridge |
| `AppsFlyerTask.whenComplete` | Always on **game thread** |
| Inbound `rpcEvent` | Native → bridge → `AppsFlyerRpc` → game thread → app callbacks |

---

## 4. Events

Native emits JSON on `rpcEvent`. C++ demuxes by top-level `"event"` and parses `"data"` into:

- Conversion success/fail → `cocos2d::ValueMap`
- Deep link → `appsflyer::DeepLinkData` (`status` → FOUND / NOT_FOUND / ERROR, optional `error`, `deepLink` map)

---

## 5. Limitations

- **GAP-010:** No iOS cold-start attribution queue / `handleOpenURL` RPC glue in-plugin.
- **GAP-007:** Android RPC Kotlin remains in the sample app module (not a published AAR).
- Nested JSON in event payloads may require future parser hardening.

---

## 6. File map

| Concern | Location |
|---------|----------|
| Sample chain | `Classes/AppDelegate.cpp` |
| Public API | `Classes/AppsFlyer/AppsFlyerX.h` + `generated/*` |
| Transport | `Classes/AppsFlyer/AppsFlyerRpc.{h,cpp}` |
| Codegen | `schema/`, `scripts/codegen/run.sh`, `tools/af-schema-compiler/` |
| Android bridge | `proj.android-studio/.../AppsFlyerCocosRpcBridge.kt` |
| iOS bridge | `proj.ios_mac/ios/rpc/AppsFlyerCocosRpcBridge.*` |