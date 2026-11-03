#pragma once

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace afcodegen {

struct ParamField {
    std::string name;
    std::string cppType;
    bool required = false;
    bool hasDefault = false;
    nlohmann::json defaultValue;
};

struct SchemaMethod {
    std::string name;
    nlohmann::json rpc;
    nlohmann::json publicApi;
    nlohmann::json plugins;
    nlohmann::json mappings;
    bool isListener = false;
};

std::vector<SchemaMethod> loadEligibleMethods(const nlohmann::json& schemaRoot);

std::vector<ParamField> collectParamsProperties(const nlohmann::json& publicApiParameters);

std::string paramsStructName(const std::string& methodName);

std::string appsFlyerXMethodName(const std::string& schemaName);

std::string escapeCppStringLiteral(const std::string& value);

} // namespace afcodegen
