# Migration guide — AppsFlyer Cocos2d-x 6.x → SDK 7 (RPC)

This guide covers the **Phase 2 POC** task surface on **`AppsFlyerX`** (schema tasks via generated headers; hand glue in `AppsFlyer/rpc/AppsFlyerRpcPlugin.h`). Full schema codegen for every `publicApi` method is a follow-up.

## Breaking changes (summary)

| 6.x / Phase 1 | SDK 7 (`AppsFlyerX` task methods; internal transport in `AppsFlyerRpcTransport.h`) |
| --- | --- |
| `void` methods, fire-and-forget worker | Every POC method returns **`AppsFlyerTask`** |
| Listener setters + batched `initSdk()` | **Immediate** native RPC per `register*Listener`; **`init`** is separate |
| Errors logged only (`[AF-SDK7]`) | **`AppsFlyerResult`** / **`AppsFlyerError`** via **`whenComplete`** |
| Fixed `awaitResponse: false` on start/logEvent | **`StartParams` / `LogEventParams`** pass **`awaitResponse`** through |

Public names do **not** include `Rpc`. Internal transport remains `AppsFlyerRpcClient` / `executeRpcBlocking`.

## Recommended init sequence (SDK 7)

1. Configure dev key / Apple app ID (legacy `AppsFlyerX::setAppsFlyerDevKey`, `setAppleAppID` still used in the sample).
2. **`registerDeepLinkListener`** → on success → **`init`**
3. **`registerConversionListener`** (optional) → **`registerSessionReadyListener`**
4. In session-ready callback → **`start`**
5. **`logEvent`** when needed

### Before (Phase 1 batch)

```cpp
AppsFlyerX::setDidResolveDeepLink(didResolveDeepLink);
AppsFlyerX::registerSessionReadyListener(onSessionReadyStartSdk);
AppsFlyerX::initSdk(); // batched listener RPCs + init
```

### After (Phase 2 — `whenComplete` chain)

```cpp
AppsFlyerX::registerDeepLinkListener([](AppsFlyerXDeepLinkResult r) { /* ... */ })
    .whenComplete([](const AppsFlyerResult& r) {
        if (!r.success) return;
        AppsFlyerX::InitParams p;
        p.devKey = "...";
        p.appId = "..."; // iOS
        p.isDebug = true;
        AppsFlyerX::init(p).whenComplete([](const AppsFlyerResult& r2) {
            if (!r2.success) return;
            AppsFlyerX::registerSessionReadyListener(onSessionReadyStartSdk);
        });
    });
```

See `Classes/AppDelegate.cpp` for the full sample chain.

## Legacy `AppsFlyerX::initSdk()`

Still present for transitional callers but **no longer registers listeners**. Prefer `AppsFlyerX::init` and explicit listener tasks.

## Testing locally

```bash
./scripts/run-af-task-tests.sh
```

Android smoke build:

```bash
cd proj.android-studio && ./gradlew :app:testDebugUnitTest :app:assembleDebug
```
