//
//  AppsFlyerX.cpp
//  AppsFlyerCocos2dX
//
//  Created by Andrey Gagan
//  Copyright © 2017 AppsFlyer. All rights reserved.
//

#include "AppsFlyerX.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
/// Android proxy(.h) goes here
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "AppsFlyerXApple.h"
#endif

void AppsFlyerX::setCustomerUserID(const std::string& customerUserID) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::setCustomerUserID(customerUserID);
#endif
}
std::string AppsFlyerX::customerUserID() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::customerUserID();
#endif
}

void AppsFlyerX::setAdditionalData(cocos2d::ValueMap customData) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::setAdditionalData(customData);
#endif
}

cocos2d::ValueMap AppsFlyerX::customData() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::customData();
#endif
}

void AppsFlyerX::setAppsFlyerDevKey(const std::string& appsFlyerDevKey) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::setAppsFlyerDevKey(appsFlyerDevKey);
#endif
}

std::string AppsFlyerX::appsFlyerDevKey() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::appsFlyerDevKey();
#endif
}

void AppsFlyerX::setAppleAppID(const std::string& appleAppID) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::setAppleAppID(appleAppID);
#endif
}

std::string AppsFlyerX::appleAppID() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::appleAppID();
#endif
}

void AppsFlyerX::setCurrencyCode(const std::string& currencyCode) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::setCurrencyCode(currencyCode);
#endif
}

std::string AppsFlyerX::currencyCode() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::currencyCode();
#endif
}

void AppsFlyerX::disableAppleAdSupportTracking(bool isDisableAppleAdSupportTracking) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::disableAppleAdSupportTracking(isDisableAppleAdSupportTracking);
#endif
}

bool AppsFlyerX::isDisableAppleAdSupportTracking() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::isDisableAppleAdSupportTracking();
#endif
}

void AppsFlyerX::setIsDebug(bool isDebug) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::setIsDebug(isDebug);
#endif
}

bool AppsFlyerX::isDebug() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::isDebug();
#endif
}

void AppsFlyerX::setShouldCollectDeviceName(bool isShouldCollectDeviceName) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::setShouldCollectDeviceName(isShouldCollectDeviceName);
#endif
}

bool AppsFlyerX::isShouldCollectDeviceName() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::isShouldCollectDeviceName();
#endif
}

void AppsFlyerX::setAppInviteOneLink(std::string& appInviteOneLinkID) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::setAppInviteOneLink(appInviteOneLinkID);
#endif
}

std::string AppsFlyerX::appInviteOneLinkID() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::appInviteOneLinkID();
#endif
}

void AppsFlyerX::deviceTrackingDisabled(bool deviceTrackingDisabled) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::deviceTrackingDisabled(deviceTrackingDisabled);
#endif
}

bool AppsFlyerX::isDeviceTrackingDisabled() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::isDeviceTrackingDisabled();
#endif
}

void AppsFlyerX::disableIAdTracking(bool disableIAdTracking) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::disableIAdTracking(disableIAdTracking);
#endif
}

bool AppsFlyerX::isDisableIAdTracking() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::isDisableIAdTracking();
#endif
}

void AppsFlyerX::setUseReceiptValidationSandbox(bool useReceiptValidationSandbox) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::setUseReceiptValidationSandbox(useReceiptValidationSandbox);
#endif
}

bool AppsFlyerX::isUseReceiptValidationSandbox() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::isUseReceiptValidationSandbox();
#endif
}

void AppsFlyerX::setUseUninstallSandbox(bool setUseUninstallSandbox) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::setUseUninstallSandbox(setUseUninstallSandbox);
#endif
}

bool AppsFlyerX::isUseUninstallSandbox() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::isUseUninstallSandbox();
#endif
}

void AppsFlyerX::setAdvertiserId(const std::string& advertiserId) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::setAdvertiserId(advertiserId);
#endif
}

std::string AppsFlyerX::advertiserId() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::advertiserId();
#endif
}

void AppsFlyerX::setUserEmails(std::vector<std::string> userEmails, EmailCryptTypeX type) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::setUserEmails(userEmails, type);
#endif
}

void AppsFlyerX::trackAppLaunch() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::trackAppLaunch();
#endif
}

void AppsFlyerX::trackEvent(const std::string& eventName, const std::string& value) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::trackEvent(eventName, value);
#endif
}

void AppsFlyerX::trackEvent(const std::string& eventName, cocos2d::ValueMap values) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::trackEvent(eventName, values);
#endif
}

void AppsFlyerX::validateAndTrackInAppPurchase(std::string productIdentifier, std::string price, std::string currency, std::string tranactionId, cocos2d::ValueMap params, void (*successBlock)(cocos2d::ValueMap response), void (*failedBlock)(void* error, void* responce)) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::validateAndTrackInAppPurchase(productIdentifier, price, currency, tranactionId, params, successBlock, failedBlock);
#endif
}

void AppsFlyerX::trackLocation(double longitude, double latitude) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::trackLocation(longitude, latitude);
#endif
}

std::string AppsFlyerX::getAppsFlyerUID() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::getAppsFlyerUID();
#endif
}

// - (void) loadConversionDataWithDelegate:(id<AppsFlyerTrackerDelegate>) delegate __attribute__((deprecated));

void AppsFlyerX::handleOpenURL(const std::string& url, const std::string& sourceApplication) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::handleOpenURL(url, sourceApplication);
#endif
}

void AppsFlyerX::handleOpenURL(std::string url, std::string sourceApplication, void* annotation) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::handleOpenURL(url, sourceApplication, annotation);
#endif
}

void AppsFlyerX::handleOpenURL(std::string url, cocos2d::ValueMap options) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::handleOpenURL(url, options);
#endif
}

/*
 - (BOOL) continueUserActivity:(NSUserActivity *) userActivity restorationHandler:(void (^)(NSArray *))restorationHandler NS_AVAILABLE_IOS(9_0);
 - (void) didUpdateUserActivity:(NSUserActivity *)userActivity NS_AVAILABLE_IOS(9_0);
 */

void AppsFlyerX::handlePushNotification(cocos2d::ValueMap pushPayload) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::handlePushNotification(pushPayload);
#endif
}

void AppsFlyerX::registerUninstall(void* deviceToken, unsigned long length) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::registerUninstall(deviceToken, length);
#endif
}

std::string AppsFlyerX::getSDKVersion() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::getSDKVersion();
#endif
}

void AppsFlyerX::remoteDebuggingCallWithData(const std::string& data) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::remoteDebuggingCallWithData(data);
#endif
}

void AppsFlyerX::setHost(const std::string& host) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::setHost(host);
#endif
}
std::string AppsFlyerX::getHost() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::getHost();
#endif
}

void AppsFlyerX::setMinTimeBetweenSessions(unsigned long minTimeBetweenSessions) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::setMinTimeBetweenSessions(minTimeBetweenSessions);
#endif
}
unsigned long AppsFlyerX::getMinTimeBetweenSessions() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::getMinTimeBetweenSessions();
#endif
}

// Delegates methods proxy

void AppsFlyerX::setOnConversionDataReceived(void(*callback)(cocos2d::ValueMap installData)) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::setOnConversionDataReceived(callback);
#endif
}
void AppsFlyerX::setOnConversionDataRequestFailure(void(*callback)(cocos2d::ValueMap error)) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::setOnConversionDataRequestFailure(callback);
#endif
}
void AppsFlyerX::setOnAppOpenAttribution(void(*callback)(cocos2d::ValueMap attributionData)) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::setOnAppOpenAttribution(callback);
#endif
}
void AppsFlyerX::setOnAppOpenAttributionFailure(void(*callback)(cocos2d::ValueMap error)) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::setOnAppOpenAttributionFailure(callback);
#endif
}


