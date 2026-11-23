#!/usr/bin/env bash
# Generate and build the CMake iOS app (device). Requires cocos2d/ + external deps.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="${ROOT}/build-device"
cmake -B "${BUILD_DIR}" -GXcode \
  -DCMAKE_SYSTEM_NAME=iOS \
  -DCMAKE_OSX_SYSROOT=iphoneos \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=15.6

# CMake regenerates project.pbxproj (drops SPM + re-enables legacy build dirs).
"${ROOT}/scripts/setup-ios-spm.sh" "${BUILD_DIR}"

DERIVED_DATA="${BUILD_DIR}/DerivedData"
mkdir -p "${DERIVED_DATA}"

xcodebuild -project "${BUILD_DIR}/AppsFlyerCocos2dX.xcodeproj" \
  -scheme AppsFlyerCocos2dX \
  -configuration Debug \
  -sdk iphoneos \
  -destination 'generic/platform=iOS' \
  -derivedDataPath "${DERIVED_DATA}" \
  -clonedSourcePackagesDirPath "${BUILD_DIR}/SourcePackages" \
  "$@"