<img src="https://massets.appsflyer.com/wp-content/uploads/2018/06/20092440/static-ziv_1TP.png"  width="400" > 

# Cocos2dX AppsFlyer plugin for Android and iOS.

> **SDK 7 (RPC):** [Start here](#start-here) · **API** [`docs/API.md`](docs/API.md) · **iOS** [`docs/iOS-README.md`](docs/iOS-README.md) · Init order [`docs/SDK7-Lifecycle.md`](docs/SDK7-Lifecycle.md) · Migration [`docs/Migration-Guide.md`](docs/Migration-Guide.md)

----------

🛠 In order for us to provide optimal support, please contact AppsFlyer support through the Customer Assistant Chatbot for assistance with troubleshooting issues or product guidance. </br>
To do so, please follow [this article](https://support.appsflyer.com/hc/en-us/articles/23583984402193-Using-the-Customer-Assistant-Chatbot)

----------

## Table of contents

- [Plugin versions](#plugin-build-for)
- [Start here](#start-here)
- [Maintainers](#maintainers)

### <a id="plugin-build-for"> This plugin is built for

- Android: **af-android-sdk BOM 7.0.4**, **af-android-plugin-bridge 7.0.13** — **minSdk ≥ API 21** ([`version/android-sdk-versions.json`](version/android-sdk-versions.json))
- iOS: **AppsFlyerFramework 7.0.4**, **AppsFlyerRPC 7.0.41** (SwiftPM) — **deployment target ≥ `minimumIos`** ([`version/ios-spm-versions.json`](version/ios-spm-versions.json); see [docs/Migration-Guide.md](docs/Migration-Guide.md))

## <a id="start-here"> Start here

| If you need… | Read |
| --- | --- |
| **Migrate from plugin 6.x → SDK 7** (packaging + API) | [docs/Migration-Guide.md](docs/Migration-Guide.md) |
| **iOS integration guide** | [docs/iOS-README.md](docs/iOS-README.md) ([integration checklist](docs/iOS-README.md#integration-checklist)) |
| **Android integration guide** | [docs/Android-README.md](docs/Android-README.md) ([checklist](docs/Android-README.md#integration-checklist)) |
| **When to call init / listeners** | [docs/SDK7-Lifecycle.md](docs/SDK7-Lifecycle.md) |
| **API reference** | [docs/API.md](docs/API.md) |

## <a id="maintainers"> Maintainers

- RPC schema: [`schema/README.md`](schema/README.md)
- Codegen: [`tools/af-schema-compiler/README.md`](tools/af-schema-compiler/README.md)