# RPC schema

- **Committed contract:** `appsflyer-plugins-rpc-schema.json` (`schemaVersion` must match `rpc-schema.version`).
- **Pin:** `rpc-schema.version` — Maven artifact version for `com.appsflyer.plugins:appsflyer-plugins-rpc-schema` (currently **1.2.0**).
- **Refresh from GAR** (repo root, `AR_TOKEN` set):

```bash
RPC_SCHEMA_VERSION="$(tr -d '[:space:]' < schema/rpc-schema.version)" \
  ./scripts/download_rpc_schema_from_gar.sh
```

Phase 1 Android bridge calls RPC by wire name manually in `AppsFlyerXAndroidSdk7.cpp`; Phase 2 can codegen from this JSON (same artifact as js-core / other SDK 7 plugins).
