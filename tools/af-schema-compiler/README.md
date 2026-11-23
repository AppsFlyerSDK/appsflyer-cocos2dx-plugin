# af-schema-compiler

Build-time C++ tool that reads `schema/appsflyer-plugins-rpc-schema.json` and writes
`Classes/AppsFlyer/generated/*` — RPC methods, schema string enums (e.g. `MediationNetwork`, `LogLevel`,
`PurchaseType`), schema `valueMap` resolvers and named `transform` resolvers (`androidLogLevel`,
`androidPurchaseType`, …), and `AppsFlyerX` wrappers (three files: `AppsFlyerRpcSchema.generated.*`,
`AppsFlyerXSchemaMethods.generated.h`).

```bash
# From repo root
./scripts/codegen/run.sh
```

Requires CMake 3.16+, a C++17 compiler, and network on first configure (FetchContent for nlohmann/json).

Sources: `src/main.cpp`, `schema_model.{hpp,cpp}`, `generator.{hpp,cpp}` (`afcodegen::generateAll`).

Plugin version metadata for internal `setPluginInfo` is emitted from `version/plugin-version.json` into `AppsFlyerPluginMetadata.generated.h`.

Fetching and pinning the JSON contract: [`../../schema/README.md`](../../schema/README.md).