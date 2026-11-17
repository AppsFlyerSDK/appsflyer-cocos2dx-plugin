#ifndef AppsFlyerResult_h
#define AppsFlyerResult_h

#include <functional>
#include <string>

struct AppsFlyerError {
    int code = 0;
    std::string message;
};

struct AppsFlyerResult {
    bool success = false;
    std::string dataJson;
    bool hasError = false;
    AppsFlyerError error;
};

using AppsFlyerCompletion = std::function<void(const AppsFlyerResult&)>;

#endif
