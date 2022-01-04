//
//  AppsFlyerXApple.h
//  AppsFlyerCocos2dX
//
//  Created by AndreyG-AF on 10/4/17.
//

#ifndef AppsFlyerXApple_h
#define AppsFlyerXApple_h

#include <iostream>
#include "EmailCryptTypeX.h"
//#include "libAppsFlyer/AppsFlyerLib.h"
//#include "AppsFlyerXApple.h"
//#include "AppsFlyerXAppleHelper.h"
//#include "AppsFlyerXAppleDelegate.h"
//#include "EmailCryptTypeX.h"
#include "AppsFlyerXDeepLinkResult.h"

class AppsFlyerXApple {
private:
    /* Here will be the instance stored. */
    static AppsFlyerXApple* instance;
    
    /* Private constructor to prevent instancing. */
    AppsFlyerXApple();
    
    
    void * deepLinkDelegate;
    
    
    void * delegate;
public:
    
    static AppsFlyerXApple* getInstance();
    
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
    
    static void setShouldCollectDeviceName(bool isShouldCollectDeviceName);
    static bool isShouldCollectDeviceName();
    
    static void setAppInviteOneLink(const std::string& appInviteOneLinkID);
    static std::string appInviteOneLinkID();
    
    static void anonymizeUser(bool shouldAnonymize);
    static bool isAnonymizedUser();
    
    static void setDisableCollectASA(bool shouldDisable);
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
    
    // static std::string getSDKVersion();
    
    static void remoteDebuggingCallWithData(const std::string& data);
    
    static void setHost(const std::string& host);
    static std::string getHost();
    
    static void setMinTimeBetweenSessions(unsigned long minTimeBetweenSessions);
    static unsigned long getMinTimeBetweenSessions();

    static void stop(bool stop);
    
    
    // Delegates methods proxy    
    static void setOnConversionDataReceived(void(*callback)(cocos2d::ValueMap installData));
    static void setOnConversionDataRequestFailure(void(*callback)(cocos2d::ValueMap error));
    static void setOnAppOpenAttribution(void(*callback)(cocos2d::ValueMap attributionData));
    static void setOnAppOpenAttributionFailure(void(*callback)(cocos2d::ValueMap error));

    //Sharing data filter
    static void sharingFilter(std::vector<std::string> partners);
    static void sharingFilterForAllPartners();
    
    static void disableSKAdNetwork(bool shouldDisable);
    static bool isDisabledSKAdNetwork();
    
    static void  waitForATTUserAuthorizationWithTimeoutInterval(double timeoutInterval);
    
    static void setPhoneNumber(const std::string& phoneNumber);
    
    //DDL
    static void setDidResolveDeepLink(void(*callback)(AppsFlyerXDeepLinkResult result));
    
    static void setPartnerData(const std::string& partnerId, cocos2d::ValueMap data);
    static void setOneLinkCustomDomains(std::vector<std::string> domains);
     
    static void setCurrentDeviceLanguage(const std::string& language);
    
    static void setSharingFilterForPartners(std::vector<std::string> partners);
    
    //user-invite
    static void logInvite(const std::string& channel, cocos2d::ValueMap parameters);
    static void generateUserInviteLink(cocos2d::ValueMap parameters, std::function<void(std::string url)> callback);
};

#endif /* AppsFlyerXApple_h */
