#include "schema_model.hpp"

#include <cctype>
#include <unordered_map>
#include <unordered_set>

namespace afcodegen {
namespace {

bool isCocos2dxPublic(const nlohmann::json& method) {
    if (!method.contains("plugins")) {
        return true;
    }
    const auto& plugins = method.at("plugins");
    if (plugins.value("cocos2dx", "") == "public") {
        return true;
    }
    const std::string def = plugins.value("default", "");
    if (def == "internal") {
        return false;
    }
    return def != "none";
}

bool hasMobileRpc(const nlohmann::json& method) {
    if (!method.contains("rpc")) {
        return false;
    }
    const auto& rpc = method.at("rpc");
    const bool android = rpc.contains("android") && rpc.at("android").is_object();
    const bool ios = rpc.contains("ios") && rpc.at("ios").is_object();
    return android || ios;
}

bool hasListenerCallbacks(const nlohmann::json& method) {
    if (!method.contains("publicApi")) {
        return false;
    }
    const auto& api = method.at("publicApi");
    if (!api.contains("callbacks")) {
        return false;
    }
    const auto& cb = api.at("callbacks");
    return cb.is_object() && !cb.empty();
}

std::string cppTypeFromJsonSchema(const nlohmann::json& schemaType) {
    if (schemaType.is_string()) {
        const std::string t = schemaType.get<std::string>();
        if (t == "string") {
            return "std::string";
        }
        if (t == "boolean") {
            return "bool";
        }
        if (t == "integer") {
            return "int64_t";
        }
        if (t == "number") {
            return "double";
        }
        if (t == "object") {
            return "cocos2d::ValueMap";
        }
        return "std::string";
    }
    if (schemaType.is_array()) {
        nlohmann::json nonNull = nlohmann::json::array();
        for (const auto& item : schemaType) {
            if (item.is_string() && item.get<std::string>() == "null") {
                continue;
            }
            nonNull.push_back(item);
        }
        if (nonNull.size() == 1) {
            if (nonNull[0].is_string() && nonNull[0].get<std::string>() == "object") {
                return "cocos2d::ValueMap";
            }
            return cppTypeFromJsonSchema(nonNull[0]);
        }
        for (const auto& item : nonNull) {
            if (item.is_string() && item.get<std::string>() == "object") {
                return "cocos2d::ValueMap";
            }
        }
        return "std::string";
    }
    if (schemaType.is_object()) {
        if (!schemaType.contains("type")) {
            return "std::string";
        }
        const auto& t = schemaType.at("type");
        if (t.is_string()) {
            const std::string ts = t.get<std::string>();
            if (ts == "array") {
                if (schemaType.contains("items") && schemaType.at("items").value("type", "") == "string") {
                    return "std::vector<std::string>";
                }
                return "std::vector<std::string>";
            }
            if (ts == "object") {
                return "cocos2d::ValueMap";
            }
            return cppTypeFromJsonSchema(nlohmann::json(ts));
        }
        if (t.is_array()) {
            return cppTypeFromJsonSchema(t);
        }
    }
    return "std::string";
}

std::string pascalCase(const std::string& name) {
    if (name.empty()) {
        return name;
    }
    std::string out = name;
    out[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(out[0])));
    return out;
}

} // namespace

std::vector<SchemaMethod> loadEligibleMethods(const nlohmann::json& schemaRoot) {
    std::vector<SchemaMethod> out;
    if (!schemaRoot.contains("methods") || !schemaRoot.at("methods").is_array()) {
        return out;
    }
    for (const auto& m : schemaRoot.at("methods")) {
        if (!m.contains("name") || !m.contains("publicApi")) {
            continue;
        }
        if (m.at("publicApi").value("delivery", "") != "awaitable") {
            continue;
        }
        if (!isCocos2dxPublic(m)) {
            continue;
        }
        if (!hasMobileRpc(m)) {
            continue;
        }
        SchemaMethod method;
        method.name = m.at("name").get<std::string>();
        method.isListener = hasListenerCallbacks(m);
        method.rpc = m.value("rpc", nlohmann::json::object());
        method.publicApi = m.at("publicApi");
        method.plugins = m.value("plugins", nlohmann::json::object());
        method.mappings = m.value("mappings", nlohmann::json::object());
        out.push_back(std::move(method));
    }
    return out;
}

std::vector<ParamField> collectParamsProperties(const nlohmann::json& publicApiParameters) {
    std::vector<ParamField> fields;
    if (!publicApiParameters.is_object() || !publicApiParameters.contains("properties")) {
        return fields;
    }
    std::unordered_set<std::string> required;
    if (publicApiParameters.contains("required") && publicApiParameters.at("required").is_array()) {
        for (const auto& r : publicApiParameters.at("required")) {
            required.insert(r.get<std::string>());
        }
    }
    for (auto it = publicApiParameters.at("properties").begin();
         it != publicApiParameters.at("properties").end(); ++it) {
        ParamField field;
        field.name = it.key();
        field.cppType = cppTypeFromJsonSchema(it.value());
        field.required = required.count(field.name) != 0;
        if (it.value().is_object() && it.value().contains("default")) {
            field.hasDefault = true;
            field.defaultValue = it.value().at("default");
        }
        fields.push_back(std::move(field));
    }
    return fields;
}

std::string paramsStructName(const std::string& methodName) {
    if (methodName == "init") {
        return "InitParams";
    }
    if (methodName == "start") {
        return "StartParams";
    }
    if (methodName == "logEvent") {
        return "LogEventParams";
    }
    return pascalCase(methodName) + "Params";
}

std::string appsFlyerXMethodName(const std::string& schemaName) {
    const std::unordered_map<std::string, std::string> kAppsFlyerXRenames = {
        {"getAppsFlyerUID", "fetchAppsFlyerUID"},
    };
    const auto it = kAppsFlyerXRenames.find(schemaName);
    if (it != kAppsFlyerXRenames.end()) {
        return it->second;
    }
    return schemaName;
}

std::string escapeCppStringLiteral(const std::string& value) {
    std::string out;
    out.reserve(value.size());
    for (char c : value) {
        switch (c) {
        case '\\':
            out += "\\\\";
            break;
        case '"':
            out += "\\\"";
            break;
        default:
            out += c;
            break;
        }
    }
    return out;
}

} // namespace afcodegen
