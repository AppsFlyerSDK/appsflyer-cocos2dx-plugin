#ifndef AppsFlyerCocosThread_h
#define AppsFlyerCocosThread_h

#include <functional>

namespace appsflyer {
namespace cocos {

/// Runs [fn] on the cocos main thread (Director scheduler).
void runOnGameThread(const std::function<void()>& fn);

} // namespace cocos
} // namespace appsflyer

#endif
