
//  NSObject+AppsFlyerXAppleDeepLinkResult.m
//  AppsFlyerCocos2dX
//
//  Created by Margot Guetta on 15/12/2020.
//

#include "AppsFlyerXDeepLinkResult.h"


std::string AppsFlyerXDeepLinkResult::getDeepLinkParameter(std::string key){
    if (!deepLink.empty() && deepLink.find(key) != deepLink.end())
    {
        return deepLink.find(key)->second.asString();
    }
    return "";
}

std::string AppsFlyerXDeepLinkResult::getMatchType(){
    return getDeepLinkParameter("match_type");
}
std::string AppsFlyerXDeepLinkResult:: getDeepLinkValue(){
    return getDeepLinkParameter("deep_link_value");
}

std::string AppsFlyerXDeepLinkResult::getClickHttpReferrer(){
    return getDeepLinkParameter("click_http_referrer");
}
std::string AppsFlyerXDeepLinkResult::getMediaSource(){
    return getDeepLinkParameter("media_source");
}
std::string AppsFlyerXDeepLinkResult::getCampaign(){
    return getDeepLinkParameter("campaign");
}
std::string AppsFlyerXDeepLinkResult::getCampaignId(){
    return getDeepLinkParameter("campaign_id");
}
std::string AppsFlyerXDeepLinkResult::getAfSub1(){
    return getDeepLinkParameter("af_sub1");
}
std::string AppsFlyerXDeepLinkResult::getAfSub2(){
    return getDeepLinkParameter("af_sub2");
}
std::string AppsFlyerXDeepLinkResult::getAfSub3(){
    return getDeepLinkParameter("af_sub3");
}
std::string AppsFlyerXDeepLinkResult::getAfSub4(){
    return getDeepLinkParameter("af_sub3");
}
std::string AppsFlyerXDeepLinkResult::getAfSub5(){
    return getDeepLinkParameter("af_sub4");
}

bool AppsFlyerXDeepLinkResult::isDeferred(){
    if (!deepLink.empty() && deepLink.find("isDeferred") != deepLink.end())
    {
        return deepLink.find("isDeferred")->second.asBool();
    }
    return NULL;
}
