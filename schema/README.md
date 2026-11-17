# RPC schema

- **Committed contract:** `appsflyer-plugins-rpc-schema.json` (`schemaVersion` must match `rpc-schema.version`).
- **Pin:** `rpc-schema.version` — Maven artifact version for `com.appsflyer.plugins:appsflyer-plugins-rpc-schema` (currently **1.2.0**).
- **Refresh from GAR** (repo root; requires `AR_TOKEN` — OAuth token with read access to `maven-internal`; CI uses `appsflyerrnd/setup-ar-auth`, local dev often `export AR_TOKEN="$(gcloud auth print-access-token)"`):

  Maven: `com.appsflyer.plugins:appsflyer-plugins-rpc-schema` → `schema/appsflyer-plugins-rpc-schema.json` via `scripts/download_rpc_schema_from_gar.sh` (defaults: project `af-artifacts-prd`, repo `europe-west1-maven.pkg.dev/.../maven-internal`).

```bash
RPC_SCHEMA_VERSION="$(tr -d '[:space:]' < schema/rpc-schema.version)" \
  ./scripts/download_rpc_schema_from_gar.sh
```

After download, confirm `schemaVersion` in the JSON matches `rpc-schema.version`, then run `./scripts/codegen/run.sh` and `./scripts/verify-schema-codegen.sh`.

**No GAR access?** Use the committed JSON in the repo. Schema bumps require a maintainer with GAR read access to fetch the pinned Maven artifact, regenerate (`./scripts/codegen/run.sh`), and commit the updated JSON plus `Classes/AppsFlyer/generated/`.

## C++ codegen

```bash
./scripts/codegen/run.sh
```

Builds `tools/af-schema-compiler` (CMake + nlohmann/json via FetchContent) and writes:

- `Classes/AppsFlyer/generated/AppsFlyerRpcSchema.generated.{h,cpp}` — `appsflyer::` task APIs
- `Classes/AppsFlyer/generated/AppsFlyerXSchemaMethods.generated.h` — inline `AppsFlyerX::` wrappers (included from `AppsFlyerX.h`)

Codegen covers all schema methods marked public for Cocos (currently **79**), including `enableDebug` (wire `isDebug`), and the three listener register/unregister pairs (implementations in `AppsFlyerRpc.cpp`).

Internal RPC (e.g. **`setPluginInfo`**) is not on `AppsFlyerX`; the plugin sends it automatically immediately before the first `init` / `initialize` wire call.

Parity check:

```bash
./scripts/verify-schema-codegen.sh
```