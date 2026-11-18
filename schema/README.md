# RPC schema

- **Committed contract:** `appsflyer-plugins-rpc-schema.json` (`schemaVersion` must match `rpc-schema.version`).
- **Pin:** `rpc-schema.version` — Maven artifact version for `com.appsflyer.plugins:appsflyer-plugins-rpc-schema` (currently **1.2.0**).

## Refresh from Maven (maintainers)

Maintainers with Google Artifact Registry read access can refresh the contract with local `gcloud` credentials:

1. Authenticate: `gcloud auth login` (or export `AR_TOKEN`; optional overrides in `schema/gar.local.env` — see `gar.env.example`).
2. From repo root:

```bash
./scripts/download_rpc_schema_from_gar.sh
```

By default this resolves **Maven latest** for `com.appsflyer.plugins:appsflyer-plugins-rpc-schema` from `europe-west1-maven.pkg.dev/af-artifacts-prd/maven-internal`. Pin a specific release with `RPC_SCHEMA_VERSION=1.2.0`.

3. Update `schema/rpc-schema.version` to match the JSON `schemaVersion`, then:

```bash
./scripts/codegen/run.sh
./scripts/verify-schema-codegen.sh
```

**No registry access?** Use the committed JSON in the repo. Schema bumps are done by maintainers who fetch from Maven, regenerate, and commit the updated JSON plus `Classes/AppsFlyer/generated/`.

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