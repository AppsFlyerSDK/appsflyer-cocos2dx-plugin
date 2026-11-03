#include "emit.hpp"

#include <sstream>

namespace afcodegen {
namespace {

std::string platformGuard(const SchemaMethod& method) {
    const bool hasAndroid =
        method.rpc.contains("android") && method.rpc.at("android").is_object();
    const bool hasIos = method.rpc.contains("ios") && method.rpc.at("ios").is_object();
    if (hasAndroid && hasIos) {
        return "";
    }
    if (hasAndroid) {
        return "CC_PLATFORM_ANDROID";
    }
    if (hasIos) {
        return "CC_PLATFORM_IOS";
    }
    return "CC_PLATFORM_UNKNOWN";
}

void appendExecuteRpcReturn(std::ostringstream& out, const std::string& wireMethod) {
    out << "    return executeRpc(\"" << wireMethod << "\", paramsJson, std::move(onComplete));\n";
}

std::string wireJsonKey(const SchemaMethod& method, const std::string& publicField) {
    for (const char* plat : {"android", "ios"}) {
        if (!method.mappings.contains(plat) || !method.mappings.at(plat).is_object()) {
            continue;
        }
        for (auto it = method.mappings.at(plat).begin(); it != method.mappings.at(plat).end(); ++it) {
            if (!it.value().is_object()) {
                continue;
            }
            if (it.value().value("from", "") == publicField) {
                const std::string path = it.key();
                if (!path.empty() && path.front() == '/') {
                    return path.substr(1);
                }
                return path;
            }
        }
    }
    return publicField;
}

std::string emitStruct(const std::string& structName, const std::vector<ParamField>& fields) {
    std::ostringstream oss;
    oss << "struct " << structName << " {\n";
    for (const auto& f : fields) {
        oss << "    " << f.cppType << " " << f.name;
        if (f.cppType == "bool") {
            if (f.hasDefault && f.defaultValue.is_boolean()) {
                oss << " = " << (f.defaultValue.get<bool>() ? "true" : "false");
            } else {
                oss << " = false";
            }
        } else if (f.cppType == "int64_t" || f.cppType == "double") {
            if (f.hasDefault && f.defaultValue.is_number()) {
                oss << " = " << f.defaultValue.dump();
            }
        } else if (f.cppType == "std::string") {
            if (f.hasDefault && f.defaultValue.is_string()) {
                oss << " = \"" << escapeCppStringLiteral(f.defaultValue.get<std::string>()) << "\"";
            }
        }
        oss << ";\n";
    }
    oss << "};";
    return oss.str();
}

bool useScalarParam(const std::vector<ParamField>& fields) {
    return fields.size() == 1;
}

std::string emitCppParam(const ParamField& field) {
    if (field.cppType == "bool" || field.cppType == "int64_t" || field.cppType == "double") {
        return field.cppType + " " + field.name;
    }
    return "const " + field.cppType + "& " + field.name;
}

std::string fieldJsonAccess(const std::vector<ParamField>& fields, const ParamField& field) {
    if (useScalarParam(fields)) {
        return field.name;
    }
    return "params." + field.name;
}

std::string emitParamsArgumentList(const std::vector<ParamField>& fields, const std::string& structName) {
    if (fields.empty()) {
        return {};
    }
    if (useScalarParam(fields)) {
        return emitCppParam(fields.front());
    }
    return "const " + structName + "& params";
}

std::string emitParamsPassList(const std::vector<ParamField>& fields) {
    if (fields.empty()) {
        return {};
    }
    if (useScalarParam(fields)) {
        return fields.front().name;
    }
    return "params";
}

std::string emitInlineParamsJson(const SchemaMethod& method, const std::vector<ParamField>& fields) {
    if (fields.empty()) {
        return "    const std::string paramsJson = \"{}\";";
    }
    std::ostringstream oss;
    oss << "    std::ostringstream oss;\n";
    for (size_t i = 0; i < fields.size(); ++i) {
        const auto& f = fields[i];
        const std::string access = fieldJsonAccess(fields, f);
        const std::string jsonKey = wireJsonKey(method, f.name);
        oss << "    " << (i == 0 ? "oss << \"{\";" : "oss << \",\";") << "\n";
        const std::string key = "\\\"" + jsonKey + "\\\"";
        if (f.cppType == "bool") {
            oss << "    oss << \"" << key << ":\" << (" << access << " ? \"true\" : \"false\");\n";
        } else if (f.cppType == "int64_t" || f.cppType == "double") {
            oss << "    oss << \"" << key << ":\" << " << access << ";\n";
        } else if (f.cppType == "std::string") {
            oss << "    oss << \"" << key << ":\\\"\" << appsflyer::detail::escapeJsonString(" << access
                << ") << \"\\\"\";\n";
        } else if (f.cppType == "std::vector<std::string>") {
            oss << "    oss << \"" << key << ":\" << appsflyer::detail::encodeStringArrayJson(" << access
                << ");\n";
        } else if (f.cppType == "cocos2d::ValueMap") {
            oss << "    oss << \"" << key << ":\" << appsflyer::detail::encodeValueMapJson(" << access
                << ");\n";
        }
    }
    oss << "    oss << \"}\";\n";
    oss << "    const std::string paramsJson = oss.str();";
    return oss.str();
}

std::string emitListenerDecl(const SchemaMethod& method) {
    if (method.name == "registerDeepLinkListener") {
        return "AppsFlyerTask registerDeepLinkListener(\n"
               "    std::function<void(AppsFlyerXDeepLinkResult)> onDeepLink,\n"
               "    AppsFlyerCompletion onComplete = {});\n";
    }
    if (method.name == "registerSessionReadyListener") {
        return "AppsFlyerTask registerSessionReadyListener(void (*onSessionReady)(),\n"
               "                                           AppsFlyerCompletion onComplete = {});\n";
    }
    if (method.name == "registerConversionListener") {
        return "AppsFlyerTask registerConversionListener(void (*onSuccess)(cocos2d::ValueMap),\n"
               "                                         void (*onFail)(cocos2d::ValueMap),\n"
               "                                         AppsFlyerCompletion onComplete = {});\n";
    }
    return {};
}

std::string emitListenerXWrapper(const SchemaMethod& method) {
    const std::string xName = appsFlyerXMethodName(method.name);
    if (method.name == "registerDeepLinkListener") {
        return "    static AppsFlyerTask " + xName +
               "(std::function<void(AppsFlyerXDeepLinkResult)> onDeepLink,\n"
               "                      AppsFlyerCompletion onComplete = {}) {\n"
               "        return appsflyer::registerDeepLinkListener(std::move(onDeepLink), std::move(onComplete));\n"
               "    }\n\n";
    }
    if (method.name == "registerSessionReadyListener") {
        return "    static AppsFlyerTask " + xName +
               "(void (*onSessionReady)(), AppsFlyerCompletion onComplete = {}) {\n"
               "        return appsflyer::registerSessionReadyListener(onSessionReady, std::move(onComplete));\n"
               "    }\n\n";
    }
    if (method.name == "registerConversionListener") {
        return "    static AppsFlyerTask " + xName +
               "(void (*onSuccess)(cocos2d::ValueMap), void (*onFail)(cocos2d::ValueMap),\n"
               "                      AppsFlyerCompletion onComplete = {}) {\n"
               "        return appsflyer::registerConversionListener(onSuccess, onFail, std::move(onComplete));\n"
               "    }\n\n";
    }
    return {};
}

std::string emitAppsflyerMethodImpl(const SchemaMethod& method) {
    const auto fields = collectParamsProperties(method.publicApi.value("parameters", nlohmann::json::object()));
    const std::string structName = paramsStructName(method.name);
    const std::string guard = platformGuard(method);

    std::string android;
    std::string ios;
    if (method.rpc.contains("android") && method.rpc.at("android").is_object()) {
        android = method.rpc.at("android").value("method", "");
    }
    if (method.rpc.contains("ios") && method.rpc.at("ios").is_object()) {
        ios = method.rpc.at("ios").value("method", "");
    }
    const bool splitWire = !android.empty() && !ios.empty() && android != ios;

    std::ostringstream body;
    body << "AppsFlyerTask " << method.name << "(";
    const std::string paramsDecl = emitParamsArgumentList(fields, structName);
    if (!paramsDecl.empty()) {
        body << paramsDecl << ", ";
    }
    body << "AppsFlyerCompletion onComplete) {\n";

    if (guard.empty()) {
        body << "#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)\n";
    } else {
        body << "#if (CC_TARGET_PLATFORM == " << guard << ")\n";
    }

    if (!fields.empty()) {
        body << emitInlineParamsJson(method, fields) << "\n";
    } else {
        body << "    const std::string paramsJson = \"{}\";\n";
    }

    if (splitWire) {
        body << "#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)\n";
        appendExecuteRpcReturn(body, android);
        body << "#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)\n";
        appendExecuteRpcReturn(body, ios);
        body << "#else\n    return detail::unsupportedPlatformTask(std::move(onComplete));\n#endif\n";
    } else if (!android.empty()) {
        appendExecuteRpcReturn(body, android);
    } else if (!ios.empty()) {
        appendExecuteRpcReturn(body, ios);
    } else {
        body << "    return detail::unsupportedPlatformTask(std::move(onComplete));\n";
    }

    body << "#else\n    return detail::unsupportedPlatformTask(std::move(onComplete));\n#endif\n";

    body << "}";
    return body.str();
}

bool anyListener(const std::vector<SchemaMethod>& methods) {
    for (const auto& m : methods) {
        if (m.isListener) {
            return true;
        }
    }
    return false;
}

} // namespace

const char* kGeneratedBanner =
    "// AUTO-GENERATED by tools/af-schema-compiler — do not edit.\n"
    "// Schema: schema/appsflyer-plugins-rpc-schema.json\n";

std::unordered_map<std::string, std::string> emitAll(const std::vector<SchemaMethod>& methods) {
    const bool hasListeners = anyListener(methods);

    std::ostringstream rpcHeader;
    rpcHeader << kGeneratedBanner << R"(
#ifndef AppsFlyerRpcSchema_generated_h
#define AppsFlyerRpcSchema_generated_h

#include "../AppsFlyerTask.h"
#include "cocos2d.h"

#include <cstdint>
#include <functional>
#include <string>
#include <vector>
)";
    if (hasListeners) {
        rpcHeader << "#include \"../AppsFlyerXDeepLinkResult.h\"\n";
    }
    rpcHeader << R"(
namespace appsflyer {

)";

    std::ostringstream rpcDecls;
    for (const auto& m : methods) {
        if (m.isListener) {
            rpcDecls << emitListenerDecl(m);
            continue;
        }
        const auto fields = collectParamsProperties(m.publicApi.value("parameters", nlohmann::json::object()));
        const std::string structName = paramsStructName(m.name);
        if (fields.size() > 1) {
            rpcHeader << emitStruct(structName, fields) << "\n\n";
        }
        if (fields.empty()) {
            rpcDecls << "AppsFlyerTask " << m.name << "(AppsFlyerCompletion onComplete = {});\n";
        } else {
            rpcDecls << "AppsFlyerTask " << m.name << "(" << emitParamsArgumentList(fields, structName)
                     << ", AppsFlyerCompletion onComplete = {});\n";
        }
    }

    rpcHeader << rpcDecls.str() << "\n} // namespace appsflyer\n\n#endif\n";

    std::ostringstream rpcCpp;
    rpcCpp << kGeneratedBanner << R"(
#include "AppsFlyer/generated/AppsFlyerRpcSchema.generated.h"
#include "AppsFlyer/rpc/AppsFlyerRpcTransport.h"

#include <sstream>
#include <utility>

namespace appsflyer {

)";

    for (const auto& m : methods) {
        if (m.isListener) {
            continue;
        }
        rpcCpp << emitAppsflyerMethodImpl(m) << "\n\n";
    }
    rpcCpp << "} // namespace appsflyer\n";

    std::ostringstream xMethodsHeader;
    xMethodsHeader << kGeneratedBanner << R"(
#ifndef AppsFlyerXSchemaMethods_generated_h
#define AppsFlyerXSchemaMethods_generated_h

#include <functional>
#include <utility>

// Inline AppsFlyerX → appsflyer:: (included inside class AppsFlyerX).

)";
    for (const auto& m : methods) {
        if (m.isListener) {
            xMethodsHeader << emitListenerXWrapper(m);
            continue;
        }
        const auto fields = collectParamsProperties(m.publicApi.value("parameters", nlohmann::json::object()));
        const std::string xName = appsFlyerXMethodName(m.name);
        const std::string structName = paramsStructName(m.name);
        if (fields.size() > 1) {
            xMethodsHeader << "    using " << structName << " = appsflyer::" << structName << ";\n";
        }
        xMethodsHeader << "    static AppsFlyerTask " << xName << "(";
        const std::string paramsDecl = emitParamsArgumentList(fields, structName);
        if (!paramsDecl.empty()) {
            xMethodsHeader << paramsDecl << ", ";
        }
        xMethodsHeader << "AppsFlyerCompletion onComplete = {}) {\n";
        xMethodsHeader << "        return appsflyer::" << m.name << "(";
        const std::string paramsPass = emitParamsPassList(fields);
        if (!paramsPass.empty()) {
            xMethodsHeader << paramsPass << ", ";
        }
        xMethodsHeader << "std::move(onComplete));\n    }\n\n";
    }
    xMethodsHeader << "#endif\n";

    return {
        {"AppsFlyerRpcSchema.generated.h", rpcHeader.str()},
        {"AppsFlyerRpcSchema.generated.cpp", rpcCpp.str()},
        {"AppsFlyerXSchemaMethods.generated.h", xMethodsHeader.str()},
    };
}

} // namespace afcodegen
