//
//  AppsFlyerX.cpp
//  AppsFlyerCocos2dX
//
//  Created by Andrey Gagan
//  Copyright © 2017 AppsFlyer. All rights reserved.
//

#include "AppsFlyerX.h"

#include "AFSDKXValidateAndLogResult.h"
#include "AppsFlyerXCore.h"

bool AppsFlyerX::manualStart = false;

void AppsFlyerX::setManualStart(bool isManualStart) {
    manualStart = isManualStart;
}

void AppsFlyerX::enableTCFDataCollection(bool shouldCollectConsentData) {
    appsflyer::enableTCFDataCollection(shouldCollectConsentData);
}

void AppsFlyerX::setConsentData(const AppsFlyerXConsent& consentData) {
    appsflyer::setConsentData(consentData);
}

void AppsFlyerX::setCustomerUserID(const std::string& customerUserID) {
    appsflyer::setCustomerUserID(customerUserID);
}

std::string AppsFlyerX::customerUserID() {
    return appsflyer::customerUserID();
}

void AppsFlyerX::setAdditionalData(cocos2d::ValueMap customData) {
    appsflyer::setAdditionalData(customData);
}

cocos2d::ValueMap AppsFlyerX::customData() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "customData is not supported for Android.");
    return {};
#else
    return appsflyer::customData();
#endif
}

void AppsFlyerX::setAppsFlyerDevKey(const std::string& appsFlyerDevKey) {
    appsflyer::setDevKey(appsFlyerDevKey);
}

std::string AppsFlyerX::appsFlyerDevKey() {
    return appsflyer::devKey();
}

void AppsFlyerX::setAppleAppID(const std::string& appleAppID) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "setAppleAppID is not supported for Android.");
#else
    appsflyer::setAppleAppId(appleAppID);
#endif
}

std::string AppsFlyerX::appleAppID() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "appleAppID is not supported for Android.");
    return {};
#else
    return appsflyer::appleAppId();
#endif
}

void AppsFlyerX::setCurrencyCode(const std::string& currencyCode) {
    appsflyer::setCurrencyCode(currencyCode);
}

std::string AppsFlyerX::currencyCode() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "currencyCode is not supported for Android.");
    return {};
#else
    return appsflyer::currencyCode();
#endif
}

#ifndef AFSDK_NO_IDFA
void AppsFlyerX::disableAdvertisingIdentifier(bool shouldDisable) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    appsflyer::setDisableAdvertisingIdentifiers(shouldDisable);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    appsflyer::disableAdvertisingIdentifier(shouldDisable);
#endif
}
#endif

#ifndef AFSDK_NO_IDFA
bool AppsFlyerX::isDisabledAdvertisingIdentifier() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "isDisabledAdvertiserIdentifier is not supported for Android.");
    return false;
#else
    return appsflyer::isDisabledAdvertisingIdentifier();
#endif
}
#endif

void AppsFlyerX::didEnterBackground() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    appsflyer::didEnterBackground();
#endif
}

void AppsFlyerX::setIsDebug(bool isDebug) {
    appsflyer::setPendingIsDebug(isDebug);
}

bool AppsFlyerX::isDebug() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "isDebug is not supported for Android.");
    return false;
#else
    return appsflyer::pendingIsDebug();
#endif
}

void AppsFlyerX::setShouldCollectDeviceName(bool isShouldCollectDeviceName) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "setShouldCollectDeviceName is not supported for Android.");
#else
    appsflyer::setShouldCollectDeviceName(isShouldCollectDeviceName);
#endif
}

bool AppsFlyerX::isShouldCollectDeviceName() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "isShouldCollectDeviceName is not supported for Android.");
    return false;
#else
    return appsflyer::isShouldCollectDeviceName();
#endif
}

void AppsFlyerX::setAppInviteOneLink(const std::string& appInviteOneLinkID) {
    appsflyer::setAppInviteOneLink(appInviteOneLinkID);
}

std::string AppsFlyerX::appInviteOneLinkID() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "appInviteOneLinkID is not supported for Android.");
    return {};
#else
    return appsflyer::appInviteOneLinkID();
#endif
}

void AppsFlyerX::anonymizeUser(bool shouldAnonymize) {
    appsflyer::anonymizeUser(shouldAnonymize);
}

bool AppsFlyerX::isAnonymizedUser() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "isAnonymizedUser is not supported for Android.");
    return false;
#else
    return appsflyer::isAnonymizedUser();
#endif
}

void AppsFlyerX::disableCollectASA(bool shouldDisable) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "disableCollectASA is not supported for Android.");
#else
    appsflyer::setDisableCollectASA(shouldDisable);
#endif
}

bool AppsFlyerX::isDisabledCollectASA() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "isDisabledCollectASA is not supported for Android.");
    return false;
#else
    return appsflyer::isDisabledCollectASA();
#endif
}

void AppsFlyerX::setUseReceiptValidationSandbox(bool useReceiptValidationSandbox) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "setUseReceiptValidationSandbox is not supported for Android.");
#else
    appsflyer::setUseReceiptValidationSandbox(useReceiptValidationSandbox);
#endif
}

bool AppsFlyerX::isUseReceiptValidationSandbox() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "isUseReceiptValidationSandbox is not supported for Android.");
    return false;
#else
    return appsflyer::isUseReceiptValidationSandbox();
#endif
}

void AppsFlyerX::setUseUninstallSandbox(bool useUninstallSandbox) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "setUseUninstallSandbox is not supported for Android.");
#else
    appsflyer::setUseUninstallSandbox(useUninstallSandbox);
#endif
}

bool AppsFlyerX::isUseUninstallSandbox() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "isUseUninstallSandbox is not supported for Android.");
    return false;
#else
    return appsflyer::isUseUninstallSandbox();
#endif
}

void AppsFlyerX::setUserEmails(std::vector<std::string> userEmails, EmailCryptTypeX type) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "setUserEmails is not supported for Android.");
#else
    appsflyer::setUserEmails(userEmails, type);
#endif
}

void AppsFlyerX::start() {
    appsflyer::start();
}

void AppsFlyerX::initSdk() {
    appsflyer::initSdk();
}

void AppsFlyerX::registerSessionReadyListener(void (*callback)()) {
    appsflyer::registerSessionReadyListener(callback);
}

void AppsFlyerX::logEvent(const std::string& eventName, cocos2d::ValueMap values) {
    appsflyer::logEvent(eventName, values);
}

void AppsFlyerX::validateAndLogInAppPurchase(const std::string& productIdentifier,
                                             const std::string& price,
                                             const std::string& currency,
                                             const std::string& tranactionId,
                                             cocos2d::ValueMap params,
                                             std::function<void(cocos2d::ValueMap)> successBlock,
                                             std::function<void(cocos2d::ValueMap)> failureBlock) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "validateAndLogInAppPurchase is not supported for Android.");
#else
    appsflyer::validateAndLogInAppPurchase(productIdentifier, price, currency, tranactionId,
                                           params, successBlock, failureBlock);
#endif
}

void AppsFlyerX::validateAndLogInAppPurchase(const std::string& publicKey,
                                             const std::string& signature,
                                             const std::string& purchaseData,
                                             const std::string& price,
                                             const std::string& currency,
                                             cocos2d::ValueMap additionalParameters) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    appsflyer::validateAndLogInAppPurchase(publicKey, signature, purchaseData, price, currency,
                                           additionalParameters);
#else
    CCLOGWARN("%s",
              "validateAndLogInAppPurchase(publicKey, ...) is not supported for iOS.");
#endif
}

void AppsFlyerX::logAdRevenue(AFXAdRevenueData adRevenueData, cocos2d::ValueMap additionalParameters) {
    appsflyer::logAdRevenue(adRevenueData, additionalParameters);
}

void AppsFlyerX::validateAndLogInAppPurchase(
    AFSDKXPurchaseDetails& details, cocos2d::ValueMap params,
    std::function<void(cocos2d::ValueMap, cocos2d::ValueMap)> completionHandler) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    auto adaptedHandler = [completionHandler](AFSDKXValidateAndLogResult result) {
        completionHandler(result.getResult(), result.getError());
    };
    appsflyer::validateAndLogInAppPurchase(details, std::move(params), std::move(adaptedHandler));
#else
    appsflyer::validateAndLogInAppPurchase(details, params, completionHandler);
#endif
}

std::string AppsFlyerX::getAppsFlyerUID() {
    return appsflyer::getAppsFlyerUID();
}

void AppsFlyerX::handleOpenURL(const std::string& url, const std::string& sourceApplication) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "handleOpenURL is not supported for Android.");
#else
    appsflyer::handleOpenURL(url, sourceApplication);
#endif
}

void AppsFlyerX::handleOpenURL(std::string url, std::string sourceApplication, void* annotation) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "handleOpenURL is not supported for Android.");
#else
    appsflyer::handleOpenURL(url, sourceApplication, annotation);
#endif
}

void AppsFlyerX::handleOpenURL(std::string url, cocos2d::ValueMap options) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "handleOpenURL is not supported for Android.");
#else
    appsflyer::handleOpenURL(url, options);
#endif
}

void AppsFlyerX::handlePushNotification(cocos2d::ValueMap pushPayload) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "handlePushNotification is not supported for Android.");
#else
    appsflyer::handlePushNotification(pushPayload);
#endif
}

void AppsFlyerX::registerUninstall(void* deviceToken, unsigned long length) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s",
              "registerUninstall(void* deviceToken, unsigned long length) is not supported for "
              "Android.");
#else
    appsflyer::registerUninstall(deviceToken, length);
#endif
}

void AppsFlyerX::registerUninstall(const std::string& token) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    CCLOGWARN("%s", "registerUninstall(token) is not supported for iOS.");
#else
    appsflyer::registerUninstall(token);
#endif
}

void AppsFlyerX::setHost(const std::string& host) {
    appsflyer::setHost(host);
}

std::string AppsFlyerX::getHost() {
    return appsflyer::getHost();
}

void AppsFlyerX::setMinTimeBetweenSessions(unsigned long minTimeBetweenSessions) {
    appsflyer::setMinTimeBetweenSessions(minTimeBetweenSessions);
}

unsigned long AppsFlyerX::getMinTimeBetweenSessions() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return appsflyer::getMinTimeBetweenSessions();
#else
    return 0;
#endif
}

void AppsFlyerX::setOnConversionDataSuccess(void (*callback)(cocos2d::ValueMap installData)) {
    appsflyer::setOnConversionSuccess(callback);
}

void AppsFlyerX::setOnConversionDataFail(void (*callback)(cocos2d::ValueMap error)) {
    appsflyer::setOnConversionFail(callback);
}

void AppsFlyerX::setOnAppOpenAttribution(void (*callback)(cocos2d::ValueMap attributionData)) {
    appsflyer::setOnAppOpenAttribution(callback);
}

void AppsFlyerX::setOnAppOpenAttributionFailure(void (*callback)(cocos2d::ValueMap error)) {
    appsflyer::setOnAppOpenAttributionFailure(callback);
}

void AppsFlyerX::stop(bool shouldStop) {
    appsflyer::stop(shouldStop);
}

[[deprecated("This method is deprecated, please replace by setSharingFilterForPartners API")]]
void AppsFlyerX::sharingFilter(std::vector<std::string> partners) {
    appsflyer::sharingFilter(partners);
}

[[deprecated("This method is deprecated, please replace by setSharingFilterForPartners API")]]
void AppsFlyerX::sharingFilterForAllPartners() {
    appsflyer::sharingFilterForAllPartners();
}

void AppsFlyerX::logLocation(double longitude, double latitude) {
    appsflyer::logLocation(longitude, latitude);
}

void AppsFlyerX::disableSKAdNetwork(bool shouldDisable) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "disableSKAdNetwork is not supported for Android.");
#else
    appsflyer::disableSKAdNetwork(shouldDisable);
#endif
}

bool AppsFlyerX::isDisabledSKAdNetwork() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "isDisabledSKAdNetwork is not supported for Android.");
    return false;
#else
    return appsflyer::isDisabledSKAdNetwork();
#endif
}

#ifndef AFSDK_NO_IDFA
void AppsFlyerX::waitForATTUserAuthorizationWithTimeoutInterval(double timeoutInterval) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "waitForATTUserAuthorizationWithTimeoutInterval is not supported for Android.");
#else
    appsflyer::waitForATTUserAuthorizationWithTimeoutInterval(timeoutInterval);
#endif
}
#endif

void AppsFlyerX::setPhoneNumber(const std::string& phoneNumber) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "setPhoneNumber is not supported for Android.");
#else
    appsflyer::setPhoneNumber(phoneNumber);
#endif
}

void AppsFlyerX::setDidResolveDeepLink(void (*callback)(AppsFlyerXDeepLinkResult result)) {
    appsflyer::setOnDeepLinking(callback);
}

void AppsFlyerX::setPartnerData(std::string partnerId, cocos2d::ValueMap data) {
    appsflyer::setPartnerData(partnerId, data);
}

void AppsFlyerX::setOneLinkCustomDomains(std::vector<std::string> domains) {
    appsflyer::setOneLinkCustomDomains(domains);
}

void AppsFlyerX::setCurrentDeviceLanguage(const std::string& language) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s", "setCurrentDeviceLanguage is not supported for Android.");
#else
    appsflyer::setCurrentDeviceLanguage(language);
#endif
}

void AppsFlyerX::setSharingFilterForPartners(std::vector<std::string> partners) {
    appsflyer::setSharingFilterForPartners(partners);
}

void AppsFlyerX::logInvite(const std::string& channel, cocos2d::ValueMap parameters) {
    appsflyer::logInvite(channel, parameters);
}

void AppsFlyerX::generateUserInviteLink(cocos2d::ValueMap parameters, void (*onResponse)(std::string url),
                                        void (*onResponseError)(std::string url)) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    appsflyer::generateInviteUrl(parameters, onResponse, onResponseError);
#else
    CCLOGWARN("%s",
              "Please use generateUserInviteLink(cocos2d::ValueMap parameters, "
              "std::function<void(std::string url)> callback) for iOS");
#endif
}

void AppsFlyerX::generateUserInviteLink(cocos2d::ValueMap parameters,
                                        std::function<void(std::string url)> callback) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCLOGWARN("%s",
              "Please use generateUserInviteLink(cocos2d::ValueMap parameters,void(*onResponse)"
              "(std::string url), void(*onResponseError)(std::string url)) for Android");
#else
    appsflyer::generateUserInviteLink(parameters, callback);
#endif
}

void AppsFlyerX::setDisableNetworkData(bool disable) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    CCLOGWARN("%s", "setDisableNetworkData is not supported for iOS");
#else
    appsflyer::setDisableNetworkData(disable);
#endif
}
