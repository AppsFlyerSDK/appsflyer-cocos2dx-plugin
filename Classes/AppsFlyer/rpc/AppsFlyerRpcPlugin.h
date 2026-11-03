#ifndef AppsFlyerRpcPlugin_h
#define AppsFlyerRpcPlugin_h

#include "AppsFlyerRpcTransport.h"

#include "../AppsFlyerXDeepLinkResult.h"

#include <functional>

namespace appsflyer {

#if defined(AF_RPC_HEADLESS_TEST)
void setDeepLinkCallback(void (*callback)(AppsFlyerXDeepLinkResult));
std::function<void(AppsFlyerXDeepLinkResult)> deepLinkHandlerSnapshot();
#endif

} // namespace appsflyer

#endif
