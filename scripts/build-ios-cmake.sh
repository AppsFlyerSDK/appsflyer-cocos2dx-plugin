#!/usr/bin/env bash
# Generate and build the CMake iOS app (device). Requires cocos2d/ + external deps.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="${ROOT}/build-device"
cmake -B "${BUILD_DIR}" -GXcode \
  -DCMAKE_SYSTEM_NAME=iOS \
  -DCMAKE_OSX_SYSROOT=iphoneos \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=15.6
if [[ ! -f "${BUILD_DIR}/AppsFlyerCocos2dX.xcworkspace/contents.xcworkspacedata" ]]; then
  echo "Missing ${BUILD_DIR}/AppsFlyerCocos2dX.xcworkspace — run ./scripts/setup-ios-rpc-pods.sh build-device first." >&2
  exit 1
fi

xcodebuild -workspace "${BUILD_DIR}/AppsFlyerCocos2dX.xcworkspace" \
  -scheme AppsFlyerCocos2dX \
  -configuration Debug \
  -sdk iphoneos \
  -destination 'generic/platform=iOS' \
  "$@"
