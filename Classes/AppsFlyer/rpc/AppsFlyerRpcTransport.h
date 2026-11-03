#ifndef AppsFlyerRpcTransport_h
#define AppsFlyerRpcTransport_h

#include "../AppsFlyerTask.h"

#include "cocos2d.h"

#include <functional>
#include <string>
#include <vector>

namespace appsflyer {
namespace rpc {

struct RpcResult {
    bool success = false;
    std::string dataJson;
    int errorCode = 0;
    std::string errorMessage;
};

RpcResult executeBlocking(const std::string& method, const std::string& paramsJsonObject);

using RpcEventHandler = std::function<void(const std::string& eventJson)>;

void setRpcEventHandler(RpcEventHandler handler);
void onNativeRpcEventFromBridge(const std::string& eventJson);

#if defined(__ANDROID__)
void onNativeRpcEventFromJni(const std::string& eventJson);
#endif

} // namespace rpc

AppsFlyerTask executeRpc(const std::string& wireMethod, const std::string& paramsJsonObject,
                         AppsFlyerCompletion onComplete = {});

AppsFlyerTask registerListenerRpc(const std::string& wireMethod,
                                  const std::string& paramsJsonObject,
                                  const std::function<void()>& installListener,
                                  const std::function<void()>& restoreListener,
                                  AppsFlyerCompletion onComplete = {});

namespace detail {

using GameThreadPoster = std::function<void(const std::function<void()>&)>;
using RpcExecutor = std::function<rpc::RpcResult(const std::string& method, const std::string& paramsJson)>;

void postCompletionOnGameThread(const std::function<void()>& fn);

void setGameThreadPosterForTests(GameThreadPoster poster);
void setRpcExecutorForTests(RpcExecutor executor);
void resetTestHooks();

std::string escapeJsonString(const std::string& value);
std::string encodeStringArrayJson(const std::vector<std::string>& values);
std::string encodeValueMapJson(const cocos2d::ValueMap& map);

AppsFlyerTask unsupportedPlatformTask(AppsFlyerCompletion onComplete);

} // namespace detail

} // namespace appsflyer

#endif
