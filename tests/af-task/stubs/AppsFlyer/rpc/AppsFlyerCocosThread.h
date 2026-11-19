#ifndef AppsFlyerCocosThread_test_stub
#define AppsFlyerCocosThread_test_stub

#include <functional>

namespace appsflyer {
namespace cocos {

void runOnGameThread(const std::function<void()>& fn);

} // namespace cocos
} // namespace appsflyer

#endif
