# Phase 2 — RPC synchronization & schema-aligned API (draft)

| Field | Value |
| --- | --- |
| **Status** | **Superseded** — see [`PHASE2-TECH-DESIGN.md`](./PHASE2-TECH-DESIGN.md) |
| **Epic** | [DELIVERY-132096](https://appsflyer.atlassian.net/browse/DELIVERY-132096) |
| **Depends on** | Phase 1 RPC spine merged |
| **Repo** | `appsflyer-cocos2dx-plugin` |
| **Related** | [`PHASE1-TECH-DESIGN.md`](./PHASE1-TECH-DESIGN.md) §8.1 |

This document proposes how Phase 2 closes the **RPC synchronization gap** called out in Phase 1. It aligns Cocos2d-x with the **shared mobile plugin RPC contract** (immediate listener registration, completions, errors, documented init order) without changing Phase 1 scope.

---

## 1. Problem (from Phase 1)

Phase 1 keeps legacy **`AppsFlyerX` void APIs**. Listener setters only store C++ callbacks; native `register*Listener` wires run in an **`initSdk()` batch**. The game cannot:

- Know when a native listener registration succeeded or failed.
- Register a deep-link listener **after** `initSdk()` and still get correct SDK 7 behavior.
- Await `init`, `start`, or `logEvent` completion with error details.

Phase 1 documents this in §8.1 as **intentionally unsupported**.

---

## 2. Selected approach (recommended for Phase 2)

### 2.1 Schema-first public API

- Generate C++ types and dispatch from `schema/appsflyer-plugins-rpc-schema.json` (`publicApi` entries with `cocos2dx` visibility).
- Ship a **breaking 7.x** surface (new header(s) or renamed `AppsFlyerX` methods) documented in migration guide.
- Wire names and platform splits (`subscribeForDeepLink` vs `registerDeeplinkListener`) come from schema **only** — no hand-maintained table in core.

### 2.2 Per-call native RPC for listeners

When the app calls a **register** / **unregister** listener API:

1. Install or update **local** event demux + callback slot (same event channel as Phase 1 `rpcEvent`).
2. Enqueue **one** outbound RPC for that wire on the **RPC worker thread** (never the GL thread).
3. On response, hop to the **cocos game thread** and invoke the app’s **completion handler** (if provided).

**No batching** of listener registration inside `init`. `init` is its own RPC (plus `setPluginInfo`, `enableDebug`, etc.) with the app owning order.

**Ordering rule (documented, same as other mobile plugins):**

- Deep-link listener registration **before** `init`.
- Other listeners: after `init` unless schema/docs specify otherwise (e.g. session-ready).

### 2.3 Completion handlers (C++ idiom)

RPC methods that return `void` on the native side expose:

```cpp
using RpcCompletion = std::function<void(const RpcResult& result)>;

void registerDeepLinkListener(
    std::function<void(AppsFlyerDeepLinkResult)> onDeepLinking,
    RpcCompletion onComplete = {});

void init(const InitParams& params, RpcCompletion onComplete = {});
void start(StartParams params, RpcCompletion onComplete = {});
void logEvent(const LogEventParams& params, RpcCompletion onComplete = {});
```

Rules:

- **`onComplete` is optional.** If omitted, behavior matches fire-and-forget (Phase 1-style) but RPC still runs immediately on the worker — not deferred to a bundled `initSdk`.
- When **`onComplete` is set**, it always runs on the **game thread**, success or failure.
- **`RpcResult`** carries `success`, optional `data` JSON string, and structured `error` (code + message) parsed from the native envelope (Phase 2 typed parse, not substring-only).

Games that need strict sequencing use completions (or chain calls from completion handlers), e.g. register deep link → onComplete → `init` → onComplete → `registerSessionReadyListener` → …

### 2.4 Listener rollback on failure

Mirror the mobile plugin pattern:

- Before calling native `register*Listener`, **snapshot** prior callback state for the affected event name(s).
- Install new callback(s) in the demux registry.
- If native RPC **fails**, **restore** the snapshot and report error via `onComplete`.
- If RPC **succeeds**, keep new callbacks.

Unregister APIs: on success, clear callbacks; on failure, leave prior state unchanged.

### 2.5 Event delivery (unchanged from Phase 1)

- Native → single `rpcEvent` channel → `runOnGameThread` → demux by `event` field → app callbacks.
- Optional: hold/replay early events for a listener registered shortly after attach (evaluate against schema event semantics in implementation; not required for Phase 2 MVP if schema events are stream-only after register).

### 2.6 Threading (extends Phase 1 §8)

| Step | Thread |
| --- | --- |
| App calls generated API | Game thread (typical) |
| Outbound RPC | Dedicated **worker** (same as Phase 1) |
| `onComplete` | **Game thread** |
| `rpcEvent` → app listener | **Game thread** |

### 2.7 `awaitResponse`

Generated `start` / `logEvent` (and other schema methods with `awaitResponse`) pass the param through to JSON. Classification on Android remains in bridge `isBlockingRpc` (extend tests from Phase 1).

---

## 3. Non-goals for this slice (still Phase 2/3)

- Coroutine/`co_await` wrapper (optional thin header later; not required for parity).
- macOS desktop legacy path — keep out of RPC sync scope unless product asks.
- Full typed payload parsing for every event (can land incrementally after completions).

---

## 4. Verification (add to Phase 2 test plan)

- Unit tests: rollback on failed `registerDeepLinkListener` RPC (mock transport).
- Ordering test: deep link register → init → session-ready → start (sample or headless core test).
- Parity checklist against schema `publicApi` for `cocos2dx`.
- Android: extend `BlockingRpcClassificationTest` for any new blocking listener wires if bridge classifies them.

---

## 5. Approval checklist

Before merging this draft into the canonical **Phase 2 — Schema RPC + generated C++ API** Notion page and `internal-docs/PHASE2-TECH-DESIGN.md`:

- [ ] Stakeholder approves **completion-handler** model vs alternatives (e.g. mandatory async-only API, or `std::future` only).
- [ ] Stakeholder approves **breaking 7.x** C++ naming aligned to schema (`registerDeepLinkListener`, …).
- [ ] Bridge/SDK team confirms **immediate** listener RPC order matches Android/iOS host plugins.

---

## Appendix — alternatives considered

| Option | Why not primary |
| --- | --- |
| Keep Phase 1 batching, add completions only on `initSdk` | Does not fix deep-link-before-init or per-listener errors |
| `std::future` as the only surface | Heavier for Cocos games; completions match existing callback style |
| Synchronous blocking RPC on game thread | Violates D2 / ANR risk |
