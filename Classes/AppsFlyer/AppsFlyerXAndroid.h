//
// Created by Maxim Shoustin on 10/9/17.
//

#ifndef PROJ_ANDROID_STUDIO_APPSFLYERXANDROID_H
#define PROJ_ANDROID_STUDIO_APPSFLYERXANDROID_H
#include "cocos2d.h"
#include <iostream>
#include <vector>

#include "EmailCryptTypeX.h"
#include "AppsFlyerXDeepLinkResult.h"

class AppsFlyerXAndroid {
private:
    /* Here will be the instance stored. */
    static AppsFlyerXAndroid* instance;

    /* Private constructor to prevent instancing. */
    AppsFlyerXAndroid();

    void * delegate;
public:

    static AppsFlyerXAndroid* getInstance();

    static void didEnterBackground();

    static void setCustomerUserID(const std::string& customerUserID);
    static std::string customerUserID();

    static void setAdditionalData(cocos2d::ValueMap customData);
    static cocos2d::ValueMap customData();

    static void setAppsFlyerDevKey(const std::string& appsFlyerDevKey);
    static std::string appsFlyerDevKey();

    static void setCurrencyCode(const std::string& currencyCode);
    static std::string currencyCode();

    static void disableAppleAdSupportTracking(bool isDisableAppleAdSupportTracking);
    //static bool isDisableAppleAdSupportTracking();

    static void setIsDebug(bool isDebug);
    //never used
    //static bool isDebug();

    static void setShouldCollectDeviceName(bool isShouldCollectDeviceName);
    //static bool isShouldCollectDeviceName();

    static void setAppInviteOneLink(const std::string& appInviteOneLinkID);
    static std::string appInviteOneLinkID();

    static void anonymizeUser(bool shouldAnonymize);
    static bool isAnonymizedUser();

    static void disableIAdTracking(bool disableIAdTracking);
    static bool isDisableIAdTracking();

    static void setUseReceiptValidationSandbox(bool useReceiptValidationSandbox);
    //never used
    //static bool isUseReceiptValidationSandbox();


    static void setAdvertiserId(const std::string& advertiserId);
    static std::string advertiserId();

    static void setUserEmails(std::vector<std::string> userEmails, EmailCryptTypeX type);

    static void start();

    static void logEvent(const std::string& eventName, const std::string& value);

    static void logEvent(const std::string& eventName, cocos2d::ValueMap values);

    static void validateAndLogInAppPurchase(std::string productIdentifier, std::string price, std::string currency, std::string tranactionId, cocos2d::ValueMap params, void (*successBlock)(cocos2d::ValueMap response), void (*failedBlock)(void* error, void* responce));

    static void logLocation(double longitude, double latitude);

    static std::string getAppsFlyerUID();

    static void validateAndLogInAppPurchase(const std::string& publicKey,
                                                          const std::string& signature,
                                                          const std::string& purchaseData,
                                                          const std::string& price,
                                                          const std::string& currency,
                                                          cocos2d::ValueMap additionalParameters);

    // - (void) loadConversionDataWithDelegate:(id<AppsFlyerTrackerDelegate>) delegate __attribute__((deprecated));

    /*
     - (BOOL) continueUserActivity:(NSUserActivity *) userActivity restorationHandler:(void (^)(NSArray *))restorationHandler NS_AVAILABLE_IOS(9_0);
     - (void) didUpdateUserActivity:(NSUserActivity *)userActivity NS_AVAILABLE_IOS(9_0);
     */

    static void handlePushNotification(cocos2d::ValueMap pushPayload);

    static void registerUninstall(const std::string& token);

    // static std::string getSDKVersion();

    static void remoteDebuggingCallWithData(const std::string& data);

    static void setHost(const std::string& host);
    static std::string getHost();

    static void setMinTimeBetweenSessions(unsigned long minTimeBetweenSessions);
    static unsigned long getMinTimeBetweenSessions();


    static void stop(bool shouldStop);


    // Delegates methods proxy
    static void setOnConversionDataSuccess(void(*callback)(cocos2d::ValueMap installData));
    static void setOnConversionDataFail(void(*callback)(cocos2d::ValueMap error));
    static void setOnAppOpenAttribution(void(*callback)(cocos2d::ValueMap attributionData));
    static void setOnAppOpenAttributionFailure(void(*callback)(cocos2d::ValueMap error));
    static void setOnResponse(void(*callback)(std::string oneLinkURL));
    static void setOnResponseError(void(*callback)(std::string message));

      //Sharing data filter
    static void sharingFilter(std::vector<std::string> partners);
    static void sharingFilterForAllPartners();

    //ddl delegate
    static void setOnDeepLinking(void(*callback)(AppsFlyerXDeepLinkResult result));

    static void setPartnerData(const std::string& partnerId, cocos2d::ValueMap data);

    static void setOneLinkCustomDomains(std::vector<std::string> domains);

    static void setDisableAdvertisingIdentifiers(bool disable);

    static void setSharingFilterForPartners(std::vector<std::string> partners);

    static void generateInviteUrl(cocos2d::ValueMap parameter, void(*onResponse)(std::string url), void(*onResponseError)(std::string url));
    static void logInvite(const std::string& channel,cocos2d::ValueMap parameters);

    static void setDisableNetworkData(bool disable);
};


#endif //PROJ_ANDROID_STUDIO_APPSFLYERXANDROID_H

