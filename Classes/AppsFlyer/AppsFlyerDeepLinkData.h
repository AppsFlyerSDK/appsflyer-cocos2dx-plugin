#ifndef AppsFlyerDeepLinkData_h
#define AppsFlyerDeepLinkData_h

#include "cocos2d.h"

#include <algorithm>
#include <cctype>
#include <string>

namespace appsflyer {

/// Normalized deep-link resolution status
enum class DeepLinkStatus { Found, NotFound, Error };

/// Unified deep-link callback payload (`registerDeepLinkListener` / `onDeepLinking`).
struct DeepLinkData {
    DeepLinkStatus status = DeepLinkStatus::NotFound;
    /// Platform error detail when status is Error (Android type string or iOS message).
    std::string error;
    /// Click / UDL fields (e.g. `deep_link_value`, `media_source`) when present.
    cocos2d::ValueMap deepLink;

    std::string deepLinkStringField(const std::string& key) const {
        const auto it = deepLink.find(key);
        if (it == deepLink.end() || it->second.getType() != cocos2d::Value::Type::STRING) {
            return {};
        }
        return it->second.asString();
    }
};

inline std::string deepLinkStatusToWire(DeepLinkStatus status) {
    switch (status) {
    case DeepLinkStatus::Found:
        return "FOUND";
    case DeepLinkStatus::NotFound:
        return "NOT_FOUND";
    case DeepLinkStatus::Error:
        return "ERROR";
    }
    return "ERROR";
}

/// Collapses iOS/Android raw status strings onto Found / NotFound / Error
inline DeepLinkStatus deepLinkStatusFromWire(const std::string& raw) {
    std::string normalized;
    normalized.reserve(raw.size());
    for (char c : raw) {
        if (c == '_' || c == '-') {
            continue;
        }
        normalized.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }
    if (normalized == "found") {
        return DeepLinkStatus::Found;
    }
    if (normalized == "notfound") {
        return DeepLinkStatus::NotFound;
    }
    if (normalized == "error" || normalized == "failure") {
        return DeepLinkStatus::Error;
    }
    return DeepLinkStatus::Error;
}

} // namespace appsflyer

#endif