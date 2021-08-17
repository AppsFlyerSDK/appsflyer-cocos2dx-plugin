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
#include "AppsFlyerXMacro.h"
#include "AppsFlyerXDeepLinkResult.h"

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
    
    static void disableAdvertisingIdentifier(bool shouldDisable);
    static bool isDisabledAdvertisingIdentifier();
    
    static void setIsDebug(bool isDebug);
    static bool isDebug();

    static void didEnterBackground();
    
    static void setShouldCollectDeviceName(bool isShouldCollectDeviceName);
    static bool isShouldCollectDeviceName();
    
    static void setAppInviteOneLink(std::string& appInviteOneLinkID);
    static std::string appInviteOneLinkID();
    
    static void anonymizeUser(bool shouldAnonymize);
    static bool isAnonymizedUser();
    
    static void disableCollectASA(bool shouldDisable);
    static bool isDisabledCollectASA();

    static void setUseReceiptValidationSandbox(bool useReceiptValidationSandbox);
    static bool isUseReceiptValidationSandbox();
    
    static void setUseUninstallSandbox(bool setUseUninstallSandbox);
    static bool isUseUninstallSandbox();
    
    static void setUserEmails(std::vector<std::string> userEmails, EmailCryptTypeX type);
    
    static void start();
        
    static void logEvent(const std::string& eventName, cocos2d::ValueMap values);
    
    static void validateAndLogInAppPurchase(const std::string& productIdentifier,
                                              const std::string& price,
                                              const std::string& currency,
                                              const std::string& tranactionId,
                                              cocos2d::ValueMap params,
                                              std::function<void(cocos2d::ValueMap)> successBlock,
                                              std::function<void(cocos2d::ValueMap)> failureBlock);

    static void validateAndLogInAppPurchase(const std::string& publicKey,
                                                     const std::string& signature,
                                                     const std::string& purchaseData,
                                                     const std::string& price,
                                                     const std::string& currency,
                                                     cocos2d::ValueMap additionalParameters);

    static void logLocation(double longitude, double latitude);
    
    static std::string getAppsFlyerUID();
    

    
    static void handleOpenURL(const std::string& url, const std::string& sourceApplication);
    
    static void handleOpenURL(std::string url, std::string sourceApplication, void* annotation);
    
    static void handleOpenURL(std::string url, cocos2d::ValueMap options);
    
    /*
     - (BOOL) continueUserActivity:(NSUserActivity *) userActivity restorationHandler:(void (^)(NSArray *))restorationHandler NS_AVAILABLE_IOS(9_0);
     - (void) didUpdateUserActivity:(NSUserActivity *)userActivity NS_AVAILABLE_IOS(9_0);
     */
    
    static void handlePushNotification(cocos2d::ValueMap pushPayload);
    
    static void registerUninstall(void* deviceToken, unsigned long length);

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

    //Sharing Data filter
    static void sharingFilter(std::vector<std::string> partners);
    static void sharingFilterForAllPartners();
    
    static void waitForATTUserAuthorizationWithTimeoutInterval(double timeoutInterval);
    
    static void disableSKAdNetwork(bool shouldDisable);
    static bool isDisabledSKAdNetwork();
    
    static void setPhoneNumber(const std::string& phoneNumber);
    
    //ddl
    static void setDidResolveDeepLink(void(*callback)(AppsFlyerXDeepLinkResult result));

    static void setPartnerData(std::string partnerId, cocos2d::ValueMap data);
    
    static void setOneLinkCustomDomains(std::vector<std::string> domains);
    
    static void setCurrentDeviceLanguage(const std::string& language);
};
#endif /* A;ppsFlyerX_h */



