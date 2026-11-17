#include "AppsFlyer/rpc/AppsFlyerCocosThread.h" // test stub header (sync-game-thread noop)

namespace appsflyer {
namespace cocos {

void runOnGameThread(const std::function<void()>& fn) {
    if (fn) {
        fn();
    }
}

} // namespace cocos
} // namespace appsflyer
