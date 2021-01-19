
//  NSObject+AppsFlyerXAppleDeepLinkResult.m
//  AppsFlyerCocos2dX
//
//  Created by Margot Guetta on 15/12/2020.
//

#include "AppsFlyerXAppleDeepLinkResult.h"


std::string AppsFlyerXAppleDeepLinkResult::getDeepLinkParameter(std::string key){
    if (!deepLink.empty() && deepLink.find(key) != deepLink.end())
    {
        return deepLink.find(key)->second.asString();
    }
    return NULL;
}

std::string AppsFlyerXAppleDeepLinkResult::getMatchType(){
    return getDeepLinkParameter("match_type");
}
std::string AppsFlyerXAppleDeepLinkResult:: getDeepLinkValue(){
    return getDeepLinkParameter("deep_link_value");
}

std::string AppsFlyerXAppleDeepLinkResult::getClickHttpReferrer(){
    return getDeepLinkParameter("click_http_referrer");
}
std::string AppsFlyerXAppleDeepLinkResult::getMediaSource(){
    return getDeepLinkParameter("media_source");
}
std::string AppsFlyerXAppleDeepLinkResult::getCampaign(){
    return getDeepLinkParameter("campaign");
}
std::string AppsFlyerXAppleDeepLinkResult::getCampaignId(){
    return getDeepLinkParameter("campaign_id");
}
std::string AppsFlyerXAppleDeepLinkResult::getAfSub1(){
    return getDeepLinkParameter("af_sub1");
}
std::string AppsFlyerXAppleDeepLinkResult::getAfSub2(){
    return getDeepLinkParameter("af_sub2");
}
std::string AppsFlyerXAppleDeepLinkResult::getAfSub3(){
    return getDeepLinkParameter("af_sub3");
}
std::string AppsFlyerXAppleDeepLinkResult::getAfSub4(){
    return getDeepLinkParameter("af_sub3");
}
std::string AppsFlyerXAppleDeepLinkResult::getAfSub5(){
    return getDeepLinkParameter("af_sub4");
}

bool AppsFlyerXAppleDeepLinkResult::isDeferred(){
    if (!deepLink.empty() && deepLink.find("isDeferred") != deepLink.end())
    {
        return deepLink.find("isDeferred")->second.asBool();
    }
    return NULL;
}
