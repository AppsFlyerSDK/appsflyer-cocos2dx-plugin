#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_DIR="${ROOT}/tools/af-schema-compiler/build"
BIN="${BUILD_DIR}/af-schema-compiler"

if [[ ! -x "${BIN}" ]]; then
  cmake -S "${ROOT}/tools/af-schema-compiler" -B "${BUILD_DIR}"
fi
cmake --build "${BUILD_DIR}" --config Release

"${BIN}" "${ROOT}"
