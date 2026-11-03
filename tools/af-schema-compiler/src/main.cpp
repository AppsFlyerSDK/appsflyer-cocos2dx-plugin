#include "emit.hpp"
#include "schema_model.hpp"

#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

static fs::path repoRootFromArgv(int argc, char** argv) {
    if (argc >= 2) {
        return fs::absolute(argv[1]);
    }
    return fs::absolute(fs::path(__FILE__).parent_path().parent_path().parent_path().parent_path());
}

int main(int argc, char** argv) {
    const fs::path repoRoot = repoRootFromArgv(argc, argv);
    const fs::path schemaPath = repoRoot / "schema/appsflyer-plugins-rpc-schema.json";
    const fs::path outDir = repoRoot / "Classes/AppsFlyer/generated";

    std::ifstream in(schemaPath);
    if (!in) {
        std::cerr << "Failed to open schema: " << schemaPath << "\n";
        return 1;
    }

    nlohmann::json schema;
    try {
        in >> schema;
    } catch (const std::exception& ex) {
        std::cerr << "Schema parse error: " << ex.what() << "\n";
        return 1;
    }

    const auto methods = afcodegen::loadEligibleMethods(schema);
    const auto files = afcodegen::emitAll(methods);

    std::error_code ec;
    fs::create_directories(outDir, ec);

    for (const auto& entry : files) {
        const fs::path outPath = outDir / entry.first;
        std::ofstream out(outPath);
        if (!out) {
            std::cerr << "Failed to write: " << outPath << "\n";
            return 1;
        }
        out << entry.second;
        std::cout << "Wrote " << fs::relative(outPath, repoRoot) << " (" << methods.size()
                  << " methods)\n";
    }

    const fs::path legacyXCpp = outDir / "AppsFlyerX.generated.cpp";
    if (fs::exists(legacyXCpp)) {
        fs::remove(legacyXCpp, ec);
        if (!ec) {
            std::cout << "Removed stale " << fs::relative(legacyXCpp, repoRoot) << "\n";
        }
    }

    return 0;
}
