# iOS sample sources (`proj.ios_mac`)

Runtime and UI sources for the sample app live under **`ios/`** (e.g. `AppController.mm`, `SwiftBridge.swift`, `ios/rpc/`).

There is **no** checked-in `.xcodeproj` here. To build the sample:

1. From the repo root, follow **Case A** in [`docs/iOS-README.md`](../docs/iOS-README.md) (`cmake` + `setup-ios-spm.sh` or `./scripts/build-ios-cmake.sh`).
2. For init order and the C++ startup chain, see [`docs/SDK7-Lifecycle.md`](../docs/SDK7-Lifecycle.md).