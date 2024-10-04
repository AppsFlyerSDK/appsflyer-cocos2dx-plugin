//
//  AFXAdRevenueData.h
//  MyGame
//
//  Created by ivan.obodovskyi on 04.10.2024.
//

#include <string>

#ifndef AFXAdRevenueData_h
#define AFXAdRevenueData_h

enum class AppsFlyerXAdRevenueMediationNetworkType {
    GoogleAdMob = 1,
    IronSource = 2,
    ApplovinMax = 3,
    Fyber = 4,
    Appodeal = 5,
    Admost = 6,
    Topon = 7,
    Tradplus = 8,
    Yandex = 9,
    ChartBoost = 10,
    Unity = 11,
    ToponPte = 12,
    Custom = 13,
    DirectMonetization = 14
};

#define kAppsFlyerAdRevenueMonetizationNetwork         @"monetization_network"
#define kAppsFlyerAdRevenueMediationNetwork            @"mediation_network"
#define kAppsFlyerAdRevenueEventRevenue                @"event_revenue"
#define kAppsFlyerAdRevenueEventRevenueCurrency        @"event_revenue_currency"
#define kAppsFlyerAdRevenueCustomParameters            @"custom_parameters"
#define kAFADRWrapperTypeGeneric                       @"adrevenue_sdk"

//Pre-defined keys for non-mandatory dictionary

//Code ISO 3166-1 format
#define kAppsFlyerAdRevenueCountry                     @"country"

//ID of the ad unit for the impression
#define kAppsFlyerAdRevenueAdUnit                      @"ad_unit"

//Format of the ad
#define kAppsFlyerAdRevenueAdType                      @"ad_type"

//ID of the ad placement for the impression
#define kAppsFlyerAdRevenuePlacement                   @"placement"


class AFXAdRevenueData {
public:
    // Constructor with initialization parameters
    AFXAdRevenueData(const std::string& monetizationNetwork,
                    AppsFlyerXAdRevenueMediationNetworkType mediationNetwork,
                    const std::string& currencyIso4217Code,
                    double eventRevenue);

    // Getter functions for each property
    std::string getMonetizationNetwork() const;
    AppsFlyerXAdRevenueMediationNetworkType getMediationNetwork() const;
    std::string getCurrencyIso4217Code() const;
    double getEventRevenue() const;
    int convertMeditationType(AppsFlyerXAdRevenueMediationNetworkType a);

private:
    // Private member variables
    std::string monetizationNetwork;
    AppsFlyerXAdRevenueMediationNetworkType mediationNetwork;
    std::string currencyIso4217Code;
    double eventRevenue;
};


#endif /* AFXAdRevenueData_h */
