#!/usr/bin/env bash
# Installs CocoaPods deps for iOS SDK 7 RPC into the CMake build directory.
# Usage:
#   ./scripts/setup-ios-rpc-pods.sh build-device
#   ./scripts/setup-ios-rpc-pods.sh build-ios-rpc
#   ./scripts/setup-ios-rpc-pods.sh build-device --fresh
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="${1:-${ROOT}/build}"
FRESH=false
if [[ "${2:-}" == "--fresh" ]]; then
  FRESH=true
fi

if [[ ! -f "${BUILD_DIR}/AppsFlyerCocos2dX.xcodeproj/project.pbxproj" ]]; then
  echo "Missing ${BUILD_DIR}/AppsFlyerCocos2dX.xcodeproj — run CMake for iOS first." >&2
  exit 1
fi

PBXPROJ="${BUILD_DIR}/AppsFlyerCocos2dX.xcodeproj/project.pbxproj"
if grep -q 'SDKROOT = macosx' "${PBXPROJ}" 2>/dev/null; then
  echo "macOS desktop is not supported in SDK 7 — use an iOS CMake build dir (build-ios-rpc / build-device)." >&2
  exit 1
fi

cp "${ROOT}/Podfile" "${BUILD_DIR}/Podfile"
cd "${BUILD_DIR}"

if [[ "${FRESH}" == true ]]; then
  rm -rf Pods Podfile.lock AppsFlyerCocos2dX.xcworkspace
fi

POD_INSTALL_FLAGS=(install --repo-update)
if [[ "${AF_COCOAPODS_SKIP_REPO_UPDATE:-}" == "1" ]]; then
  POD_INSTALL_FLAGS=(install)
fi

pod "${POD_INSTALL_FLAGS[@]}"
echo "Open ${BUILD_DIR}/AppsFlyerCocos2dX.xcworkspace in Xcode (not .xcodeproj)."
