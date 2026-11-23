#include "generator.hpp"

#include <cctype>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace afcodegen {
namespace {

void appendExecuteRpcReturn(std::ostringstream& out, const std::string& wireMethod) {
    out << "    return executeRpc(\"" << wireMethod << "\", paramsJson, std::move(onComplete));\n";
}

bool isHandWrittenUnregisterMethod(const std::string& name) {
    return name == "unregisterDeepLinkListener" || name == "unregisterConversionListener" ||
           name == "unregisterSessionReadyListener";
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
    if (!field.schemaEnumName.empty()) {
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
        } else if (!f.schemaEnumName.empty()) {
            const std::string schemaKeyFn = "appsflyer::detail::schemaKey_" + f.schemaEnumName;
            std::string wireExpr = schemaKeyFn + "(" + access + ")";
            const std::string resolver = valueMapResolverCall(method, f.name);
            if (!resolver.empty()) {
                wireExpr = resolver + "(" + wireExpr + ")";
            } else {
                const std::string transform = transformResolverCall(method, f.name);
                if (!transform.empty()) {
                    wireExpr = transform + "(" + wireExpr + ")";
                }
            }
            oss << "    oss << \"" << key << ":\\\"\" << appsflyer::detail::escapeJsonString(" << wireExpr
                << ") << \"\\\"\";\n";
        } else if (f.cppType == "std::string") {
            const std::string resolver = valueMapResolverCall(method, f.name);
            const std::string stringExpr =
                resolver.empty() ? access : (resolver + "(" + access + ")");
            oss << "    oss << \"" << key << ":\\\"\" << appsflyer::detail::escapeJsonString(" << stringExpr
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
               "    std::function<void(appsflyer::DeepLinkData)> onDeepLink,\n"
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
               "(std::function<void(appsflyer::DeepLinkData)> onDeepLink,\n"
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

    std::string android;
    std::string ios;
    const bool hasAndroid = method.rpc.contains("android") && method.rpc.at("android").is_object();
    const bool hasIos = method.rpc.contains("ios") && method.rpc.at("ios").is_object();
    if (hasAndroid) {
        android = method.rpc.at("android").value("method", "");
    }
    if (hasIos) {
        ios = method.rpc.at("ios").value("method", "");
    }
    const bool splitWire = hasAndroid && hasIos && android != ios;

    std::ostringstream body;
    body << "AppsFlyerTask " << method.name << "(";
    const std::string paramsDecl = emitParamsArgumentList(fields, structName);
    if (!paramsDecl.empty()) {
        body << paramsDecl << ", ";
    }
    body << "AppsFlyerCompletion onComplete) {\n";

    if (!fields.empty()) {
        body << emitInlineParamsJson(method, fields) << "\n";
    } else {
        body << "    const std::string paramsJson = \"{}\";\n";
    }

    if (hasAndroid && hasIos) {
        if (splitWire) {
            body << "#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)\n";
            appendExecuteRpcReturn(body, android);
            body << "#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)\n";
            appendExecuteRpcReturn(body, ios);
            body << "#endif\n";
        } else {
            appendExecuteRpcReturn(body, android);
        }
    } else if (hasAndroid) {
        body << "#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)\n";
        appendExecuteRpcReturn(body, android);
        body << "#else\n    return detail::unsupportedPlatformTask(std::move(onComplete));\n#endif\n";
    } else if (hasIos) {
        body << "#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)\n";
        appendExecuteRpcReturn(body, ios);
        body << "#else\n    return detail::unsupportedPlatformTask(std::move(onComplete));\n#endif\n";
    } else {
        body << "    return detail::unsupportedPlatformTask(std::move(onComplete));\n";
    }

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


std::string resolverFunctionName(const std::string& methodName, const std::string& fieldName) {
    return "resolveValueMap_" + methodName + "_" + fieldName;
}

std::string schemaKeyFunctionName(const std::string& enumName) { return "schemaKey_" + enumName; }

std::string camelToPascalMember(const std::string& camel) {
    if (camel.empty()) {
        return camel;
    }
    std::string out = camel;
    out[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(out[0])));
    return out;
}

const nlohmann::json* mappingEntryForField(const SchemaMethod& method, const std::string& fieldName,
                                            const char* platform) {
    if (!method.mappings.contains(platform) || !method.mappings.at(platform).is_object()) {
        return nullptr;
    }
    for (auto it = method.mappings.at(platform).begin(); it != method.mappings.at(platform).end(); ++it) {
        if (!it.value().is_object()) {
            continue;
        }
        if (it.value().value("from", "") == fieldName && it.value().contains("valueMap")) {
            return &it.value();
        }
    }
    return nullptr;
}

bool fieldHasValueMap(const SchemaMethod& method, const std::string& fieldName) {
    return mappingEntryForField(method, fieldName, "android") != nullptr ||
           mappingEntryForField(method, fieldName, "ios") != nullptr;
}

std::string emitMapInitializer(const nlohmann::json& valueMap) {
    std::ostringstream oss;
    oss << "{";
    bool first = true;
    for (auto it = valueMap.begin(); it != valueMap.end(); ++it) {
        if (!it.value().is_string()) {
            continue;
        }
        if (!first) {
            oss << ", ";
        }
        first = false;
        oss << "{\"" << escapeCppStringLiteral(it.key()) << "\", \""
            << escapeCppStringLiteral(it.value().get<std::string>()) << "\"}";
    }
    oss << "}";
    return oss.str();
}

std::string emitSchemaEnumClass(const std::string& enumName, const nlohmann::json& enumValues) {
    std::ostringstream oss;
    oss << "enum class " << enumName << " {\n";
    for (const auto& value : enumValues) {
        if (!value.is_string()) {
            continue;
        }
        oss << "    " << camelToPascalMember(value.get<std::string>()) << ",\n";
    }
    oss << "};\n";
    return oss.str();
}

void appendSchemaEnums(std::ostringstream& out, const std::vector<SchemaMethod>& methods) {
    std::unordered_map<std::string, nlohmann::json> seen;
    for (const auto& method : methods) {
        const auto fields = collectParamsProperties(method.publicApi.value("parameters", nlohmann::json::object()));
        for (const auto& field : fields) {
            if (field.schemaEnumName.empty()) {
                continue;
            }
            if (seen.count(field.schemaEnumName) != 0) {
                continue;
            }
            const auto& prop = method.publicApi.at("parameters").at("properties").at(field.name);
            if (!prop.contains("enum")) {
                continue;
            }
            seen[field.schemaEnumName] = prop.at("enum");
            out << emitSchemaEnumClass(field.schemaEnumName, prop.at("enum")) << "\n";
        }
    }
}

void appendSchemaKeyDecls(std::ostringstream& out, const std::vector<SchemaMethod>& methods) {
    std::unordered_map<std::string, nlohmann::json> seen;
    for (const auto& method : methods) {
        const auto fields = collectParamsProperties(method.publicApi.value("parameters", nlohmann::json::object()));
        for (const auto& field : fields) {
            if (field.schemaEnumName.empty() || seen.count(field.schemaEnumName) != 0) {
                continue;
            }
            seen[field.schemaEnumName] = nlohmann::json::object();
            out << "std::string " << schemaKeyFunctionName(field.schemaEnumName) << "(" << field.schemaEnumName
                << " value);\n";
        }
    }
}

void appendSchemaKeyDefs(std::ostringstream& out, const std::vector<SchemaMethod>& methods) {
    std::unordered_map<std::string, nlohmann::json> seen;
    for (const auto& method : methods) {
        const auto fields = collectParamsProperties(method.publicApi.value("parameters", nlohmann::json::object()));
        for (const auto& field : fields) {
            if (field.schemaEnumName.empty() || seen.count(field.schemaEnumName) != 0) {
                continue;
            }
            const auto& prop = method.publicApi.at("parameters").at("properties").at(field.name);
            seen[field.schemaEnumName] = prop.at("enum");
            const std::string enumName = field.schemaEnumName;
            out << "std::string " << schemaKeyFunctionName(enumName) << "(" << enumName << " value) {\n";
            out << "    switch (value) {\n";
            for (const auto& v : prop.at("enum")) {
                if (!v.is_string()) {
                    continue;
                }
                const std::string key = v.get<std::string>();
                out << "    case " << enumName << "::" << camelToPascalMember(key) << ":\n";
                out << "        return \"" << escapeCppStringLiteral(key) << "\";\n";
            }
            out << "    }\n";
            out << "    return \"\";\n";
            out << "}\n\n";
        }
    }
}

const nlohmann::json* mappingEntryForFrom(const SchemaMethod& method, const std::string& fromField,
                                          const char* platform) {
    if (!method.mappings.contains(platform) || !method.mappings.at(platform).is_object()) {
        return nullptr;
    }
    for (auto it = method.mappings.at(platform).begin(); it != method.mappings.at(platform).end(); ++it) {
        if (!it.value().is_object()) {
            continue;
        }
        if (it.value().value("from", "") == fromField) {
            return &it.value();
        }
    }
    return nullptr;
}

const nlohmann::json* validateMethodPurchaseTypeEnum(const std::vector<SchemaMethod>& methods) {
    for (const auto& method : methods) {
        if (method.name != "validateAndLogInAppPurchase") {
            continue;
        }
        const auto& purchase = method.publicApi.at("parameters").at("properties").at("purchase");
        if (!purchase.contains("oneOf") || !purchase.at("oneOf").is_array() || purchase.at("oneOf").empty()) {
            return nullptr;
        }
        const auto& first = purchase.at("oneOf").at(0);
        if (!first.contains("properties") || !first.at("properties").contains("purchaseType")) {
            return nullptr;
        }
        const auto& pt = first.at("properties").at("purchaseType");
        if (!pt.contains("enum")) {
            return nullptr;
        }
        return &pt.at("enum");
    }
    return nullptr;
}

std::string emitSchemaKeySwitch(const std::string& enumName, const nlohmann::json& enumValues) {
    std::ostringstream out;
    out << "std::string schemaKey_" << enumName << "(" << enumName << " value) {\n";
    out << "    switch (value) {\n";
    for (const auto& v : enumValues) {
        if (!v.is_string()) {
            continue;
        }
        const std::string key = v.get<std::string>();
        std::string member = key;
        if (!member.empty()) {
            member[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(member[0])));
        }
        out << "    case " << enumName << "::" << member << ":\n";
        out << "        return \"" << escapeCppStringLiteral(key) << "\";\n";
    }
    out << "    }\n";
    out << "    return \"\";\n";
    out << "}\n";
    return out.str();
}
} // namespace

const char* kGeneratedBanner =
    "// AUTO-GENERATED by tools/af-schema-compiler — do not edit.\n"
    "// Schema: schema/appsflyer-plugins-rpc-schema.json\n";

std::vector<ValueMapBinding> collectValueMapBindings(const std::vector<SchemaMethod>& methods) {
    std::vector<ValueMapBinding> bindings;
    for (const auto& method : methods) {
        if (method.isListener) {
            continue;
        }
        const auto fields = collectParamsProperties(method.publicApi.value("parameters", nlohmann::json::object()));
        for (const auto& field : fields) {
            if (!fieldHasValueMap(method, field.name)) {
                continue;
            }
            if (field.schemaEnumName.empty() && field.cppType != "std::string") {
                continue;
            }
            bindings.push_back(ValueMapBinding{
                method.name,
                field.name,
                resolverFunctionName(method.name, field.name),
            });
        }
    }
    return bindings;
}

std::string valueMapResolverCall(const SchemaMethod& method, const std::string& fieldName) {
    if (!fieldHasValueMap(method, fieldName)) {
        return {};
    }
    return "appsflyer::detail::" + resolverFunctionName(method.name, fieldName);
}

std::string emitValueMapHeaderSection(const std::vector<SchemaMethod>& methods) {
    const auto bindings = collectValueMapBindings(methods);
    const std::string transformDecls = emitTransformHeaderDecls(methods);
    const std::string purchaseTypeDecl = emitValidatePurchaseTypeSchemaKeyDecl();

    std::ostringstream enums;
    appendSchemaEnums(enums, methods);
    if (bindings.empty() && enums.str().empty() && transformDecls.empty() && purchaseTypeDecl.empty()) {
        return {};
    }

    std::ostringstream header;
    header << enums.str();
    if (!bindings.empty() || !enums.str().empty() || !transformDecls.empty() || !purchaseTypeDecl.empty()) {
        header << "namespace detail {\n\n";
        appendSchemaKeyDecls(header, methods);
        if (!purchaseTypeDecl.empty()) {
            header << purchaseTypeDecl;
        }
        header << transformDecls;
        for (const auto& binding : bindings) {
            header << "std::string " << binding.resolverFn << "(const std::string& schemaKey);\n";
        }
        header << "\n} // namespace detail\n";
    }
    return header.str();
}

std::string emitValueMapCppSection(const std::vector<SchemaMethod>& methods) {
    const auto bindings = collectValueMapBindings(methods);
    const std::string transformDefs = emitTransformCppDefs(methods);
    const std::string purchaseTypeDef = emitValidatePurchaseTypeSchemaKeyDef(methods);
    bool hasEnumKeys = false;
    for (const auto& method : methods) {
        const auto fields = collectParamsProperties(method.publicApi.value("parameters", nlohmann::json::object()));
        for (const auto& field : fields) {
            if (!field.schemaEnumName.empty()) {
                hasEnumKeys = true;
                break;
            }
        }
    }
  if (bindings.empty() && !hasEnumKeys && transformDefs.empty() && purchaseTypeDef.empty()) {
        return {};
    }

    std::ostringstream cpp;
    cpp << "namespace detail {\n";
    if (hasEnumKeys) {
        cpp << "\n";
        appendSchemaKeyDefs(cpp, methods);
    }
    if (!purchaseTypeDef.empty()) {
        cpp << "\n" << purchaseTypeDef << "\n";
    }
    if (!transformDefs.empty()) {
        cpp << transformDefs;
    }
    if (!bindings.empty()) {
        cpp << "namespace {\n\n";
        for (const auto& binding : bindings) {
            SchemaMethod method;
            for (const auto& m : methods) {
                if (m.name == binding.methodName) {
                    method = m;
                    break;
                }
            }
            const nlohmann::json* androidEntry = mappingEntryForField(method, binding.fieldName, "android");
            const nlohmann::json* iosEntry = mappingEntryForField(method, binding.fieldName, "ios");

            cpp << "const std::unordered_map<std::string, std::string>& " << binding.resolverFn
                << "Map() {\n";
            cpp << "#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)\n";
            if (androidEntry != nullptr) {
                cpp << "    static const std::unordered_map<std::string, std::string> kMap = "
                    << emitMapInitializer(androidEntry->at("valueMap")) << ";\n";
            } else {
                cpp << "    static const std::unordered_map<std::string, std::string> kMap = {};\n";
            }
            cpp << "#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)\n";
            if (iosEntry != nullptr) {
                cpp << "    static const std::unordered_map<std::string, std::string> kMap = "
                    << emitMapInitializer(iosEntry->at("valueMap")) << ";\n";
            } else {
                cpp << "    static const std::unordered_map<std::string, std::string> kMap = {};\n";
            }
            cpp << "#else\n";
            cpp << "    static const std::unordered_map<std::string, std::string> kMap = {};\n";
            cpp << "#endif\n";
            cpp << "    return kMap;\n";
            cpp << "}\n\n";
        }
        cpp << "} // namespace\n\n";

        for (const auto& binding : bindings) {
            cpp << "std::string " << binding.resolverFn << "(const std::string& schemaKey) {\n";
            cpp << "    const auto& map = " << binding.resolverFn << "Map();\n";
            cpp << "    const auto it = map.find(schemaKey);\n";
            cpp << "    if (it != map.end()) {\n";
            cpp << "        return it->second;\n";
            cpp << "    }\n";
            cpp << "    return schemaKey;\n";
            cpp << "}\n\n";
        }
    }
    cpp << "} // namespace detail\n\n";
    return cpp.str();
}

std::string transformNameForPublicField(const SchemaMethod& method, const std::string& publicFieldName) {
    if (const auto* android = mappingEntryForFrom(method, publicFieldName, "android")) {
        if (android->contains("valueMap")) {
            return {};
        }
        if (android->contains("transform")) {
            return android->at("transform").get<std::string>();
        }
    }
    if (const auto* ios = mappingEntryForFrom(method, publicFieldName, "ios")) {
        if (ios->contains("valueMap")) {
            return {};
        }
        if (ios->contains("transform")) {
            return ios->at("transform").get<std::string>();
        }
    }
    return {};
}

std::string transformResolverCall(const SchemaMethod& method, const std::string& publicFieldName) {
    const std::string transform = transformNameForPublicField(method, publicFieldName);
    if (transform.empty()) {
        return {};
    }
    return "appsflyer::detail::resolveTransform_" + transform;
}

std::string emitTransformHeaderDecls(const std::vector<SchemaMethod>& methods) {
    std::unordered_set<std::string> transforms;
    for (const auto& method : methods) {
        const auto fields = collectParamsProperties(method.publicApi.value("parameters", nlohmann::json::object()));
        for (const auto& field : fields) {
            const std::string t = transformNameForPublicField(method, field.name);
            if (!t.empty()) {
                transforms.insert(t);
            }
        }
        if (method.name == "validateAndLogInAppPurchase") {
            transforms.insert("androidPurchaseType");
            transforms.insert("iosPurchaseType");
        }
    }
    if (transforms.empty()) {
        return {};
    }
    std::ostringstream header;
    for (const auto& t : transforms) {
        header << "std::string resolveTransform_" << t << "(const std::string& schemaKey);\n";
    }
    return header.str();
}

std::string emitTransformCppDefs(const std::vector<SchemaMethod>& methods) {
    std::unordered_set<std::string> transforms;
    for (const auto& method : methods) {
        const auto fields = collectParamsProperties(method.publicApi.value("parameters", nlohmann::json::object()));
        for (const auto& field : fields) {
            const std::string t = transformNameForPublicField(method, field.name);
            if (!t.empty()) {
                transforms.insert(t);
            }
        }
        if (method.name == "validateAndLogInAppPurchase") {
            transforms.insert("androidPurchaseType");
            transforms.insert("iosPurchaseType");
        }
    }
    if (transforms.empty()) {
        return {};
    }

    std::ostringstream cpp;
    for (const auto& t : transforms) {
        if (t == "androidLogLevel") {
            cpp << "std::string resolveTransform_androidLogLevel(const std::string& schemaKey) {\n";
            cpp << "    static const std::unordered_map<std::string, std::string> kMap = {\n";
            cpp << "        {\"none\", \"NONE\"},\n";
            cpp << "        {\"error\", \"ERROR\"},\n";
            cpp << "        {\"warning\", \"WARNING\"},\n";
            cpp << "        {\"info\", \"INFO\"},\n";
            cpp << "        {\"debug\", \"DEBUG\"},\n";
            cpp << "        {\"verbose\", \"VERBOSE\"},\n";
            cpp << "    };\n";
            cpp << "    const auto it = kMap.find(schemaKey);\n";
            cpp << "    return it != kMap.end() ? it->second : schemaKey;\n";
            cpp << "}\n\n";
        } else if (t == "androidPurchaseType") {
            cpp << "std::string resolveTransform_androidPurchaseType(const std::string& schemaKey) {\n";
            cpp << "    if (schemaKey == \"oneTimePurchase\") {\n";
            cpp << "        return \"one_time_purchase\";\n";
            cpp << "    }\n";
            cpp << "    return schemaKey;\n";
            cpp << "}\n\n";
        } else if (t == "iosPurchaseType") {
            cpp << "std::string resolveTransform_iosPurchaseType(const std::string& schemaKey) {\n";
            cpp << "    return schemaKey;\n";
            cpp << "}\n\n";
        }
    }
    return cpp.str();
}

std::string emitValidatePurchaseTypeEnum(const std::vector<SchemaMethod>& methods) {
    const nlohmann::json* purchaseTypeEnum = validateMethodPurchaseTypeEnum(methods);
    if (purchaseTypeEnum == nullptr) {
        return {};
    }

    std::ostringstream out;
    out << "enum class PurchaseType {\n";
    for (const auto& v : *purchaseTypeEnum) {
        if (!v.is_string()) {
            continue;
        }
        std::string member = v.get<std::string>();
        if (!member.empty()) {
            member[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(member[0])));
        }
        out << "    " << member << ",\n";
    }
    out << "};\n\n";
    return out.str();
}

std::string emitValidateInAppPurchaseStructs(const std::vector<SchemaMethod>& methods) {
    const nlohmann::json* purchaseTypeEnum = validateMethodPurchaseTypeEnum(methods);
    if (purchaseTypeEnum == nullptr) {
        return {};
    }

    std::ostringstream out;
    out << "#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)\n";
    out << "struct InAppPurchaseDetails {\n";
    out << "    PurchaseType purchaseType;\n";
    out << "    std::string productId;\n";
    out << "    std::string purchaseToken;\n";
    out << "};\n";
    out << "#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)\n";
    out << "struct InAppPurchaseDetails {\n";
    out << "    PurchaseType purchaseType;\n";
    out << "    std::string productId;\n";
    out << "    std::string transactionId;\n";
    out << "};\n";
    out << "#else\n";
    out << "struct InAppPurchaseDetails {\n";
    out << "    PurchaseType purchaseType;\n";
    out << "    std::string productId;\n";
    out << "    std::string purchaseToken;\n";
    out << "    std::string transactionId;\n";
    out << "};\n";
    out << "#endif\n\n";

    out << "struct ValidateAndLogInAppPurchaseParams {\n";
    out << "    InAppPurchaseDetails purchase;\n";
    out << "    cocos2d::ValueMap additionalParameters;\n";
    out << "};\n\n";
    return out.str();
}

std::string emitValidateAndLogInAppPurchaseMethodImpl(const SchemaMethod& method) {
    (void)method;
    std::ostringstream body;
    body << "AppsFlyerTask validateAndLogInAppPurchase(const ValidateAndLogInAppPurchaseParams& params, "
            "AppsFlyerCompletion onComplete) {\n";
    body << "    const std::string purchaseTypeKey = appsflyer::detail::schemaKey_PurchaseType("
            "params.purchase.purchaseType);\n";
    body << "    std::ostringstream oss;\n";
    body << "#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)\n";
    body << "    const std::string wirePurchaseType = "
            "appsflyer::detail::resolveTransform_androidPurchaseType(purchaseTypeKey);\n";
    body << "    oss << \"{\";\n";
    body << "    oss << \"\\\"purchaseType\\\":\\\"\" << "
            "appsflyer::detail::escapeJsonString(wirePurchaseType) << \"\\\"\";\n";
    body << "    oss << \",\\\"productId\\\":\\\"\" << "
            "appsflyer::detail::escapeJsonString(params.purchase.productId) << \"\\\"\";\n";
    body << "    oss << \",\\\"purchaseToken\\\":\\\"\" << "
            "appsflyer::detail::escapeJsonString(params.purchase.purchaseToken) << \"\\\"\";\n";
    body << "    oss << \",\\\"additionalParameters\\\":\" << "
            "appsflyer::detail::encodeValueMapJson(params.additionalParameters);\n";
    body << "    oss << \"}\";\n";
    body << "#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)\n";
    body << "    const std::string wirePurchaseType = "
            "appsflyer::detail::resolveTransform_iosPurchaseType(purchaseTypeKey);\n";
    body << "    oss << \"{\";\n";
    body << "    oss << \"\\\"product\\\":{\\\"productId\\\":\\\"\" << "
            "appsflyer::detail::escapeJsonString(params.purchase.productId) << \"\\\"}\";\n";
    body << "    oss << \",\\\"transaction\\\":{\\\"transactionId\\\":\\\"\" << "
            "appsflyer::detail::escapeJsonString(params.purchase.transactionId) << \"\\\"\";\n";
    body << "    oss << \",\\\"purchaseType\\\":\\\"\" << "
            "appsflyer::detail::escapeJsonString(wirePurchaseType) << \"\\\"}\";\n";
    body << "    oss << \",\\\"additionalParameters\\\":\" << "
            "appsflyer::detail::encodeValueMapJson(params.additionalParameters);\n";
    body << "    oss << \"}\";\n";
    body << "#else\n";
    body << "    return detail::unsupportedPlatformTask(std::move(onComplete));\n";
    body << "#endif\n";
    body << "    const std::string paramsJson = oss.str();\n";
    body << "#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)\n";
    body << "    return executeRpc(\"validateAndLogInAppPurchase\", paramsJson, std::move(onComplete));\n";
    body << "#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)\n";
    body << "    return executeRpc(\"validateAndLogInAppPurchase\", paramsJson, std::move(onComplete));\n";
    body << "#endif\n";
    body << "}";
    return body.str();
}

std::string emitValidatePurchaseTypeSchemaKeyDecl() { return "std::string schemaKey_PurchaseType(PurchaseType value);\n"; }

std::string emitValidatePurchaseTypeSchemaKeyDef(const std::vector<SchemaMethod>& methods) {
    const nlohmann::json* purchaseTypeEnum = validateMethodPurchaseTypeEnum(methods);
    if (purchaseTypeEnum == nullptr) {
        return {};
    }
    return emitSchemaKeySwitch("PurchaseType", *purchaseTypeEnum);
}

std::unordered_map<std::string, std::string> generateAll(const std::vector<SchemaMethod>& methods) {
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
        rpcHeader << "#include \"../AppsFlyerDeepLinkData.h\"\n";
    }
    rpcHeader << R"(
namespace appsflyer {

)";
    rpcHeader << emitValidatePurchaseTypeEnum(methods);
    rpcHeader << emitValueMapHeaderSection(methods);

    std::ostringstream rpcDecls;
    for (const auto& m : methods) {
        if (m.isListener) {
            rpcDecls << emitListenerDecl(m);
            continue;
        }
        const auto fields = collectParamsProperties(m.publicApi.value("parameters", nlohmann::json::object()));
        const std::string structName = paramsStructName(m.name);
        if (m.name == "validateAndLogInAppPurchase") {
            rpcHeader << emitValidateInAppPurchaseStructs(methods);
        } else if (fields.size() > 1) {
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
#include "AppsFlyer/AppsFlyerRpc.h"

#include "cocos2d.h"

#include <cctype>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <unordered_map>
#include <utility>

namespace appsflyer {

)";

    for (const auto& m : methods) {
        if (m.isListener || isHandWrittenUnregisterMethod(m.name)) {
            continue;
        }
        if (m.name == "validateAndLogInAppPurchase") {
            rpcCpp << emitValidateAndLogInAppPurchaseMethodImpl(m) << "\n\n";
        } else {
            rpcCpp << emitAppsflyerMethodImpl(m) << "\n\n";
        }
    }
    rpcCpp << emitValueMapCppSection(methods);
    rpcCpp << "} // namespace appsflyer\n";

    std::ostringstream xMethodsHeader;
    xMethodsHeader << kGeneratedBanner << R"(
#ifndef AppsFlyerXSchemaMethods_generated_h
#define AppsFlyerXSchemaMethods_generated_h

#include <functional>
#include <utility>

// Inline AppsFlyerX → appsflyer:: (included inside class AppsFlyerX).

)";
    xMethodsHeader << "    using LogLevel = appsflyer::LogLevel;\n";
    if (!collectValueMapBindings(methods).empty()) {
        xMethodsHeader << "    using MediationNetwork = appsflyer::MediationNetwork;\n";
    }
    xMethodsHeader << "\n";
    for (const auto& m : methods) {
        if (m.isListener) {
            xMethodsHeader << emitListenerXWrapper(m);
            continue;
        }
        const auto fields = collectParamsProperties(m.publicApi.value("parameters", nlohmann::json::object()));
        const std::string xName = appsFlyerXMethodName(m.name);
        const std::string structName = paramsStructName(m.name);
        if (m.name == "validateAndLogInAppPurchase") {
            xMethodsHeader << "    using PurchaseType = appsflyer::PurchaseType;\n";
            xMethodsHeader << "    using InAppPurchaseDetails = appsflyer::InAppPurchaseDetails;\n";
            xMethodsHeader << "    using " << structName << " = appsflyer::" << structName << ";\n";
        } else if (fields.size() > 1) {
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
