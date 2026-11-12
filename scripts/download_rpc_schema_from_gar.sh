#!/usr/bin/env bash
set -euo pipefail

# Downloads appsflyer-plugins-rpc-schema.json from GAR maven-internal.
# Source: appsflyer-android-sdk/scripts/download_rpc_schema_from_gar.sh
#
# Required env:
#   AR_TOKEN - OAuth access token (CI: from appsflyerrnd/setup-ar-auth)
#
# Optional env:
#   RPC_SCHEMA_VERSION - semver or "latest" (default: latest)
#   OUTPUT_PATH        - default: schema/appsflyer-plugins-rpc-schema.json

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUTPUT_PATH="${OUTPUT_PATH:-${REPO_ROOT}/schema/appsflyer-plugins-rpc-schema.json}"
RPC_SCHEMA_VERSION="${RPC_SCHEMA_VERSION:-latest}"
GAR_PROJECT="${GAR_PROJECT:-af-artifacts-prd}"
GAR_LOCATION="${GAR_LOCATION:-europe-west1}"
GAR_REPOSITORY="${GAR_REPOSITORY:-maven-internal}"
MAVEN_GROUP_ID="${MAVEN_GROUP_ID:-com.appsflyer.plugins}"
MAVEN_ARTIFACT_ID="${MAVEN_ARTIFACT_ID:-appsflyer-plugins-rpc-schema}"

if [ -z "${AR_TOKEN:-}" ]; then
  echo "AR_TOKEN is required" >&2
  exit 1
fi

GROUP_PATH="$(printf '%s' "$MAVEN_GROUP_ID" | tr '.' '/')"
REPO_BASE="https://${GAR_LOCATION}-maven.pkg.dev/${GAR_PROJECT}/${GAR_REPOSITORY}"
AUTH=( -u "oauth2accesstoken:${AR_TOKEN}" )

artifact_url() {
  local version="$1"
  echo "${REPO_BASE}/${GROUP_PATH}/${MAVEN_ARTIFACT_ID}/${version}/${MAVEN_ARTIFACT_ID}-${version}.json"
}

resolve_latest_version() {
  local metadata_url="${REPO_BASE}/${GROUP_PATH}/${MAVEN_ARTIFACT_ID}/maven-metadata.xml"
  local metadata
  metadata="$(curl -fsSL "${AUTH[@]}" "$metadata_url")"
  local version
  version="$(printf '%s' "$metadata" | sed -n 's:.*<latest>\([^<]*\)</latest>.*:\1:p' | head -n 1)"
  if [ -z "$version" ]; then
    version="$(printf '%s' "$metadata" | sed -n 's:.*<release>\([^<]*\)</release>.*:\1:p' | head -n 1)"
  fi
  printf '%s' "$version"
}

VERSION="$RPC_SCHEMA_VERSION"
if [ "$VERSION" = "latest" ]; then
  VERSION="$(resolve_latest_version)"
  if [ -z "$VERSION" ]; then
    echo "No published versions found for ${MAVEN_GROUP_ID}:${MAVEN_ARTIFACT_ID}" >&2
    exit 1
  fi
fi

mkdir -p "$(dirname "$OUTPUT_PATH")"
URL="$(artifact_url "$VERSION")"
echo "Downloading ${MAVEN_GROUP_ID}:${MAVEN_ARTIFACT_ID}:${VERSION}"
curl -fsSL "${AUTH[@]}" "$URL" -o "$OUTPUT_PATH"
echo "Saved RPC schema (${VERSION}) to ${OUTPUT_PATH}"
