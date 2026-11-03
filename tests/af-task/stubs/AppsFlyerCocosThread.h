#ifndef AppsFlyerCocosThread_test_stub
#define AppsFlyerCocosThread_test_stub

#include <functional>

namespace appsflyer {
namespace cocos {

inline void runOnGameThread(const std::function<void()>& fn) {
    if (fn) {
        fn();
    }
}

} // namespace cocos
} // namespace appsflyer

#endif
