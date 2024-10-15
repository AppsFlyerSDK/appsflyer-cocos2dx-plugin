//
//  AFXAdRevenueData.cpp
//  MyGame
//
//  Created by ivan.obodovskyi on 04.10.2024.
//

#include <stdio.h>
#include "AFXAdRevenueData.h"

// Constructor definition
AFXAdRevenueData::AFXAdRevenueData(const std::string& monetizationNetwork,
                                 AppsFlyerXAdRevenueMediationNetworkType mediationNetwork,
                                 const std::string& currencyIso4217Code,
                                 double eventRevenue)
    : monetizationNetwork(monetizationNetwork),
      mediationNetwork(mediationNetwork),
      currencyIso4217Code(currencyIso4217Code),
      eventRevenue(eventRevenue) {
}

// Getters implementation
std::string AFXAdRevenueData::getMonetizationNetwork() const {
    return monetizationNetwork;
}

AppsFlyerXAdRevenueMediationNetworkType AFXAdRevenueData::getMediationNetwork() const {
    return mediationNetwork;
}

std::string AFXAdRevenueData::meditationNetworkString() const  {
    switch (mediationNetwork) {
    case AppsFlyerXAdRevenueMediationNetworkType::GoogleAdMob:
        return "GOOGLE_ADMOB";
    case AppsFlyerXAdRevenueMediationNetworkType::IronSource:
        return "IRONSOURCE";
    case AppsFlyerXAdRevenueMediationNetworkType::ApplovinMax:
        return "APPLOVIN_MAX";
    case AppsFlyerXAdRevenueMediationNetworkType::Fyber:
        return "FYBER";
    case AppsFlyerXAdRevenueMediationNetworkType::Appodeal:
        return "APPODEAL";
    case AppsFlyerXAdRevenueMediationNetworkType::Admost:
        return "ADMOST";
    case AppsFlyerXAdRevenueMediationNetworkType::Topon:
        return "TOPON";
    case AppsFlyerXAdRevenueMediationNetworkType::Tradplus:
        return "TRADPLUS";
    case AppsFlyerXAdRevenueMediationNetworkType::Yandex:
        return "YANDEX";
    case AppsFlyerXAdRevenueMediationNetworkType::ChartBoost:
        return "CHARTBOOST";
    case AppsFlyerXAdRevenueMediationNetworkType::Unity:
        return "UNITY";
    case AppsFlyerXAdRevenueMediationNetworkType::ToponPte:
        return "TOPON_PTE";
    case AppsFlyerXAdRevenueMediationNetworkType::Custom:
        return "CUSTOM_MEDIATION";
    case AppsFlyerXAdRevenueMediationNetworkType::DirectMonetization:
        return "DIRECT_MONETIZATION_NETWORK";
    default:
        return "unknown";
    }
};

std::string AFXAdRevenueData::getCurrencyIso4217Code() const {
    return currencyIso4217Code;
}

double AFXAdRevenueData::getEventRevenue() const {
    return eventRevenue;
}

int convertMeditationType(AppsFlyerXAdRevenueMediationNetworkType a) {
    return static_cast<int>(static_cast<int>(a));
}
