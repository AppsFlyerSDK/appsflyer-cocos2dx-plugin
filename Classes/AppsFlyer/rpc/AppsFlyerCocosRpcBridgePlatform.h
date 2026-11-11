#pragma once

#include <string>

namespace appsflyer {
namespace rpc {

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
std::string executeRpcBlockingRequest(const std::string& requestJson);
#endif

void onNativeRpcEventFromBridge(const std::string& eventJson);

} // namespace rpc
} // namespace appsflyer
