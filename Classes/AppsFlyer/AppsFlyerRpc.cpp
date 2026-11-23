#include "AppsFlyerRpc.h"

#include "AppsFlyer/AppsFlyerTaskState.h"
#include "AppsFlyer/generated/AppsFlyerPluginMetadata.generated.h"

#include "cocos2d.h"

#include <atomic>
#include <cctype>
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <exception>
#include <functional>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <utility>

#if defined(AF_RPC_HEADLESS_TEST)

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#endif

// AppsFlyerRpc.cpp — Cocos ↔ native JSON-RPC transport (see AppsFlyerRpc.h for namespace map).

namespace appsflyer {

namespace detail {
void postOnGameThread(const std::function<void()>& fn);
AppsFlyerResult toAppsFlyerResult(const rpc::RpcResult& rpc);
rpc::RpcResult runRpc(const std::string& method, const std::string& paramsJson);
} // namespace detail

namespace {

bool isInitWireMethod(const std::string& wireMethod) {
    return wireMethod == "init" || wireMethod == "initialize";
}

std::string buildSetPluginInfoParamsJson() {
    return std::string(R"({"plugin":")") + kAppsFlyerPluginId + R"(","pluginVersion":")" +
           detail::escapeJsonString(kAppsFlyerPluginVersion) + R"(","additionalParams":{}})";
}

AppsFlyerTask registerListenerRpc(const std::string& wireMethod, const std::string& paramsJsonObject,
                                const std::function<void()>& installListener,
                                const std::function<void()>& restoreListener,
                                AppsFlyerCompletion onComplete) {
    installListener();

    auto state = std::make_shared<AppsFlyerTaskState>();
    AppsFlyerTask task(state);
    if (onComplete) {
        task.whenComplete(std::move(onComplete));
    }

    std::thread([state, wireMethod, paramsJsonObject, restoreListener]() {
        const rpc::RpcResult rpc = detail::runRpc(wireMethod, paramsJsonObject);
        const AppsFlyerResult result = detail::toAppsFlyerResult(rpc);
        detail::postOnGameThread([state, result, restoreListener]() {
            if (!result.success) {
                restoreListener();
            }
            state->finish(result);
        });
    }).detach();

    return task;
}

} // namespace

// -----------------------------------------------------------------------------
// appsflyer — public API (AppsFlyerRpcSchema.generated.cpp)
// -----------------------------------------------------------------------------

AppsFlyerTask executeRpc(const std::string& wireMethod, const std::string& paramsJsonObject,
                         AppsFlyerCompletion onComplete) {
    auto state = std::make_shared<AppsFlyerTaskState>();
    AppsFlyerTask task(state);
    if (onComplete) {
        task.whenComplete(std::move(onComplete));
    }

    std::thread([state, wireMethod, paramsJsonObject]() {
        if (isInitWireMethod(wireMethod)) {
            const rpc::RpcResult pluginInfo =
                detail::runRpc("setPluginInfo", buildSetPluginInfoParamsJson());
            if (!pluginInfo.success) {
                const AppsFlyerResult failed = detail::toAppsFlyerResult(pluginInfo);
                detail::postOnGameThread([state, failed]() { state->finish(failed); });
                return;
            }
        }
        const rpc::RpcResult rpc = detail::runRpc(wireMethod, paramsJsonObject);
        const AppsFlyerResult result = detail::toAppsFlyerResult(rpc);
        detail::postOnGameThread([state, result]() { state->finish(result); });
    }).detach();

    return task;
}


// -----------------------------------------------------------------------------
// appsflyer::detail — JSON, tasks, tests
// -----------------------------------------------------------------------------

namespace detail {

namespace {

std::atomic<bool> g_useTestGameThread{false};
GameThreadPoster g_testGameThreadPoster;
std::atomic<bool> g_useTestRpc{false};
RpcExecutor g_testRpcExecutor;

} // namespace

bool rpcTestHookActive() {
    return g_useTestRpc.load() && static_cast<bool>(g_testRpcExecutor);
}

rpc::RpcResult invokeRpcTestHook(const std::string& method, const std::string& paramsJson) {
    return g_testRpcExecutor(method, paramsJson);
}

#if !defined(AF_RPC_HEADLESS_TEST)
void postOnDirectorThread(const std::function<void()>& fn) {
    if (!fn) {
        return;
    }
    auto director = cocos2d::Director::getInstance();
    if (director == nullptr || director->getScheduler() == nullptr) {
        return;
    }
    director->getScheduler()->performFunctionInCocosThread([fn]() { fn(); });
}
#endif

void postOnGameThread(const std::function<void()>& fn) {
    if (g_useTestGameThread.load() && g_testGameThreadPoster) {
        g_testGameThreadPoster(fn);
        return;
    }
#if defined(AF_RPC_HEADLESS_TEST)
    if (fn) {
        fn();
    }
#else
    postOnDirectorThread(fn);
#endif
}

AppsFlyerResult toAppsFlyerResult(const rpc::RpcResult& rpc) {
    AppsFlyerResult result;
    result.success = rpc.success;
    result.dataJson = rpc.dataJson;
    if (!rpc.success) {
        result.hasError = true;
        result.error.code = rpc.errorCode != 0 ? rpc.errorCode : 500;
        result.error.message = rpc.errorMessage.empty() ? rpc.dataJson : rpc.errorMessage;
    }
    return result;
}

void setGameThreadPosterForTests(GameThreadPoster poster) {
    g_testGameThreadPoster = std::move(poster);
    g_useTestGameThread.store(static_cast<bool>(g_testGameThreadPoster));
}

void setRpcExecutorForTests(RpcExecutor executor) {
    g_testRpcExecutor = std::move(executor);
    g_useTestRpc.store(static_cast<bool>(g_testRpcExecutor));
}

void resetTestHooks() {
    g_useTestGameThread.store(false);
    g_useTestRpc.store(false);
    g_testGameThreadPoster = {};
    g_testRpcExecutor = {};
}

AppsFlyerTask unsupportedPlatformTask(AppsFlyerCompletion onComplete) {
    AppsFlyerResult result;
    result.success = false;
    result.hasError = true;
    result.error.code = 501;
    result.error.message = "RPC supported on Android/iOS only";
    auto state = std::make_shared<AppsFlyerTaskState>();
    AppsFlyerTask task(state);
    if (onComplete) {
        task.whenComplete(std::move(onComplete));
    }
    postOnGameThread([state, result]() { state->finish(result); });
    return task;
}

rpc::RpcResult runRpc(const std::string& method, const std::string& paramsJson) {
    return rpc::executeBlocking(method, paramsJson);
}



namespace {

size_t skipWs(const std::string& s, size_t i) {
    while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i])) != 0) {
        ++i;
    }
    return i;
}

std::string unescapeJsonString(const std::string& value) {
    std::string out;
    out.reserve(value.size());
    for (size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '\\' && i + 1 < value.size()) {
            const char next = value[i + 1];
            if (next == '"' || next == '\\' || next == '/') {
                out.push_back(next);
                ++i;
            } else if (next == 'n') {
                out.push_back('\n');
                ++i;
            } else if (next == 't') {
                out.push_back('\t');
                ++i;
            } else {
                out.push_back(value[i]);
            }
        } else {
            out.push_back(value[i]);
        }
    }
    return out;
}

size_t findMatchingBracket(const std::string& s, size_t openIndex) {
    if (openIndex >= s.size()) {
        return std::string::npos;
    }
    const char open = s[openIndex];
    char close = '\0';
    if (open == '{') {
        close = '}';
    } else if (open == '[') {
        close = ']';
    } else {
        return std::string::npos;
    }
    int depth = 0;
    bool inString = false;
    for (size_t i = openIndex; i < s.size(); ++i) {
        const char c = s[i];
        if (inString) {
            if (c == '\\' && i + 1 < s.size()) {
                ++i;
                continue;
            }
            if (c == '"') {
                inString = false;
            }
            continue;
        }
        if (c == '"') {
            inString = true;
            continue;
        }
        if (c == open) {
            ++depth;
        } else if (c == close) {
            --depth;
            if (depth == 0) {
                return i;
            }
        }
    }
    return std::string::npos;
}

size_t findMatchingBrace(const std::string& s, size_t openIndex) {
    if (openIndex >= s.size() || s[openIndex] != '{') {
        return std::string::npos;
    }
    return findMatchingBracket(s, openIndex);
}

bool parseJsonStringLiteral(const std::string& s, size_t& i, std::string& out) {
    if (i >= s.size() || s[i] != '"') {
        return false;
    }
    ++i;
    const size_t valStart = i;
    while (i < s.size() && s[i] != '"') {
        if (s[i] == '\\' && i + 1 < s.size()) {
            i += 2;
        } else {
            ++i;
        }
    }
    if (i >= s.size()) {
        return false;
    }
    out = unescapeJsonString(s.substr(valStart, i - valStart));
    ++i;
    return true;
}

cocos2d::Value valueFromJsonNumberToken(const std::string& num) {
    const char* str = num.c_str();
    char* end = nullptr;
    if (num.find_first_of(".eE") == std::string::npos) {
        errno = 0;
        const long long v = std::strtoll(str, &end, 10);
        if (errno == 0 && end != str && *end == '\0' && v >= INT_MIN && v <= INT_MAX) {
            return cocos2d::Value(static_cast<int>(v));
        }
    }
    errno = 0;
    end = nullptr;
    const double d = std::strtod(str, &end);
    if (errno == 0 && end != str && *end == '\0') {
        return cocos2d::Value(d);
    }
    return cocos2d::Value(num);
}

bool parseJsonValue(const std::string& s, size_t& i, cocos2d::Value& out);

bool parseJsonObjectContents(const std::string& s, size_t& i, cocos2d::ValueMap& map) {
    i = skipWs(s, i);
    if (i < s.size() && s[i] == '}') {
        ++i;
        return true;
    }
    while (i < s.size()) {
        std::string key;
        if (!parseJsonStringLiteral(s, i, key)) {
            return false;
        }
        i = skipWs(s, i);
        if (i >= s.size() || s[i] != ':') {
            return false;
        }
        ++i;
        cocos2d::Value val;
        if (!parseJsonValue(s, i, val)) {
            return false;
        }
        map[std::move(key)] = std::move(val);
        i = skipWs(s, i);
        if (i < s.size() && s[i] == ',') {
            ++i;
            continue;
        }
        if (i < s.size() && s[i] == '}') {
            ++i;
            return true;
        }
        return false;
    }
    return false;
}

bool parseJsonValue(const std::string& s, size_t& i, cocos2d::Value& out) {
    i = skipWs(s, i);
    if (i >= s.size()) {
        return false;
    }
    const char c = s[i];
    if (c == '"') {
        std::string str;
        if (!parseJsonStringLiteral(s, i, str)) {
            return false;
        }
        out = cocos2d::Value(std::move(str));
        return true;
    }
    if (c == '{') {
        cocos2d::ValueMap map;
        ++i;
        if (!parseJsonObjectContents(s, i, map)) {
            return false;
        }
        out = cocos2d::Value(std::move(map));
        return true;
    }
    if (c == '[') {
        cocos2d::ValueVector vec;
        ++i;
        i = skipWs(s, i);
        if (i < s.size() && s[i] == ']') {
            ++i;
            out = cocos2d::Value(std::move(vec));
            return true;
        }
        while (i < s.size()) {
            cocos2d::Value elem;
            if (!parseJsonValue(s, i, elem)) {
                return false;
            }
            vec.push_back(std::move(elem));
            i = skipWs(s, i);
            if (i < s.size() && s[i] == ',') {
                ++i;
                continue;
            }
            if (i < s.size() && s[i] == ']') {
                ++i;
                break;
            }
            return false;
        }
        out = cocos2d::Value(std::move(vec));
        return true;
    }
    if (c == 't' && i + 4 <= s.size() && s.compare(i, 4, "true") == 0) {
        i += 4;
        out = cocos2d::Value(true);
        return true;
    }
    if (c == 'f' && i + 5 <= s.size() && s.compare(i, 5, "false") == 0) {
        i += 5;
        out = cocos2d::Value(false);
        return true;
    }
    if (c == 'n' && i + 4 <= s.size() && s.compare(i, 4, "null") == 0) {
        i += 4;
        out = cocos2d::Value();
        return true;
    }
    if (std::isdigit(static_cast<unsigned char>(c)) != 0 || c == '-') {
        const size_t valStart = i;
        while (i < s.size() &&
               (std::isdigit(static_cast<unsigned char>(s[i])) != 0 || s[i] == '.' || s[i] == '-' ||
                s[i] == 'e' || s[i] == 'E' || s[i] == '+')) {
            ++i;
        }
        out = valueFromJsonNumberToken(s.substr(valStart, i - valStart));
        return true;
    }
    return false;
}

} // namespace

std::string extractJsonObjectField(const std::string& json, const char* fieldName) {
    const std::string needle = std::string("\"") + fieldName + "\":";
    size_t pos = json.find(needle);
    if (pos == std::string::npos) {
        return {};
    }
    pos = skipWs(json, pos + needle.size());
    if (pos >= json.size() || json[pos] != '{') {
        return {};
    }
    const size_t end = findMatchingBrace(json, pos);
    if (end == std::string::npos) {
        return {};
    }
    return json.substr(pos, end - pos + 1);
}

cocos2d::ValueMap decodeJsonObjectToValueMap(const std::string& jsonObject) {
    if (jsonObject.empty()) {
        return {};
    }
    size_t i = skipWs(jsonObject, 0);
    cocos2d::Value root;
    if (!parseJsonValue(jsonObject, i, root)) {
        return {};
    }
    if (root.getType() != cocos2d::Value::Type::MAP) {
        return {};
    }
    return root.asValueMap();
}

std::string escapeJsonString(const std::string& value) {
    std::ostringstream oss;
    for (char c : value) {
        switch (c) {
        case '\\':
            oss << "\\\\";
            break;
        case '"':
            oss << "\\\"";
            break;
        case '\n':
            oss << "\\n";
            break;
        case '\r':
            oss << "\\r";
            break;
        case '\t':
            oss << "\\t";
            break;
        default:
            oss << c;
            break;
        }
    }
    return oss.str();
}

std::string encodeStringArrayJson(const std::vector<std::string>& values) {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < values.size(); ++i) {
        if (i > 0) {
            oss << ",";
        }
        oss << "\"" << escapeJsonString(values[i]) << "\"";
    }
    oss << "]";
    return oss.str();
}

std::string encodeValueMapJson(const cocos2d::ValueMap& map) {
#if defined(AF_RPC_HEADLESS_TEST)
    (void)map;
    return "{}";
#else
    std::ostringstream oss;
    oss << "{";
    bool first = true;
    for (const auto& entry : map) {
        if (!first) {
            oss << ",";
        }
        first = false;
        oss << "\"" << escapeJsonString(entry.first) << "\":";
        const cocos2d::Value& v = entry.second;
        switch (v.getType()) {
        case cocos2d::Value::Type::STRING:
            oss << "\"" << escapeJsonString(v.asString()) << "\"";
            break;
        case cocos2d::Value::Type::BOOLEAN:
            oss << (v.asBool() ? "true" : "false");
            break;
        case cocos2d::Value::Type::INTEGER:
            oss << v.asInt();
            break;
        case cocos2d::Value::Type::FLOAT:
        case cocos2d::Value::Type::DOUBLE:
            oss << v.asDouble();
            break;
        default:
            oss << "null";
            break;
        }
    }
    oss << "}";
    return oss.str();
#endif
}


} // namespace detail


// -----------------------------------------------------------------------------
// appsflyer::rpc — native blocking RPC + rpcEvent
// -----------------------------------------------------------------------------

namespace rpc {

namespace {

std::mutex g_eventHandlerMutex;
RpcEventHandler g_eventHandler;

#if !defined(AF_RPC_HEADLESS_TEST) && (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

std::string jstringToStd(JNIEnv* env, jstring value) {
    if (value == nullptr) {
        return {};
    }
    const char* utf = env->GetStringUTFChars(value, nullptr);
    std::string out(utf ? utf : "");
    if (utf) {
        env->ReleaseStringUTFChars(value, utf);
    }
    return out;
}

#endif

} // namespace

void setRpcEventHandler(RpcEventHandler handler) {
    std::lock_guard<std::mutex> lock(g_eventHandlerMutex);
    g_eventHandler = std::move(handler);
}

void onNativeRpcEventFromBridge(const std::string& eventJson) {
    RpcEventHandler handler;
    {
        std::lock_guard<std::mutex> lock(g_eventHandlerMutex);
        handler = g_eventHandler;
    }
    if (!handler) {
        return;
    }
    detail::postOnGameThread([handler, eventJson]() { handler(eventJson); });
}

#if defined(AF_RPC_HEADLESS_TEST)

RpcResult platformExecuteBlocking(const std::string&, const std::string&) {
    RpcResult result;
    result.success = false;
    result.errorMessage = "headless test stub";
    return result;
}

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

std::string buildRequestJson(const std::string& method, const std::string& paramsJsonObject) {
    return std::string("{\"method\":\"") + method + "\",\"params\":" +
           (paramsJsonObject.empty() ? "{}" : paramsJsonObject) + "}";
}

RpcResult platformExecuteBlocking(const std::string& method, const std::string& paramsJsonObject) {
    RpcResult result;
    cocos2d::JniMethodInfo methodInfo;
    if (!cocos2d::JniHelper::getStaticMethodInfo(
            methodInfo,
            "com/appsflyer/cocos2dx/rpc/AppsFlyerCocosRpcBridge",
            "executeRpcBlocking",
            "(Ljava/lang/String;)Ljava/lang/String;")) {
        result.success = false;
        result.errorCode = 500;
        result.errorMessage = "AppsFlyerCocosRpcBridge JNI entry not found";
        return result;
    }

    JNIEnv* env = methodInfo.env;
    const std::string requestJson = buildRequestJson(method, paramsJsonObject);
    jstring jRequest = env->NewStringUTF(requestJson.c_str());
    if (jRequest == nullptr || env->ExceptionCheck()) {
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
        if (jRequest != nullptr) {
            env->DeleteLocalRef(jRequest);
        }
        env->DeleteLocalRef(methodInfo.classID);
        result.success = false;
        result.errorCode = 500;
        result.errorMessage = "JNI NewStringUTF failed for RPC request";
        return result;
    }
    jstring jResponse =
        (jstring)env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, jRequest);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        env->DeleteLocalRef(jRequest);
        env->DeleteLocalRef(methodInfo.classID);
        result.success = false;
        result.errorCode = 500;
        result.errorMessage = "Java exception in AppsFlyerCocosRpcBridge.executeRpcBlocking";
        return result;
    }
    env->DeleteLocalRef(jRequest);

    const std::string responseJson = jstringToStd(env, jResponse);
    if (jResponse) {
        env->DeleteLocalRef(jResponse);
    }
    env->DeleteLocalRef(methodInfo.classID);

    result.success = responseJson.find("\"success\":true") != std::string::npos;
    if (!result.success) {
        result.errorCode = 500;
        result.errorMessage = responseJson;
    } else {
        result.dataJson = responseJson;
    }
    return result;
}

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

RpcResult platformExecuteBlocking(const std::string& method, const std::string& paramsJsonObject) {
    RpcResult result;
    const std::string requestJson = std::string("{\"method\":\"") + method + "\",\"params\":" +
                                    (paramsJsonObject.empty() ? "{}" : paramsJsonObject) + "}";
    const std::string responseJson = executeRpcBlockingRequest(requestJson);
    result.success = responseJson.find("\"success\":true") != std::string::npos;
    if (!result.success) {
        result.errorCode = 500;
        result.errorMessage = responseJson;
    } else {
        result.dataJson = responseJson;
    }
    return result;
}

#else

RpcResult platformExecuteBlocking(const std::string& /*method*/, const std::string& /*paramsJsonObject*/) {
    RpcResult result;
    result.success = false;
    result.errorCode = 501;
    result.errorMessage = "RPC supported on Android/iOS only";
    return result;
}

#endif

RpcResult executeBlocking(const std::string& method, const std::string& paramsJsonObject) {
    if (detail::rpcTestHookActive()) {
        return detail::invokeRpcTestHook(method, paramsJsonObject);
    }
    return platformExecuteBlocking(method, paramsJsonObject);
}


} // namespace rpc


// -----------------------------------------------------------------------------
// Listener registry and rpcEvent demux (file-local; before register*Listener)
// -----------------------------------------------------------------------------

namespace {

constexpr const char* kEventSessionReady = "onSessionReady";
constexpr const char* kEventConversionSuccess = "onConversionDataSuccess";
constexpr const char* kEventConversionFail = "onConversionDataFail";
constexpr const char* kEventDeepLinking = "onDeepLinking";
constexpr const char* kEventDeepLinkReceived = "onDeepLinkReceived";

struct ListenerRegistry {
    std::mutex mutex;
    bool rpcEventHandlerInstalled = false;

    void (*onSessionReady)() = nullptr;
    void (*onConversionSuccess)(cocos2d::ValueMap) = nullptr;
    void (*onConversionFail)(cocos2d::ValueMap) = nullptr;
    void (*onDeepLinkingFn)(DeepLinkData) = nullptr;
    std::function<void(DeepLinkData)> onDeepLinkingHandler;
};

ListenerRegistry gListeners;

std::string extractJsonStringField(const std::string& json, const char* field) {
    const std::string key = std::string("\"") + field + "\":\"";
    const auto pos = json.find(key);
    if (pos == std::string::npos) {
        return {};
    }
    const auto start = pos + key.size();
    const auto end = json.find('"', start);
    if (end == std::string::npos) {
        return {};
    }
    return json.substr(start, end - start);
}

DeepLinkData parseDeepLinkEvent(const std::string& eventJson) {
    DeepLinkData result;
    result.status = DeepLinkStatus::NotFound;
    const std::string dataJson = detail::extractJsonObjectField(eventJson, "data");
    if (dataJson.empty()) {
        return result;
    }
    const cocos2d::ValueMap data = detail::decodeJsonObjectToValueMap(dataJson);
    const auto statusIt = data.find("status");
    if (statusIt != data.end() && statusIt->second.getType() == cocos2d::Value::Type::STRING) {
        result.status = deepLinkStatusFromWire(statusIt->second.asString());
    }
    const auto errorIt = data.find("error");
    if (errorIt != data.end()) {
        if (errorIt->second.getType() == cocos2d::Value::Type::STRING) {
            result.error = errorIt->second.asString();
        } else if (errorIt->second.getType() == cocos2d::Value::Type::INTEGER) {
            result.error = std::to_string(errorIt->second.asInt());
        } else if (!errorIt->second.isNull()) {
            result.error = errorIt->second.asString();
        }
    }
    const std::string deepLinkJson = detail::extractJsonObjectField(dataJson, "deepLink");
    if (!deepLinkJson.empty()) {
        result.deepLink = detail::decodeJsonObjectToValueMap(deepLinkJson);
    } else {
        const auto deepLinkIt = data.find("deepLink");
        if (deepLinkIt != data.end() && deepLinkIt->second.getType() == cocos2d::Value::Type::MAP) {
            result.deepLink = deepLinkIt->second.asValueMap();
        } else if (deepLinkIt != data.end() && deepLinkIt->second.getType() == cocos2d::Value::Type::STRING) {
            result.deepLink = detail::decodeJsonObjectToValueMap(deepLinkIt->second.asString());
        }
    }
    if (result.status == DeepLinkStatus::NotFound && !result.deepLink.empty()) {
        result.status = DeepLinkStatus::Found;
    }
    return result;
}

cocos2d::ValueMap parseConversionEventData(const std::string& eventJson) {
    const std::string dataJson = detail::extractJsonObjectField(eventJson, "data");
    if (dataJson.empty()) {
        return {};
    }
    return detail::decodeJsonObjectToValueMap(dataJson);
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
const char* deepLinkRegisterWireMethod() { return "subscribeForDeepLink"; }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
const char* deepLinkRegisterWireMethod() { return "registerDeeplinkListener"; }
#else
const char* deepLinkRegisterWireMethod() { return nullptr; }
#endif

void ensureRpcEventHandlerInstalled() {
    if (gListeners.rpcEventHandlerInstalled) {
        return;
    }
    gListeners.rpcEventHandlerInstalled = true;
    rpc::setRpcEventHandler([](const std::string& eventJson) {
        try {
        const std::string eventName = extractJsonStringField(eventJson, "event");
        CCLOG("[AppsFlyer] event (game thread): %s", eventJson.c_str());

        if (eventName == kEventSessionReady) {
            void (*onReady)() = nullptr;
            {
                std::lock_guard<std::mutex> lock(gListeners.mutex);
                onReady = gListeners.onSessionReady;
            }
            if (onReady) {
                onReady();
            }
            return;
        }

        if (eventName == kEventConversionSuccess) {
            void (*onSuccess)(cocos2d::ValueMap) = nullptr;
            {
                std::lock_guard<std::mutex> lock(gListeners.mutex);
                onSuccess = gListeners.onConversionSuccess;
            }
            if (onSuccess) {
                onSuccess(parseConversionEventData(eventJson));
            }
            return;
        }

        if (eventName == kEventConversionFail) {
            void (*onFail)(cocos2d::ValueMap) = nullptr;
            {
                std::lock_guard<std::mutex> lock(gListeners.mutex);
                onFail = gListeners.onConversionFail;
            }
            if (onFail) {
                onFail(parseConversionEventData(eventJson));
            }
            return;
        }

        if (eventName == kEventDeepLinking || eventName == kEventDeepLinkReceived) {
            void (*onDeepLinkFn)(DeepLinkData) = nullptr;
            std::function<void(DeepLinkData)> onDeepLinkHandler;
            {
                std::lock_guard<std::mutex> lock(gListeners.mutex);
                onDeepLinkFn = gListeners.onDeepLinkingFn;
                onDeepLinkHandler = gListeners.onDeepLinkingHandler;
            }
            const DeepLinkData result = parseDeepLinkEvent(eventJson);
            if (onDeepLinkHandler) {
                onDeepLinkHandler(result);
            } else if (onDeepLinkFn) {
                onDeepLinkFn(result);
            }
        }
        } catch (const std::exception& ex) {
            CCLOG("[AppsFlyer] rpc event handler error: %s", ex.what());
        } catch (...) {
            CCLOG("[AppsFlyer] rpc event handler error: unknown");
        }
    });
}

void installSessionReadyCallback(void (*callback)()) {
    std::lock_guard<std::mutex> lock(gListeners.mutex);
    gListeners.onSessionReady = callback;
}

void (*snapshotSessionReadyCallback())() {
    std::lock_guard<std::mutex> lock(gListeners.mutex);
    return gListeners.onSessionReady;
}

void installConversionSuccessCallback(void (*callback)(cocos2d::ValueMap)) {
    std::lock_guard<std::mutex> lock(gListeners.mutex);
    gListeners.onConversionSuccess = callback;
}

void (*snapshotConversionSuccessCallback())(cocos2d::ValueMap) {
    std::lock_guard<std::mutex> lock(gListeners.mutex);
    return gListeners.onConversionSuccess;
}

void installConversionFailCallback(void (*callback)(cocos2d::ValueMap)) {
    std::lock_guard<std::mutex> lock(gListeners.mutex);
    gListeners.onConversionFail = callback;
}

void (*snapshotConversionFailCallback())(cocos2d::ValueMap) {
    std::lock_guard<std::mutex> lock(gListeners.mutex);
    return gListeners.onConversionFail;
}

void installDeepLinkHandler(std::function<void(DeepLinkData)> handler) {
    std::lock_guard<std::mutex> lock(gListeners.mutex);
    gListeners.onDeepLinkingHandler = std::move(handler);
    gListeners.onDeepLinkingFn = nullptr;
}

void installDeepLinkCallback(void (*callback)(DeepLinkData)) {
    std::lock_guard<std::mutex> lock(gListeners.mutex);
    gListeners.onDeepLinkingFn = callback;
    gListeners.onDeepLinkingHandler = {};
}

void (*snapshotDeepLinkCallback())(DeepLinkData) {
    std::lock_guard<std::mutex> lock(gListeners.mutex);
    return gListeners.onDeepLinkingFn;
}

std::function<void(DeepLinkData)> snapshotDeepLinkHandler() {
    std::lock_guard<std::mutex> lock(gListeners.mutex);
    return gListeners.onDeepLinkingHandler;
}

} // namespace


// -----------------------------------------------------------------------------
// appsflyer — listener registration (schema-generated decls)
// -----------------------------------------------------------------------------

AppsFlyerTask registerDeepLinkListener(std::function<void(DeepLinkData)> onDeepLink,
                                       AppsFlyerCompletion onComplete) {
    ensureRpcEventHandlerInstalled();
    const auto previousHandler = snapshotDeepLinkHandler();
    void (*previousFn)(DeepLinkData) = snapshotDeepLinkCallback();

    std::function<void(DeepLinkData)> handler = std::move(onDeepLink);
    auto install = [handler]() {
        installDeepLinkCallback(nullptr);
        installDeepLinkHandler(handler);
    };
    auto restore = [previousHandler, previousFn]() {
        installDeepLinkHandler(previousHandler);
        installDeepLinkCallback(previousFn);
    };

    return registerListenerRpc(deepLinkRegisterWireMethod(), "{}", install, restore,
                               std::move(onComplete));
}

AppsFlyerTask registerSessionReadyListener(void (*sessionReadyHandler)(),
                                           AppsFlyerCompletion onComplete) {
    ensureRpcEventHandlerInstalled();
    void (*previous)() = snapshotSessionReadyCallback();
    auto install = [sessionReadyHandler]() { installSessionReadyCallback(sessionReadyHandler); };
    auto restore = [previous]() { installSessionReadyCallback(previous); };
    return registerListenerRpc("registerSessionReadyListener", "{}", install, restore,
                               std::move(onComplete));
}

AppsFlyerTask registerConversionListener(void (*onSuccess)(cocos2d::ValueMap),
                                         void (*onFail)(cocos2d::ValueMap),
                                         AppsFlyerCompletion onComplete) {
    ensureRpcEventHandlerInstalled();
    void (*prevSuccess)(cocos2d::ValueMap) = snapshotConversionSuccessCallback();
    void (*prevFail)(cocos2d::ValueMap) = snapshotConversionFailCallback();
    auto install = [onSuccess, onFail]() {
        installConversionSuccessCallback(onSuccess);
        installConversionFailCallback(onFail);
    };
    auto restore = [prevSuccess, prevFail]() {
        installConversionSuccessCallback(prevSuccess);
        installConversionFailCallback(prevFail);
    };
    return registerListenerRpc("registerConversionListener", "{}", install, restore,
                               std::move(onComplete));
}

#if defined(AF_RPC_HEADLESS_TEST)
void setDeepLinkCallback(void (*callback)(DeepLinkData)) {
    installDeepLinkCallback(callback);
}

std::function<void(DeepLinkData)> deepLinkHandlerSnapshot() {
    return snapshotDeepLinkHandler();
}
#endif

} // namespace appsflyer

#if !defined(AF_RPC_HEADLESS_TEST) && (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

extern "C" JNIEXPORT void JNICALL
Java_com_appsflyer_cocos2dx_rpc_AppsFlyerCocosRpcBridge_nativeDeliverRpcEvent(
    JNIEnv* env,
    jclass /*clazz*/,
    jstring eventJson) {
    if (eventJson == nullptr) {
        return;
    }
    const char* utf = env->GetStringUTFChars(eventJson, nullptr);
    std::string payload(utf ? utf : "");
    if (utf) {
        env->ReleaseStringUTFChars(eventJson, utf);
    }
    appsflyer::rpc::onNativeRpcEventFromBridge(payload);
}

#endif