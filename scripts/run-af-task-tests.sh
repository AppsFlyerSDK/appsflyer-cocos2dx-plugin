#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT/tests/af-task"
rm -rf build
cmake -B build
cmake --build build
./build/appsflyer_task_tests
