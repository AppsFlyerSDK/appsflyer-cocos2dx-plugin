#include "AppsFlyer/rpc/AppsFlyerRpcTransport.h"

#include "cocos2d.h"

#include <sstream>

namespace appsflyer {
namespace detail {

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
} // namespace appsflyer
