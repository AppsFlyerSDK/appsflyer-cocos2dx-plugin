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

std::string AFXAdRevenueData::getCurrencyIso4217Code() const {
    return currencyIso4217Code;
}

double AFXAdRevenueData::getEventRevenue() const {
    return eventRevenue;
}

int convertMeditationType(AppsFlyerXAdRevenueMediationNetworkType a) {
    return static_cast<int>(static_cast<int>(a));
}
