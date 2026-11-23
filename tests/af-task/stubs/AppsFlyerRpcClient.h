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

inline RpcResult executeBlocking(const std::string&, const std::string&) {
    RpcResult r;
    r.success = false;
    r.errorMessage = "stub";
    return r;
}

using RpcEventHandler = std::function<void(const std::string& eventJson)>;

inline void setRpcEventHandler(RpcEventHandler) {}

} // namespace rpc
} // namespace appsflyer

#endif
