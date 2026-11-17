#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SCHEMA="${ROOT}/schema/appsflyer-plugins-rpc-schema.json"
GEN_HDR="${ROOT}/Classes/AppsFlyer/generated/AppsFlyerRpcSchema.generated.h"
GEN_CPP="${ROOT}/Classes/AppsFlyer/generated/AppsFlyerRpcSchema.generated.cpp"
GEN_IMPL="${GEN_HDR}"

if [[ ! -f "${SCHEMA}" || ! -f "${GEN_HDR}" || ! -f "${GEN_CPP}" ]]; then
  echo "Missing schema or generated outputs (run ./scripts/codegen/run.sh first)" >&2
  exit 1
fi

python3 - "${SCHEMA}" "${GEN_HDR}" "${GEN_CPP}" <<'PY'
import json, os, re, sys

schema_path, gen_hdr_path, gen_cpp_path = sys.argv[1:4]
with open(schema_path, encoding="utf-8") as f:
    schema = json.load(f)

def cocos_public(m):
    plugins = m.get("plugins") or {}
    cocos = plugins.get("cocos2dx")
    if cocos == "internal" or cocos == "none":
        return False
    if cocos == "public":
        return True
    default = plugins.get("default")
    if default in ("internal", "none"):
        return False
    pub = (m.get("publicApi") or {})
    if pub.get("delivery") not in (None, "awaitable"):
        return False
    return True

expected = sorted(
    m["name"]
    for m in schema.get("methods", [])
    if cocos_public(m) and not m.get("isListener")
)

with open(gen_hdr_path, encoding="utf-8") as f:
    gen = f.read()
with open(gen_cpp_path, encoding="utf-8") as f:
    gen_cpp = f.read()

found = sorted(set(re.findall(r"AppsFlyerTask (\w+)\(", gen)))

missing = sorted(set(expected) - set(found))
extra = sorted(set(found) - set(expected))

if missing or extra:
    if missing:
        print("Schema methods missing from AppsFlyerX codegen:", ", ".join(missing), file=sys.stderr)
    if extra:
        print("Codegen methods not in schema cocos public set:", ", ".join(extra), file=sys.stderr)
    sys.exit(1)

if "enum class MediationNetwork" in gen:
    for m in schema.get("methods", []):
        if m.get("name") != "logAdRevenue":
            continue
        enums = (
            m.get("publicApi", {})
            .get("parameters", {})
            .get("properties", {})
            .get("mediationNetwork", {})
            .get("enum", [])
        )
        for key in enums:
            pascal = key[0].upper() + key[1:] if key else key
            if pascal not in gen or ('"%s"' % key) not in gen_cpp:
                print("MediationNetwork schema key missing from generated enum:", key, file=sys.stderr)
                sys.exit(1)
        break

print("Schema/codegen parity OK (%d methods)" % len(expected))
PY