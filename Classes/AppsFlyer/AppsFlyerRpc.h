#ifndef AppsFlyerRpc_h
#define AppsFlyerRpc_h

#include "AppsFlyerTask.h"
#include "AppsFlyerDeepLinkData.h"

#include "cocos2d.h"

#include <functional>
#include <string>
#include <vector>

// RPC bridge (AppsFlyerRpc.cpp):
//   appsflyer         — executeRpc + listener registration (schema / generated entry points)
//   appsflyer::detail — JSON helpers + test hooks (used by generated param encoding)
//   appsflyer::rpc    — synchronous native transport and inbound rpcEvent (bridges + unit tests)

namespace appsflyer {

namespace rpc {

struct RpcResult {
    bool success = false;
    std::string dataJson;
    int errorCode = 0;
    std::string errorMessage;
};

/// Synchronous RPC call into the Kotlin/Swift bridge (must not run on cocos GL thread for blocking ops).
RpcResult executeBlocking(const std::string& method, const std::string& paramsJsonObject);

using RpcEventHandler = std::function<void(const std::string& eventJson)>;

void setRpcEventHandler(RpcEventHandler handler);
void onNativeRpcEventFromBridge(const std::string& eventJson);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
std::string executeRpcBlockingRequest(const std::string& requestJson);
#endif

} // namespace rpc

AppsFlyerTask executeRpc(const std::string& wireMethod, const std::string& paramsJsonObject,
                         AppsFlyerCompletion onComplete = {});

namespace detail {

using GameThreadPoster = std::function<void(const std::function<void()>&)>;
using RpcExecutor = std::function<rpc::RpcResult(const std::string& method, const std::string& paramsJson)>;

void setGameThreadPosterForTests(GameThreadPoster poster);
void setRpcExecutorForTests(RpcExecutor executor);
void resetTestHooks();

std::string escapeJsonString(const std::string& value);
std::string encodeStringArrayJson(const std::vector<std::string>& values);
std::string encodeValueMapJson(const cocos2d::ValueMap& map);

cocos2d::ValueMap decodeJsonObjectToValueMap(const std::string& jsonObject);

std::string extractJsonObjectField(const std::string& json, const char* fieldName);

AppsFlyerTask unsupportedPlatformTask(AppsFlyerCompletion onComplete);

} // namespace detail

#if defined(AF_RPC_HEADLESS_TEST)
void setDeepLinkCallback(void (*callback)(DeepLinkData));
std::function<void(DeepLinkData)> deepLinkHandlerSnapshot();
#endif

} // namespace appsflyer

#endif