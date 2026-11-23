#ifndef AppsFlyerRpcClient_test_stub
#define AppsFlyerRpcClient_test_stub

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

RpcResult executeBlocking(const std::string& method, const std::string& paramsJson);

using RpcEventHandler = std::function<void(const std::string& eventJson)>;

void setRpcEventHandler(RpcEventHandler handler);

} // namespace rpc
} // namespace appsflyer

#endif
