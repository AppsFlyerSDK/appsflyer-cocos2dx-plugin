#ifndef cocos2d_h_sdk7_test_stub
#define cocos2d_h_sdk7_test_stub

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace cocos2d {

class Value {
public:
    Value() = default;
    explicit Value(const std::string& s) : str_(s) {}
    std::string asString() const { return str_; }

private:
    std::string str_;
};

using ValueMap = std::unordered_map<std::string, Value>;
using ValueVector = std::vector<Value>;

class Director {
public:
    static Director* getInstance() { return &instance(); }
    class Scheduler {
    public:
        void performFunctionInCocosThread(const std::function<void()>&) {}
    };
    Scheduler* getScheduler() { return &scheduler_; }

private:
    static Director& instance() {
        static Director d;
        return d;
    }
    Scheduler scheduler_;
};

#define CCLOG(...) ((void)0)
#define CCLOGWARN(...) ((void)0)

} // namespace cocos2d

#ifndef CC_TARGET_PLATFORM
#define CC_TARGET_PLATFORM CC_PLATFORM_ANDROID
#endif

#ifndef CC_PLATFORM_ANDROID
#define CC_PLATFORM_ANDROID 3
#endif

#endif
