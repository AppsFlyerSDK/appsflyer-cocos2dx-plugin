//
//  AppsFlyerX.h
//  AppsFlyerCocos2dX
//
//  Created by Andrey Gagan
//  Copyright © 2017 AppsFlyer. All rights reserved.
//

#ifndef AppsFlyerX_h
#define AppsFlyerX_h

#include "cocos2d.h"
#include <iostream>

#include "EmailCryptTypeX.h"

class AppsFlyerX {
public:
    
    static void setCustomerUserID(const std::string& customerUserID);
    static std::string customerUserID();
    
    static void setAdditionalData(cocos2d::ValueMap customData);
    static cocos2d::ValueMap customData();
    
    static void setAppsFlyerDevKey(const std::string& appsFlyerDevKey);
    static std::string appsFlyerDevKey();
    
    static void setAppleAppID(const std::string& appleAppID);
    static std::string appleAppID();
    
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
    
    static void setUseUninstallSandbox(bool setUseUninstallSandbox);
    static bool isUseUninstallSandbox();
    
    static void setAdvertiserId(const std::string& advertiserId);
    static std::string advertiserId();
    
    static void setUserEmails(std::vector<std::string> userEmails, EmailCryptTypeX type);
    
    static void trackAppLaunch();
    
    static void trackEvent(const std::string& eventName, const std::string& value);
    
    static void trackEvent(const std::string& eventName, cocos2d::ValueMap values);
    
    static void validateAndTrackInAppPurchase(const std::string& productIdentifier,
                                              const std::string& price,
                                              const std::string& currency,
                                              const std::string& tranactionId,
                                              cocos2d::ValueMap params,
                                              std::function<void(cocos2d::ValueMap)> successBlock,
                                              std::function<void(cocos2d::ValueMap)> failureBlock);
    
    static void trackLocation(double longitude, double latitude);
    
    static std::string getAppsFlyerUID();
    
    // - (void) loadConversionDataWithDelegate:(id<AppsFlyerTrackerDelegate>) delegate __attribute__((deprecated));
    
    static void handleOpenURL(const std::string& url, const std::string& sourceApplication);
    
    static void handleOpenURL(std::string url, std::string sourceApplication, void* annotation);
    
    static void handleOpenURL(std::string url, cocos2d::ValueMap options);
    
    /*
     - (BOOL) continueUserActivity:(NSUserActivity *) userActivity restorationHandler:(void (^)(NSArray *))restorationHandler NS_AVAILABLE_IOS(9_0);
     - (void) didUpdateUserActivity:(NSUserActivity *)userActivity NS_AVAILABLE_IOS(9_0);
     */
    
    static void handlePushNotification(cocos2d::ValueMap pushPayload);
    
    static void registerUninstall(void* deviceToken, unsigned long length);
    
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

#endif /* AppsFlyerX_h */



