//
//  AppsFlyerX.cpp
//  AppsFlyerCocos2dX
//
//  Created by Andrey Gagan
//  Copyright © 2017 AppsFlyer. All rights reserved.
//

#include "AppsFlyerX.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "AppsFlyerXAndroid.h"

#define COCOS2D_DEBUG 1

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "AppsFlyerXApple.h"
#endif

void AppsFlyerX::setCustomerUserID(const std::string& customerUserID) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AppsFlyerXAndroid::setCustomerUserID(customerUserID);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::setCustomerUserID(customerUserID);
#endif
}
std::string AppsFlyerX::customerUserID() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return AppsFlyerXAndroid::customerUserID();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::customerUserID();
#endif
}

void AppsFlyerX::setAdditionalData(cocos2d::ValueMap customData) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AppsFlyerXAndroid::setAdditionalData(customData);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::setAdditionalData(customData);
#endif
}

cocos2d::ValueMap AppsFlyerX::customData() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Not supported for Android
    CCLOGWARN("%s", "customData is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::customData();
#endif
}

void AppsFlyerX::setAppsFlyerDevKey(const std::string& appsFlyerDevKey) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AppsFlyerXAndroid::setAppsFlyerDevKey(appsFlyerDevKey);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::setAppsFlyerDevKey(appsFlyerDevKey);
#endif
}

std::string AppsFlyerX::appsFlyerDevKey() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return AppsFlyerXAndroid::appsFlyerDevKey();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::appsFlyerDevKey();
#endif
}

void AppsFlyerX::setAppleAppID(const std::string& appleAppID) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Not supported for Android
    CCLOGWARN("%s", "setAppleAppID is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::setAppleAppID(appleAppID);
#endif
}

std::string AppsFlyerX::appleAppID() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Not supported for Android
    CCLOGWARN("%s", "appleAppID is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::appleAppID();
#endif
}

void AppsFlyerX::setCurrencyCode(const std::string& currencyCode) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AppsFlyerXAndroid::setCurrencyCode(currencyCode);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::setCurrencyCode(currencyCode);
#endif
}

std::string AppsFlyerX::currencyCode() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Not supported for Android
    CCLOGWARN("%s", "currencyCode is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::currencyCode();
#endif
}

void AppsFlyerX::disableAdvertisingIdentifier(bool shouldDisable) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Not supported for Android
    CCLOGWARN("%s", "disableAdvertiserIdentifier is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::disableAdvertisingIdentifier(shouldDisable);
#endif
}

bool AppsFlyerX::isDisabledAdvertisingIdentifier() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Not supported for Android
    CCLOGWARN("%s", "isDisabledAdvertiserIdentifier is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::isDisabledAdvertisingIdentifier();
#endif
}

 void AppsFlyerX::didEnterBackground(){
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
     AppsFlyerXAndroid::didEnterBackground();
#endif
 }

void AppsFlyerX::setIsDebug(bool isDebug) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AppsFlyerXAndroid::setIsDebug(isDebug);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
     AppsFlyerXApple::setIsDebug(isDebug);
#endif
}

bool AppsFlyerX::isDebug() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Not supported for Android
    CCLOGWARN("%s", "isDebug is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::isDebug();
#endif
}

void AppsFlyerX::setShouldCollectDeviceName(bool isShouldCollectDeviceName) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Not supported for Android
    CCLOGWARN("%s", "setShouldCollectDeviceName is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::setShouldCollectDeviceName(isShouldCollectDeviceName);
#endif
}

bool AppsFlyerX::isShouldCollectDeviceName() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Not supported for Android
    CCLOGWARN("%s", "isShouldCollectDeviceName is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::isShouldCollectDeviceName();
#endif
}

void AppsFlyerX::setAppInviteOneLink(std::string& appInviteOneLinkID) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
   AppsFlyerXAndroid::setAppInviteOneLink(appInviteOneLinkID);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::setAppInviteOneLink(appInviteOneLinkID);
#endif
}

std::string AppsFlyerX::appInviteOneLinkID() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Not supported for Android
    CCLOGWARN("%s", "appInviteOneLinkID is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::appInviteOneLinkID();
#endif
}

void AppsFlyerX::anonymizeUser(bool shouldAnonymize) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AppsFlyerXAndroid::anonymizeUser(shouldAnonymize);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::anonymizeUser(shouldAnonymize);
#endif
}

bool AppsFlyerX::isAnonymizedUser() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Not supported for Android
    CCLOGWARN("%s", "isAnonymizedUser is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::isAnonymizedUser();
#endif
}

void AppsFlyerX::disableCollectASA(bool shouldDisable) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Not supported for Android
    CCLOGWARN("%s", "disableCollectASA is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::setDisableCollectASA(shouldDisable);
#endif
}

bool AppsFlyerX::isDisabledCollectASA() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Not supported for Android
    CCLOGWARN("%s", "isDisabledCollectASA is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::isDisabledCollectASA();
#endif
}

void AppsFlyerX::setUseReceiptValidationSandbox(bool useReceiptValidationSandbox) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Not supported for Android
    CCLOGWARN("%s", "setUseReceiptValidationSandbox is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::setUseReceiptValidationSandbox(useReceiptValidationSandbox);
#endif
}

bool AppsFlyerX::isUseReceiptValidationSandbox() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Not supported for Android
    CCLOGWARN("%s", "isUseReceiptValidationSandbox is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::isUseReceiptValidationSandbox();
#endif
}

void AppsFlyerX::setUseUninstallSandbox(bool setUseUninstallSandbox) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Not supported for Android
    CCLOGWARN("%s", "setUseUninstallSandbox is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::setUseUninstallSandbox(setUseUninstallSandbox);
#endif
}

bool AppsFlyerX::isUseUninstallSandbox() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Not supported for Android
    CCLOGWARN("%s", "isUseUninstallSandbox is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::isUseUninstallSandbox();
#endif
}

void AppsFlyerX::setAdvertiserId(const std::string& advertiserId) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Not supported for Android
    CCLOGWARN("%s", "setAdvertiserId is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::setAdvertiserId(advertiserId);
#endif
}

std::string AppsFlyerX::advertiserId() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Not supported for Android
    CCLOGWARN("%s", "advertiserId is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::advertiserId();
#endif
}

void AppsFlyerX::setUserEmails(std::vector<std::string> userEmails, EmailCryptTypeX type) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Not supported for Android
    CCLOGWARN("%s", "setUserEmails is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::setUserEmails(userEmails, type);
#endif
}

void AppsFlyerX::start() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AppsFlyerXAndroid::start();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::start();
#endif
}

void AppsFlyerX::logEvent(const std::string& eventName, cocos2d::ValueMap values) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AppsFlyerXAndroid::logEvent(eventName, values);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::logEvent(eventName, values);
#endif
}

void AppsFlyerX::validateAndLogInAppPurchase(const std::string& productIdentifier,
                                               const std::string& price,
                                               const std::string& currency,
                                               const std::string& tranactionId,
                                               cocos2d::ValueMap params,
                                               std::function<void(cocos2d::ValueMap)> successBlock,
                                               std::function<void(cocos2d::ValueMap)> failureBlock) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Not supported for Android
    CCLOGWARN("%s", "validateAndLogInAppPurchase is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

    AppsFlyerXApple::validateAndLogInAppPurchase(productIdentifier, price, currency, tranactionId, params, [&](cocos2d::ValueMap result) {
        successBlock(result);
    }, [&] (cocos2d::ValueMap error) {
        successBlock(error);
    });
#endif
}

void AppsFlyerX::validateAndLogInAppPurchase(const std::string& publicKey,
                                               const std::string& signature,
                                               const std::string& purchaseData,
                                               const std::string& price,
                                               const std::string& currency,
                                               cocos2d::ValueMap additionalParameters){
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AppsFlyerXAndroid::validateAndLogInAppPurchase(publicKey, signature, purchaseData, price, currency, additionalParameters);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    // Not supported for iOS
    CCLOGWARN("%s", "validateAndLogInAppPurchase(const std::string& publicKey, const std::string& signature, const std::string& purchaseData, const std::string& price, const std::string& currency, cocos2d::ValueMap additionalParameters) is not supported for Android.");
#endif
}


std::string AppsFlyerX::getAppsFlyerUID() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
   return AppsFlyerXAndroid::getAppsFlyerUID();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::getAppsFlyerUID();
#endif
}


void AppsFlyerX::handleOpenURL(const std::string& url, const std::string& sourceApplication) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Not supported for Android
    CCLOGWARN("%s", "handleOpenURL is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::handleOpenURL(url, sourceApplication);
#endif
}

void AppsFlyerX::handleOpenURL(std::string url, std::string sourceApplication, void* annotation) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Not supported for Android
    CCLOGWARN("%s", "handleOpenURL is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::handleOpenURL(url, sourceApplication, annotation);
#endif
}

void AppsFlyerX::handleOpenURL(std::string url, cocos2d::ValueMap options) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Not supported for Android
    CCLOGWARN("%s", "handleOpenURL is not supported for Android.");
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
    // Not supported for Android
    CCLOGWARN("%s", "handlePushNotification is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::handlePushNotification(pushPayload);
#endif
}

void AppsFlyerX::registerUninstall(void* deviceToken, unsigned long length) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Not supported for Android
    CCLOGWARN("%s", "registerUninstall(void* deviceToken, unsigned long length) is not supported for IOS.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::registerUninstall(deviceToken, length);
#endif
}

void AppsFlyerX::registerUninstall(const std::string& token) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AppsFlyerXAndroid::registerUninstall(token);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    // Not supported for IOS
    CCLOGWARN("%s", "handlePushNotification is not supported for IOS.");
#endif
}

// std::string AppsFlyerX::getSDKVersion() {
// #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//     return AppsFlyerXAndroid::getSDKVersion();
// #elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
//     return AppsFlyerXApple::getSDKVersion();
// #endif
// }


void AppsFlyerX::setHost(const std::string& host) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AppsFlyerXAndroid::setHost(host);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::setHost(host);
#endif
}
std::string AppsFlyerX::getHost() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return AppsFlyerXAndroid::getHost();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::getHost();
#endif
}

void AppsFlyerX::setMinTimeBetweenSessions(unsigned long minTimeBetweenSessions) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
   AppsFlyerXAndroid::setMinTimeBetweenSessions(minTimeBetweenSessions);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AppsFlyerXApple::setMinTimeBetweenSessions(minTimeBetweenSessions);
#endif
}

// TODO: remove
unsigned long AppsFlyerX::getMinTimeBetweenSessions() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // FIXME: Android is not support this API
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::getMinTimeBetweenSessions();
#endif
}

// Delegates methods proxy
void AppsFlyerX::setOnConversionDataSuccess(void(*callback)(cocos2d::ValueMap installData)) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AppsFlyerXAndroid::setOnConversionDataSuccess(callback);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::setOnConversionDataReceived(callback);
#endif
}

void AppsFlyerX::setOnConversionDataFail(void(*callback)(cocos2d::ValueMap error)) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AppsFlyerXAndroid::setOnConversionDataFail(callback);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::setOnConversionDataRequestFailure(callback);
#endif
}

void AppsFlyerX::setOnAppOpenAttribution(void(*callback)(cocos2d::ValueMap attributionData)) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AppsFlyerXAndroid::setOnAppOpenAttribution(callback);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::setOnAppOpenAttribution(callback);
#endif
}

void AppsFlyerX::setOnAppOpenAttributionFailure(void(*callback)(cocos2d::ValueMap error)) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AppsFlyerXAndroid::setOnAppOpenAttributionFailure(callback);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::setOnAppOpenAttributionFailure(callback);
#endif
}


void AppsFlyerX::stop(bool shouldStop) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AppsFlyerXAndroid::stop(shouldStop);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::stop(shouldStop);
#endif
}

void AppsFlyerX::sharingFilter(std::vector<std::string> partners){
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AppsFlyerXAndroid::sharingFilter(partners);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::sharingFilter(partners);
#endif
}

void AppsFlyerX::sharingFilterForAllPartners(){
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AppsFlyerXAndroid::sharingFilterForAllPartners();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::sharingFilterForAllPartners();
#endif
}

void AppsFlyerX::logLocation(double longitude, double latitude){
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::logLocation(longitude, latitude);
#endif
}

void AppsFlyerX::disableSKAdNetwork(bool shouldDisable){
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    //not supported for Android
    CCLOGWARN("%s", "disableSKAdNetwork is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::disableSKAdNetwork(shouldDisable);
#endif
}

bool AppsFlyerX::isDisabledSKAdNetwork(){
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    //not supported for Android
    CCLOGWARN("%s", "isDisabledSKAdNetwork is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::isDisabledSKAdNetwork();
#endif
}

void AppsFlyerX::waitForAdvertisingIdentifierWithTimeoutInterval(double timeoutInterval){
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    //not supported for Android
    CCLOGWARN("%s", "waitForAdvertisingIdentifierWithTimeoutInterval is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::waitForAdvertisingIdentifierWithTimeoutInterval(timeoutInterval);
#endif
}


void AppsFlyerX::setPhoneNumber(const std::string& phoneNumber){
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    //not supported for Android
    CCLOGWARN("%s", "setPhoneNumber is not supported for Android.");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return AppsFlyerXApple::setPhoneNumber(phoneNumber);
#endif
}
