#!/usr/bin/env bash
# Wire AppsFlyer iOS RPC + SDK via Swift Package Manager into a CMake-generated Xcode project.
#
# Usage:
#   ./scripts/setup-ios-spm.sh build-device
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="${1:-${ROOT}/build-device}"

if [[ ! -f "${BUILD_DIR}/AppsFlyerCocos2dX.xcodeproj/project.pbxproj" ]]; then
  echo "Missing ${BUILD_DIR}/AppsFlyerCocos2dX.xcodeproj — run CMake for iOS first." >&2
  echo "  cmake -B ${BUILD_DIR} -GXcode -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_SYSROOT=iphoneos -DCMAKE_OSX_DEPLOYMENT_TARGET=15.6" >&2
  exit 1
fi

PBXPROJ="${BUILD_DIR}/AppsFlyerCocos2dX.xcodeproj/project.pbxproj"
if grep -q 'SDKROOT = macosx' "${PBXPROJ}" 2>/dev/null; then
  echo "macOS desktop is not supported for SDK 7 iOS RPC." >&2
  exit 1
fi

python3 "${ROOT}/scripts/ios/inject_spm_into_xcodeproj.py" "${BUILD_DIR}/AppsFlyerCocos2dX.xcodeproj"

DERIVED_DATA="${BUILD_DIR}/DerivedData"
mkdir -p "${DERIVED_DATA}"

echo "Resolving Swift packages (requires network)…"
xcodebuild -project "${BUILD_DIR}/AppsFlyerCocos2dX.xcodeproj" \
  -scheme AppsFlyerCocos2dX \
  -resolvePackageDependencies \
  -derivedDataPath "${DERIVED_DATA}" \
  -clonedSourcePackagesDirPath "${BUILD_DIR}/SourcePackages"

echo "Open ${BUILD_DIR}/AppsFlyerCocos2dX.xcodeproj in Xcode (not .xcworkspace)."