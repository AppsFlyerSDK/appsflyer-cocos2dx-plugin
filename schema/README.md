# RPC schema

- **Committed contract:** `appsflyer-plugins-rpc-schema.json` (`schemaVersion` must match `rpc-schema.version`).
- **Pin:** `rpc-schema.version` — Maven artifact version for `com.appsflyer.plugins:appsflyer-plugins-rpc-schema` (currently **1.2.0**).
- **Refresh from GAR** (repo root; requires `AR_TOKEN` — OAuth token with read access to `maven-internal`; CI uses `appsflyerrnd/setup-ar-auth`, local dev often `export AR_TOKEN="$(gcloud auth print-access-token)"`):

  Maven: `com.appsflyer.plugins:appsflyer-plugins-rpc-schema` → `schema/appsflyer-plugins-rpc-schema.json` via `scripts/download_rpc_schema_from_gar.sh` (defaults: project `af-artifacts-prd`, repo `europe-west1-maven.pkg.dev/.../maven-internal`).

```bash
RPC_SCHEMA_VERSION="$(tr -d '[:space:]' < schema/rpc-schema.version)" \
  ./scripts/download_rpc_schema_from_gar.sh
```

  After download, confirm `schemaVersion` in the JSON matches `rpc-schema.version`. See `internal-docs/PHASE2-TECH-DESIGN.md` §6.2 for env vars and maintainer workflow.

  **No GAR access?** Use the committed JSON in the repo. Schema bumps require a maintainer (typically `appsflyerrnd` org) to fetch from GAR, regenerate, and commit—see §6.2 problem/mitigation in the Phase 2 tech design.

Phase 2 C++ codegen (C++ schema compiler, not Node):

```bash
./scripts/codegen/run.sh
```

Builds `tools/af-schema-compiler` (CMake + nlohmann/json via FetchContent) and writes:

- `Classes/AppsFlyer/generated/AppsFlyerRpcSchema.generated.{h,cpp}` — `appsflyer::` task APIs; param **structs** only when a method has 2+ parameters (single-param methods take the field directly), inline wire names + JSON encoding
- `Classes/AppsFlyer/generated/AppsFlyerXSchemaMethods.generated.h` — inline `AppsFlyerX::` wrappers (included from `AppsFlyerX.h`)

Codegen covers all schema methods marked public for Cocos (currently **79**), including `enableDebug` (wire `isDebug`), and the three listener register/unregister pairs (implementations in `AppsFlyerRpcClient.cpp`; declarations + `AppsFlyerX` inline wrappers from codegen).

Internal RPC (schema `plugins.default: internal`, e.g. **`setPluginInfo`**) is not on `AppsFlyerX`; the plugin sends it automatically immediately before the first `init` / `initialize` wire call in `executeRpc`. Legacy SDK 6 helpers remain under `AppsFlyerXAndroid` / `AppsFlyerXApple` for reference, not on the public `AppsFlyerX` surface.
