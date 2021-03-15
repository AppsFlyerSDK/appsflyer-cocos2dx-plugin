
//  AppsFlyerXAppleDeepLinkResult.h
//  AppsFlyerCocos2dX
//
//  Created by Margot Guetta on 15/12/2020.



#ifndef AppsFlyerXAppleDeepLinkResult_h
#define AppsFlyerXAppleDeepLinkResult_h

#include <string>
#include "cocos2d.h"


enum DeepLinkResultStatus {FOUND, NOTFOUND, FAILURE};
enum DeepLinkError {NONE, TIMEOUT, NETWORK, HTTP_STATUS_CODE, UNEXPECTED};

class AppsFlyerXDeepLinkResult {

private:
    std::string getDeepLinkParameter(std::string key);

    public :
     DeepLinkResultStatus status;
     DeepLinkError error;
    cocos2d::ValueMap deepLink;
    std::string getMatchType();
    std::string getDeepLinkValue();;
    std::string getClickHttpReferrer();
    std::string getMediaSource();
    std::string getCampaign();
    std::string getCampaignId();
    std::string getAfSub1();
    std::string getAfSub2();
    std::string getAfSub3();
    std::string getAfSub4();
    std::string getAfSub5();
    bool isDeferred();



};

#endif
