//
// Created by Maxim Shoustin on 10/9/17.
//

#ifndef PROJ_ANDROID_STUDIO_APPSFLYERXANDROID_H
#define PROJ_ANDROID_STUDIO_APPSFLYERXANDROID_H
#include "cocos2d.h"
#include <iostream>
#include <vector>

#include "EmailCryptTypeX.h"

class AppsFlyerXAndroid {
private:
    /* Here will be the instance stored. */
    static AppsFlyerXAndroid* instance;

    /* Private constructor to prevent instancing. */
    AppsFlyerXAndroid();

    void * delegate;
public:

    static AppsFlyerXAndroid* getInstance();

    static void setCustomerUserID(const std::string& customerUserID);
    static std::string customerUserID();

    static void setAdditionalData(cocos2d::ValueMap customData);
    static cocos2d::ValueMap customData();

    static void setAppsFlyerDevKey(const std::string& appsFlyerDevKey);
    static std::string appsFlyerDevKey();

    static void setCurrencyCode(const std::string& currencyCode);
    static std::string currencyCode();

    static void disableAppleAdSupportTracking(bool isDisableAppleAdSupportTracking);
    static bool isDisableAppleAdSupportTracking();

    static void setIsDebug(bool isDebug);
    static bool isDebug();

    static void setShouldCollectDeviceName(bool isShouldCollectDeviceName);
    static bool isShouldCollectDeviceName();

    static void setAppInviteOneLink(std::string& appInviteOneLinkID);
    static std::string appInviteOneLinkID();

    static void deviceTrackingDisabled(bool deviceTrackingDisabled);
    static bool isDeviceTrackingDisabled();

    static void disableIAdTracking(bool disableIAdTracking);
    static bool isDisableIAdTracking();

    static void setUseReceiptValidationSandbox(bool useReceiptValidationSandbox);
    static bool isUseReceiptValidationSandbox();


    static void setAdvertiserId(const std::string& advertiserId);
    static std::string advertiserId();

    static void setUserEmails(std::vector<std::string> userEmails, EmailCryptTypeX type);

    static void startTracking();

    static void trackEvent(const std::string& eventName, const std::string& value);

    static void trackEvent(const std::string& eventName, cocos2d::ValueMap values);

    static void validateAndTrackInAppPurchase(std::string productIdentifier, std::string price, std::string currency, std::string tranactionId, cocos2d::ValueMap params, void (*successBlock)(cocos2d::ValueMap response), void (*failedBlock)(void* error, void* responce));

    static void trackLocation(double longitude, double latitude);

    static std::string getAppsFlyerUID();

    static void validateAndTrackInAppPurchase(const std::string& publicKey,
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

    static std::string getSDKVersion();

    static void remoteDebuggingCallWithData(const std::string& data);

    static void setHost(const std::string& host);
    static std::string getHost();

    static void setMinTimeBetweenSessions(unsigned long minTimeBetweenSessions);
    static unsigned long getMinTimeBetweenSessions();

    // Delegates methods proxy


    static void setOnConversionDataReceived(void(*callback)(cocos2d::ValueMap installData));
    static void setOnConversionDataRequestFailure(void(*callback)(cocos2d::ValueMap error));
    static void setOnAppOpenAttribution(void(*callback)(cocos2d::ValueMap attributionData));
    static void setOnAppOpenAttributionFailure(void(*callback)(cocos2d::ValueMap error));
};

#endif //PROJ_ANDROID_STUDIO_APPSFLYERXANDROID_H

