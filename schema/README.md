# RPC schema

- **Committed contract:** `appsflyer-plugins-rpc-schema.json` (`schemaVersion` must match `rpc-schema.version`).
- **Pin:** `rpc-schema.version` — Maven artifact version for `com.appsflyer.plugins:appsflyer-plugins-rpc-schema` (currently **1.2.0**).
- **Refresh from GAR** (repo root, `AR_TOKEN` set):

```bash
RPC_SCHEMA_VERSION="$(tr -d '[:space:]' < schema/rpc-schema.version)" \
  ./scripts/download_rpc_schema_from_gar.sh
```

Phase 1 calls RPC by wire name manually in `Classes/AppsFlyer/AppsFlyerXCore.cpp` (see `internal-docs/PHASE1-TECH-DESIGN.md` §5.3). There is no separate Android SDK 7 C++ module. Phase 2 codegen from this JSON replaces hand-maintained wires (same artifact as js-core / other SDK 7 plugins).
