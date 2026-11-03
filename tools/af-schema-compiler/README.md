# af-schema-compiler

Build-time C++ tool that reads `schema/appsflyer-plugins-rpc-schema.json` and writes
`Classes/AppsFlyer/generated/*`.

```bash
# From repo root
./scripts/codegen/run.sh
```

Requires CMake 3.16+, a C++17 compiler, and network on first configure (FetchContent for nlohmann/json).
