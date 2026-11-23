#ifndef cocos2d_h_sdk7_test_stub
#define cocos2d_h_sdk7_test_stub

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace cocos2d {

class Value;

using ValueMap = std::unordered_map<std::string, Value>;
using ValueVector = std::vector<Value>;

class Value {
public:
    enum class Type { STRING, BOOLEAN, INTEGER, FLOAT, DOUBLE, MAP, VECTOR, NULLTYPE };

    Value() : type_(Type::NULLTYPE) {}
    explicit Value(const std::string& s) : type_(Type::STRING), str_(s) {}
    explicit Value(bool b) : type_(Type::BOOLEAN), bool_(b) {}
    explicit Value(int i) : type_(Type::INTEGER), int_(i) {}
    explicit Value(double d) : type_(Type::DOUBLE), dbl_(d) {}
    explicit Value(const ValueMap& map) : type_(Type::MAP), map_(map) {}
    explicit Value(const ValueVector& vec) : type_(Type::VECTOR), vec_(vec) {}
    explicit Value(ValueMap&& map) : type_(Type::MAP), map_(std::move(map)) {}
    explicit Value(ValueVector&& vec) : type_(Type::VECTOR), vec_(std::move(vec)) {}

    Type getType() const { return type_; }
    bool isNull() const { return type_ == Type::NULLTYPE; }
    std::string asString() const { return type_ == Type::STRING ? str_ : std::string(); }
    bool asBool() const { return bool_; }
    int asInt() const { return int_; }
    double asDouble() const { return dbl_; }
    const ValueMap& asValueMap() const { return map_; }
    const ValueVector& asValueVector() const { return vec_; }

private:
    Type type_ = Type::NULLTYPE;
    std::string str_;
    bool bool_ = false;
    int int_ = 0;
    double dbl_ = 0;
    ValueMap map_;
    ValueVector vec_;
};

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