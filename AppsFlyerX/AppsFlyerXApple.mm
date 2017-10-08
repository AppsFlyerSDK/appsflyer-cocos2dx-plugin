//
//  AppsFlyerX.mm
//  AppsFlyerCocos2dX
//
//  Created by Andrey Gagan
//  Copyright © 2017 AppsFlyer. All rights reserved.
//

#include "AppsFlyerXApple.h"
#include "AppsFlyerXAppleHelper.h"
#include "AppsFlyerXAppleDelegate.h"
#include "libAppsFlyer/AppsFlyerTracker.h"

/* Null, because instance will be initialized on demand. */
AppsFlyerXApple* AppsFlyerXApple::instance = 0;

AppsFlyerXApple* AppsFlyerXApple::getInstance() {
    if (instance == 0) {
        instance = new AppsFlyerXApple();
    }
    
    return instance;
}

AppsFlyerXApple::AppsFlyerXApple() {}

void AppsFlyerXApple::setCustomerUserID(const std::string& customerUserID) {
    [[AppsFlyerTracker sharedTracker] setCustomerUserID: [NSString stringWithUTF8String:customerUserID.c_str()]];
}

std::string AppsFlyerXApple::customerUserID(){
    return [[[AppsFlyerTracker sharedTracker] customerUserID] UTF8String];
}

void AppsFlyerXApple::setAdditionalData(cocos2d::ValueMap customData) {
    [[AppsFlyerTracker sharedTracker] setAdditionalData: AppsFlyerXAppleHelper::valueMap2nsDictionary(customData)];
}
cocos2d::ValueMap AppsFlyerXApple::customData() {
    return AppsFlyerXAppleHelper::nsDictionary2ValueMap([[AppsFlyerTracker sharedTracker] customData]);
}

void AppsFlyerXApple::setAppsFlyerDevKey(const std::string& appsFlyerDevKey) {
    [[AppsFlyerTracker sharedTracker] setAppsFlyerDevKey:[NSString stringWithUTF8String:appsFlyerDevKey.c_str()]];
    
    static dispatch_once_t onceToken;
    static AppsFlyerXApple *xApple = nil;
    static AppsFlyerXAppleDelegate *delegate = nil;
    
    dispatch_once(&onceToken, ^{
        
        xApple = AppsFlyerXApple::getInstance();
        delegate = [[AppsFlyerXAppleDelegate alloc] init];
        xApple->delegate = delegate;
        
        [[NSNotificationCenter defaultCenter] addObserverForName: UIApplicationDidBecomeActiveNotification
         object: nil
         queue: nil
         usingBlock: ^ (NSNotification * note) {
             [[AppsFlyerTracker sharedTracker] trackAppLaunch];
         }];
    });

    [[AppsFlyerTracker sharedTracker] setDelegate: delegate];
}

std::string AppsFlyerXApple::appsFlyerDevKey() {
    return [[[AppsFlyerTracker sharedTracker] appsFlyerDevKey] UTF8String];
}

void AppsFlyerXApple::setAppleAppID(const std::string& appleAppID) {
    [[AppsFlyerTracker sharedTracker] setAppleAppID:[NSString stringWithUTF8String:appleAppID.c_str()]];
}

std::string AppsFlyerXApple::appleAppID() {
    return [[[AppsFlyerTracker sharedTracker] appleAppID] UTF8String];
}

void AppsFlyerXApple::setCurrencyCode(const std::string& currencyCode) {
    [[AppsFlyerTracker sharedTracker] setCurrencyCode:[NSString stringWithUTF8String:currencyCode.c_str()]];
}

std::string AppsFlyerXApple::currencyCode() {
    return [[[AppsFlyerTracker sharedTracker] currencyCode] UTF8String];
}

void AppsFlyerXApple::disableAppleAdSupportTracking(bool isDisableAppleAdSupportTracking) {
    [[AppsFlyerTracker sharedTracker] setDisableIAdTracking:isDisableAppleAdSupportTracking];
}

bool AppsFlyerXApple::isDisableAppleAdSupportTracking() {
    return [[AppsFlyerTracker sharedTracker] disableIAdTracking];
}

void AppsFlyerXApple::setIsDebug(bool isDebug) {
    [[AppsFlyerTracker sharedTracker] setIsDebug:isDebug];
}

bool AppsFlyerXApple::isDebug() {
    return [[AppsFlyerTracker sharedTracker] isDebug];
}

void AppsFlyerXApple::setShouldCollectDeviceName(bool isShouldCollectDeviceName) {
    [[AppsFlyerTracker sharedTracker] setShouldCollectDeviceName:isShouldCollectDeviceName];
}

bool AppsFlyerXApple::isShouldCollectDeviceName() {
    return [[AppsFlyerTracker sharedTracker] shouldCollectDeviceName];
}

void AppsFlyerXApple::setAppInviteOneLink(std::string& appInviteOneLinkID) {
    return [[AppsFlyerTracker sharedTracker] setAppInviteOneLink:[NSString stringWithUTF8String:appInviteOneLinkID.c_str()]];
}

std::string AppsFlyerXApple::appInviteOneLinkID() {
    return [[[AppsFlyerTracker sharedTracker] appInviteOneLinkID] UTF8String];
}

void AppsFlyerXApple::deviceTrackingDisabled(bool deviceTrackingDisabled) {
    [[AppsFlyerTracker sharedTracker] setDeviceTrackingDisabled:deviceTrackingDisabled];
}

bool AppsFlyerXApple::isDeviceTrackingDisabled() {
    return [[AppsFlyerTracker sharedTracker] deviceTrackingDisabled];
}

void AppsFlyerXApple::disableIAdTracking(bool disableIAdTracking) {
    [[AppsFlyerTracker sharedTracker] setDisableIAdTracking:disableIAdTracking];
}

bool AppsFlyerXApple::isDisableIAdTracking() {
    return [[AppsFlyerTracker sharedTracker] disableIAdTracking];
}

// weak! id<AppsFlyerTrackerDelegate> delegate;

void AppsFlyerXApple::setUseReceiptValidationSandbox(bool useReceiptValidationSandbox) {
    [[AppsFlyerTracker sharedTracker] setUseReceiptValidationSandbox:useReceiptValidationSandbox];
}

bool AppsFlyerXApple::isUseReceiptValidationSandbox() {
    return [[AppsFlyerTracker sharedTracker] useReceiptValidationSandbox];
}

void AppsFlyerXApple::setUseUninstallSandbox(bool setUseUninstallSandbox) {
    [[AppsFlyerTracker sharedTracker] setUseReceiptValidationSandbox:setUseUninstallSandbox];
}

bool AppsFlyerXApple::isUseUninstallSandbox() {
    return [[AppsFlyerTracker sharedTracker] useUninstallSandbox];
}

void AppsFlyerXApple::setAdvertiserId(const std::string& advertiserId) {
    [[AppsFlyerTracker sharedTracker] setAdvertiserId:[NSString stringWithUTF8String:advertiserId.c_str()]];
}

std::string AppsFlyerXApple::advertiserId() {
    return [[[AppsFlyerTracker sharedTracker] advertiserId] UTF8String];
}

void AppsFlyerXApple::setUserEmails(std::vector<std::string> userEmails, EmailCryptTypeX type) {
    NSMutableArray *emails = [NSMutableArray new];
    for (auto userEmail : userEmails) {
        [emails addObject:[NSString stringWithUTF8String:userEmail.c_str()]];
    }
    [[AppsFlyerTracker sharedTracker] setUserEmails:emails withCryptType:(EmailCryptType)[@(type) integerValue]];
}

void AppsFlyerXApple::trackAppLaunch() {
    [[AppsFlyerTracker sharedTracker] trackAppLaunch];
}

void AppsFlyerXApple::trackEvent(const std::string& eventName, const std::string& value) {
    [[AppsFlyerTracker sharedTracker] trackEvent:[NSString stringWithUTF8String:eventName.c_str()]
                                       withValue:[NSString stringWithUTF8String:value.c_str()]];
}

void AppsFlyerXApple::trackEvent(const std::string& eventName, cocos2d::ValueMap values) {
    [[AppsFlyerTracker sharedTracker] trackEvent:[NSString stringWithUTF8String:eventName.c_str()]
                                      withValues:AppsFlyerXAppleHelper::valueMap2nsDictionary(values)];
}

void AppsFlyerXApple::validateAndTrackInAppPurchase(std::string productIdentifier, std::string price, std::string currency, std::string tranactionId, cocos2d::ValueMap params, void (*successBlock)(cocos2d::ValueMap response), void (*failedBlock)(void* error, void* responce)) {
    // Unimplemented
}

void AppsFlyerXApple::trackLocation(double longitude, double latitude) {
    [[AppsFlyerTracker sharedTracker] trackLocation:longitude latitude:latitude];
}

std::string AppsFlyerXApple::getAppsFlyerUID() {
    return [[[AppsFlyerTracker sharedTracker] getAppsFlyerUID] UTF8String];
}

// - (void) loadConversionDataWithDelegate:(id<AppsFlyerTrackerDelegate>) delegate __attribute__((deprecated));

void AppsFlyerXApple::handleOpenURL(const std::string& url, const std::string& sourceApplication) {
    NSURL *nsurl = [NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]];
    [[AppsFlyerTracker sharedTracker] handleOpenURL:nsurl
                                  sourceApplication:[NSString stringWithUTF8String:sourceApplication.c_str()]];
}

void AppsFlyerXApple::handleOpenURL(std::string url, std::string sourceApplication, void* annotation) {
    // annotation unused
    NSURL *nsurl = [NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]];
    [[AppsFlyerTracker sharedTracker] handleOpenURL:nsurl sourceApplication:[NSString stringWithUTF8String:sourceApplication.c_str()] withAnnotation:nil];
}

void AppsFlyerXApple::handleOpenURL(std::string url, cocos2d::ValueMap options) {
    NSURL *nsurl = [NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]];
    [[AppsFlyerTracker sharedTracker] handleOpenUrl:nsurl options:AppsFlyerXAppleHelper::valueMap2nsDictionary(options)];
}

/*
 - (BOOL)continueUserActivity:(NSUserActivity *)userActivity restorationHandler:(void (^)(NSArray *))restorationHandler;
 - (void)didUpdateUserActivity:(NSUserActivity *)userActivity NS_AVAILABLE_IOS(9_0);
 */

void AppsFlyerXApple::handlePushNotification(cocos2d::ValueMap pushPayload) {
    [[AppsFlyerTracker sharedTracker] handlePushNotification: AppsFlyerXAppleHelper::valueMap2nsDictionary(pushPayload)];
}

void AppsFlyerXApple::registerUninstall(void* deviceToken, unsigned long length) {
    [[AppsFlyerTracker sharedTracker] registerUninstall: [NSData dataWithBytes:deviceToken length:length]];
}

std::string AppsFlyerXApple::getSDKVersion() {
    return [[[AppsFlyerTracker sharedTracker] getSDKVersion] UTF8String];
}

void AppsFlyerXApple::remoteDebuggingCallWithData(const std::string& data) {
    [[AppsFlyerTracker sharedTracker] remoteDebuggingCallWithData:[NSString stringWithUTF8String:data.c_str()]];
}

void AppsFlyerXApple::setHost(const std::string& host) {
    return [[AppsFlyerTracker sharedTracker] setHost:[NSString stringWithUTF8String:host.c_str()]];
}

std::string AppsFlyerXApple::getHost() {
    return [[[AppsFlyerTracker sharedTracker] host] UTF8String];
}

void AppsFlyerXApple::setMinTimeBetweenSessions(unsigned long minTimeBetweenSessions) {
    [[AppsFlyerTracker sharedTracker] setMinTimeBetweenSessions:minTimeBetweenSessions];
}

unsigned long AppsFlyerXApple::getMinTimeBetweenSessions() {
    return [[AppsFlyerTracker sharedTracker] minTimeBetweenSessions];
}

// Delegates methods proxy

void AppsFlyerXApple::setOnConversionDataReceived(void(*callback)(cocos2d::ValueMap installData)) {
    static_cast<AppsFlyerXAppleDelegate *>(AppsFlyerXApple::getInstance()->delegate).onConversionDataReceivedCallback = callback;
}

void AppsFlyerXApple::setOnConversionDataRequestFailure(void(*callback)(cocos2d::ValueMap error)) {
    static_cast<AppsFlyerXAppleDelegate *>(AppsFlyerXApple::getInstance()->delegate).onConversionDataRequestFailureCallback = callback;
}

void AppsFlyerXApple::setOnAppOpenAttribution(void(*callback)(cocos2d::ValueMap attributionData)) {
    static_cast<AppsFlyerXAppleDelegate *>(AppsFlyerXApple::getInstance()->delegate).onAppOpenAttributionCallback = callback;
}

void AppsFlyerXApple::setOnAppOpenAttributionFailure(void(*callback)(cocos2d::ValueMap error)) {
    static_cast<AppsFlyerXAppleDelegate *>(AppsFlyerXApple::getInstance()->delegate).onAppOpenAttributionFailureCallback = callback;
}


