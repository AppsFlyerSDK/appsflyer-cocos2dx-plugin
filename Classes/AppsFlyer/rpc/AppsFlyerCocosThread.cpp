#include "AppsFlyerCocosThread.h"

#include "cocos2d.h"

namespace appsflyer {
namespace cocos {

void runOnGameThread(const std::function<void()>& fn) {
    if (!fn) {
        return;
    }
    auto director = cocos2d::Director::getInstance();
    if (director == nullptr || director->getScheduler() == nullptr) {
        return;
    }
    director->getScheduler()->performFunctionInCocosThread([fn]() { fn(); });
}

} // namespace cocos
} // namespace appsflyer
