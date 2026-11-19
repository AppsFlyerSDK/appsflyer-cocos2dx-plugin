#pragma once

#include "schema_model.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace afcodegen {

extern const char* kGeneratedBanner;

struct ValueMapBinding {
    std::string methodName;
    std::string fieldName;
    std::string resolverFn;
};

std::vector<ValueMapBinding> collectValueMapBindings(const std::vector<SchemaMethod>& methods);

std::string valueMapResolverCall(const SchemaMethod& method, const std::string& fieldName);

std::string emitValueMapHeaderSection(const std::vector<SchemaMethod>& methods);
std::string emitValueMapCppSection(const std::vector<SchemaMethod>& methods);

std::string transformNameForPublicField(const SchemaMethod& method, const std::string& publicFieldName);

std::string transformResolverCall(const SchemaMethod& method, const std::string& publicFieldName);

std::string emitTransformHeaderDecls(const std::vector<SchemaMethod>& methods);

std::string emitTransformCppDefs(const std::vector<SchemaMethod>& methods);

std::string emitValidatePurchaseTypeEnum(const std::vector<SchemaMethod>& methods);

std::string emitValidateInAppPurchaseStructs(const std::vector<SchemaMethod>& methods);

std::string emitValidateAndLogInAppPurchaseMethodImpl(const SchemaMethod& method);

std::string emitValidatePurchaseTypeSchemaKeyDecl();

std::string emitValidatePurchaseTypeSchemaKeyDef(const std::vector<SchemaMethod>& methods);

std::unordered_map<std::string, std::string> generateAll(
    const std::vector<SchemaMethod>& methods);

} // namespace afcodegen