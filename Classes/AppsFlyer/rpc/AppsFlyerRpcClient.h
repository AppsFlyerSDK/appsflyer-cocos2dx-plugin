#ifndef AppsFlyerRpcClient_h
#define AppsFlyerRpcClient_h

#include <functional>
#include <string>

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

#if defined(__ANDROID__)
void onNativeRpcEventFromJni(const std::string& eventJson);
#endif

} // namespace rpc
} // namespace appsflyer

#endif
