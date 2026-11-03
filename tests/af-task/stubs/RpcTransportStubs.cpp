#include "AppsFlyer/rpc/AppsFlyerCocosThread.h"

namespace appsflyer {
namespace cocos {

void runOnGameThread(const std::function<void()>& fn) {
    if (fn) {
        fn();
    }
}

} // namespace cocos
} // namespace appsflyer
