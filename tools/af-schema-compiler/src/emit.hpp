#pragma once

#include "schema_model.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace afcodegen {

extern const char* kGeneratedBanner;

std::unordered_map<std::string, std::string> emitAll(
    const std::vector<SchemaMethod>& methods);

} // namespace afcodegen
