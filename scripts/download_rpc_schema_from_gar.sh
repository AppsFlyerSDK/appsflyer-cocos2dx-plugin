#!/usr/bin/env bash
set -euo pipefail

# Downloads com.appsflyer.plugins:appsflyer-plugins-rpc-schema from GAR maven-internal.
#
# Auth (local): set AR_TOKEN, or run `gcloud auth login` so this script can call
# `gcloud auth print-access-token`.
#
# Version: RPC_SCHEMA_VERSION (default: latest). Set a semver to fetch a specific release.
#
# Optional: schema/gar.local.env (gitignored) may export AR_TOKEN or RPC_SCHEMA_VERSION.

usage() {
  cat <<'EOF'
Usage: ./scripts/download_rpc_schema_from_gar.sh

Fetches appsflyer-plugins-rpc-schema.json from Artifact Registry Maven into schema/.

Uses Maven "latest" when RPC_SCHEMA_VERSION is unset. Requires gcloud login or AR_TOKEN.
EOF
}

if [[ "${1:-}" == "-h" || "${1:-}" == "--help" ]]; then
  usage
  exit 0
fi

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
LOCAL_ENV="${REPO_ROOT}/schema/gar.local.env"
if [[ -f "${LOCAL_ENV}" ]]; then
  set -a
  # shellcheck disable=SC1090
  source "${LOCAL_ENV}"
  set +a
fi

OUTPUT_PATH="${OUTPUT_PATH:-${REPO_ROOT}/schema/appsflyer-plugins-rpc-schema.json}"
GAR_PROJECT="${GAR_PROJECT:-af-artifacts-prd}"
GAR_LOCATION="${GAR_LOCATION:-europe-west1}"
GAR_REPOSITORY="${GAR_REPOSITORY:-maven-internal}"
MAVEN_GROUP_ID="${MAVEN_GROUP_ID:-com.appsflyer.plugins}"
MAVEN_ARTIFACT_ID="${MAVEN_ARTIFACT_ID:-appsflyer-plugins-rpc-schema}"
RPC_SCHEMA_VERSION="${RPC_SCHEMA_VERSION:-latest}"

resolve_ar_token() {
  if [[ -n "${AR_TOKEN:-}" ]]; then
    return 0
  fi
  if command -v gcloud >/dev/null 2>&1; then
    local token
    if token="$(gcloud auth print-access-token 2>/dev/null)" && [[ -n "${token}" ]]; then
      AR_TOKEN="${token}"
      export AR_TOKEN
      return 0
    fi
  fi
  echo "AR_TOKEN is not set and gcloud could not provide an access token." >&2
  echo "Run: gcloud auth login   (or export AR_TOKEN from your CI auth step)" >&2
  exit 1
}

resolve_ar_token

GROUP_PATH="$(printf '%s' "${MAVEN_GROUP_ID}" | tr '.' '/')"
REPO_BASE="https://${GAR_LOCATION}-maven.pkg.dev/${GAR_PROJECT}/${GAR_REPOSITORY}"
AUTH=( -u "oauth2accesstoken:${AR_TOKEN}" )

artifact_url() {
  local version="$1"
  echo "${REPO_BASE}/${GROUP_PATH}/${MAVEN_ARTIFACT_ID}/${version}/${MAVEN_ARTIFACT_ID}-${version}.json"
}

resolve_latest_version() {
  local metadata_url="${REPO_BASE}/${GROUP_PATH}/${MAVEN_ARTIFACT_ID}/maven-metadata.xml"
  local metadata
  if ! metadata="$(curl -fsSL "${AUTH[@]}" "${metadata_url}")"; then
    echo "Failed to fetch maven-metadata.xml (check registry access for ${GAR_PROJECT}/${GAR_REPOSITORY})." >&2
    exit 1
  fi
  local version
  version="$(printf '%s' "${metadata}" | sed -n 's:.*<latest>\([^<]*\)</latest>.*:\1:p' | head -n 1)"
  if [[ -z "${version}" ]]; then
    version="$(printf '%s' "${metadata}" | sed -n 's:.*<release>\([^<]*\)</release>.*:\1:p' | head -n 1)"
  fi
  printf '%s' "${version}"
}

VERSION="${RPC_SCHEMA_VERSION}"
if [[ "${VERSION}" == "latest" ]]; then
  VERSION="$(resolve_latest_version)"
  if [[ -z "${VERSION}" ]]; then
    echo "No published versions found for ${MAVEN_GROUP_ID}:${MAVEN_ARTIFACT_ID}" >&2
    exit 1
  fi
fi

mkdir -p "$(dirname "${OUTPUT_PATH}")"
URL="$(artifact_url "${VERSION}")"
echo "Downloading ${MAVEN_GROUP_ID}:${MAVEN_ARTIFACT_ID}:${VERSION}"
if ! curl -fsSL "${AUTH[@]}" "${URL}" -o "${OUTPUT_PATH}"; then
  echo "Download failed for ${URL}" >&2
  exit 1
fi

python3 - "${OUTPUT_PATH}" "${VERSION}" <<'PY'
import json, sys
path, expected_maven = sys.argv[1:3]
with open(path, encoding="utf-8") as f:
    data = json.load(f)
if not isinstance(data, dict) or "methods" not in data:
    print("Downloaded file is not a valid RPC schema JSON", file=sys.stderr)
    sys.exit(1)
schema_ver = (data.get("schemaVersion") or "").strip()
if schema_ver and schema_ver != expected_maven:
    print(
        f"Warning: Maven coordinate version {expected_maven!r} != JSON schemaVersion {schema_ver!r}",
        file=sys.stderr,
    )
print(f"Saved RPC schema (Maven {expected_maven}, schemaVersion {schema_ver or 'n/a'}) to {path}")
PY