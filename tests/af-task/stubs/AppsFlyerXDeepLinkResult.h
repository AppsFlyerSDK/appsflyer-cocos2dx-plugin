#ifndef AppsFlyerXDeepLinkResult_test_stub
#define AppsFlyerXDeepLinkResult_test_stub

#include "cocos2d.h"

#include <string>

enum DeepLinkStatus { NOTFOUND, FOUND, ERROR };

class AppsFlyerXDeepLinkResult {
public:
    DeepLinkStatus status = NOTFOUND;
    cocos2d::ValueMap deepLink;

    std::string getMediaSource() const { return {}; }
};

#endif
