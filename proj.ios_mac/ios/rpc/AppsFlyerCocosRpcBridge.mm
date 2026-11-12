#include "AppsFlyerCocosRpcBridge.h"

#include "AppsFlyer/rpc/AppsFlyerCocosRpcBridgePlatform.h"

#import <Foundation/Foundation.h>

#if __has_include("AppsFlyerCocos2dX-Swift.h")
#import "AppsFlyerCocos2dX-Swift.h"
#endif

void AppsFlyerCocosRpcNativeDeliverEvent(const char* eventJson) {
    if (eventJson == nullptr) {
        return;
    }
    appsflyer::rpc::onNativeRpcEventFromBridge(std::string(eventJson));
}

namespace appsflyer {
namespace rpc {

std::string executeRpcBlockingRequest(const std::string& requestJson) {
#if __has_include("AppsFlyerCocos2dX-Swift.h")
    NSString* req = [NSString stringWithUTF8String:requestJson.c_str()];
    NSString* resp = [AppsFlyerCocosRpcBridge executeRpcBlocking:req];
    if (resp == nil) {
        return "{\"success\":false,\"error\":{\"code\":500,\"message\":\"Empty RPC response\"}}";
    }
    return std::string([resp UTF8String]);
#else
    (void)requestJson;
    return "{\"success\":false,\"error\":{\"code\":501,\"message\":\"Swift RPC bridge not linked\"}}";
#endif
}

} // namespace rpc
} // namespace appsflyer
