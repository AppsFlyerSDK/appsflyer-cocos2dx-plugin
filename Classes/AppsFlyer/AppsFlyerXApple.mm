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
#include "AppsFlyerXAppleDeepLinkDelegate.h"
#import "libAppsFlyer/AppsFlyerLib.h"

/* Null, because instance will be initialized on demand. */
AppsFlyerXApple* AppsFlyerXApple::instance = 0;

AppsFlyerXApple* AppsFlyerXApple::getInstance() {
    if (instance == 0) {
        instance = new AppsFlyerXApple();
    }
    
    return instance;
}
//
//class AppsFlyerXAppleDeepLinkResult() {
//    public:
//        (instancetype)initWithDeepLink:(id)deeplink error:(NSError *)error;
//}
//
//class AppsFlyerXAppleDeepLink(){
//    + (instancetype _Nullable)withParameters:(NSDictionary * _Nullable)parameters;
//    + (instancetype _Nullable)withOneLink:(NSDictionary * _Nullable)parameters;
//}

AppsFlyerXApple::AppsFlyerXApple() {}

void AppsFlyerXApple::setCustomerUserID(const std::string& customerUserID) {
    [[AppsFlyerLib shared] setCustomerUserID: [NSString stringWithUTF8String:customerUserID.c_str()]];
}

std::string AppsFlyerXApple::customerUserID(){
    return [[[AppsFlyerLib shared] customerUserID] UTF8String];
}

void AppsFlyerXApple::setAdditionalData(cocos2d::ValueMap customData) {
    [[AppsFlyerLib shared] setAdditionalData: AppsFlyerXAppleHelper::valueMap2nsDictionary(customData)];
}
cocos2d::ValueMap AppsFlyerXApple::customData() {
    return AppsFlyerXAppleHelper::nsDictionary2ValueMap([[AppsFlyerLib shared] customData]);
}

void AppsFlyerXApple::setAppsFlyerDevKey(const std::string& appsFlyerDevKey) {
    [[AppsFlyerLib shared] setAppsFlyerDevKey:[NSString stringWithUTF8String:appsFlyerDevKey.c_str()]];
    
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
             [[AppsFlyerLib shared] start];
         }];
    });

    [[AppsFlyerLib shared] setDelegate: delegate];
}

std::string AppsFlyerXApple::appsFlyerDevKey() {
    return [[[AppsFlyerLib shared] appsFlyerDevKey] UTF8String];
}

void AppsFlyerXApple::setAppleAppID(const std::string& appleAppID) {
    [[AppsFlyerLib shared] setAppleAppID:[NSString stringWithUTF8String:appleAppID.c_str()]];
}

std::string AppsFlyerXApple::appleAppID() {
    return [[[AppsFlyerLib shared] appleAppID] UTF8String];
}

void AppsFlyerXApple::setCurrencyCode(const std::string& currencyCode) {
    [[AppsFlyerLib shared] setCurrencyCode:[NSString stringWithUTF8String:currencyCode.c_str()]];
}

std::string AppsFlyerXApple::currencyCode() {
    return [[[AppsFlyerLib shared] currencyCode] UTF8String];
}
#ifndef AFSDK_NO_IDFA
void AppsFlyerXApple::disableAdvertisingIdentifier(bool shouldDisable) {
    [[AppsFlyerLib shared] setDisableAdvertisingIdentifier:shouldDisable];
}
#endif

#ifndef AFSDK_NO_IDFA
bool AppsFlyerXApple::isDisabledAdvertisingIdentifier() {
    return [[AppsFlyerLib shared] disableAdvertisingIdentifier];
}
#endif

void AppsFlyerXApple::setIsDebug(bool isDebug) {
    [[AppsFlyerLib shared] setIsDebug:isDebug];
}

bool AppsFlyerXApple::isDebug() {
    return [[AppsFlyerLib shared] isDebug];
}

void AppsFlyerXApple::setShouldCollectDeviceName(bool isShouldCollectDeviceName) {
    [[AppsFlyerLib shared] setShouldCollectDeviceName:isShouldCollectDeviceName];
}

bool AppsFlyerXApple::isShouldCollectDeviceName() {
    return [[AppsFlyerLib shared] shouldCollectDeviceName];
}

void AppsFlyerXApple::setAppInviteOneLink(const std::string& appInviteOneLinkID) {
    return [[AppsFlyerLib shared] setAppInviteOneLink:[NSString stringWithUTF8String:appInviteOneLinkID.c_str()]];
}

std::string AppsFlyerXApple::appInviteOneLinkID() {
    return [[[AppsFlyerLib shared] appInviteOneLinkID] UTF8String];
}

void AppsFlyerXApple::anonymizeUser(bool shouldAnonymize) {
    [[AppsFlyerLib shared] setAnonymizeUser:shouldAnonymize];
}

bool AppsFlyerXApple::isAnonymizedUser() {
    return [[AppsFlyerLib shared] anonymizeUser];
}

void AppsFlyerXApple::setDisableCollectASA(bool shouldDisable) {
    [[AppsFlyerLib shared] setDisableCollectASA:shouldDisable];
}

bool AppsFlyerXApple::isDisabledCollectASA() {
    return [[AppsFlyerLib shared] disableCollectASA];
}


void AppsFlyerXApple::setUseReceiptValidationSandbox(bool useReceiptValidationSandbox) {
    [[AppsFlyerLib shared] setUseReceiptValidationSandbox:useReceiptValidationSandbox];
}

bool AppsFlyerXApple::isUseReceiptValidationSandbox() {
    return [[AppsFlyerLib shared] useReceiptValidationSandbox];
}

void AppsFlyerXApple::setUseUninstallSandbox(bool setUseUninstallSandbox) {
    [[AppsFlyerLib shared] setUseUninstallSandbox:setUseUninstallSandbox];
}

bool AppsFlyerXApple::isUseUninstallSandbox() {
    return [[AppsFlyerLib shared] useUninstallSandbox];
}


void AppsFlyerXApple::setUserEmails(std::vector<std::string> userEmails, EmailCryptTypeX type) {
    NSMutableArray *emails = [NSMutableArray new];
    for (auto userEmail : userEmails) {
        [emails addObject:[NSString stringWithUTF8String:userEmail.c_str()]];
    }
    [[AppsFlyerLib shared] setUserEmails:emails withCryptType:(EmailCryptType)[@(type) integerValue]];
}

void AppsFlyerXApple::start() {
    [[AppsFlyerLib shared] start];
}


void AppsFlyerXApple::logEvent(const std::string& eventName, cocos2d::ValueMap values) {
    NSMutableDictionary *dictionary = AppsFlyerXAppleHelper::valueMap2nsDictionary(values).mutableCopy;
    NSString *event = [NSString stringWithUTF8String:eventName.c_str()];
    
    
    id revenueString = dictionary[@"af_revenue"];
    if (revenueString && [revenueString isKindOfClass:[NSString class]]) {
        NSNumberFormatter *formatter = [[NSNumberFormatter alloc] init];
        formatter.numberStyle = NSNumberFormatterDecimalStyle;
        NSNumber *revenueNumber = [formatter numberFromString:revenueString];
        dictionary[@"af_revenue"] = revenueNumber;
    }
    
    [[AppsFlyerLib shared] logEvent:event withValues:dictionary];
    
}

void AppsFlyerXApple::validateAndLogInAppPurchase(const std::string& productIdentifier,
                                                    const std::string& price,
                                                    const std::string& currency,
                                                    const std::string& tranactionId,
                                                    cocos2d::ValueMap params,
                                                    std::function<void(cocos2d::ValueMap)> successBlock,
                                                    std::function<void(cocos2d::ValueMap)> failureBlock) {
    
    NSString *lProductIdentifier = [NSString stringWithUTF8String:productIdentifier.c_str()];
    NSString *lPrice = [NSString stringWithUTF8String:price.c_str()];
    NSString *lCurrency = [NSString stringWithUTF8String:currency.c_str()];
    NSString *lTranactionId = [NSString stringWithUTF8String:tranactionId.c_str()];
    NSDictionary *lParams = AppsFlyerXAppleHelper::valueMap2nsDictionary(params);
    
    [[AppsFlyerLib shared] validateAndLogInAppPurchase:lProductIdentifier
                                                              price:lPrice
                                                           currency:lCurrency
                                                      transactionId:lTranactionId
                                               additionalParameters:lParams success:^(NSDictionary *response) {
        cocos2d::ValueMap lResponce = AppsFlyerXAppleHelper::nsDictionary2ValueMap(response);
        successBlock(lResponce);
    } failure:^(NSError *error, id response) {
        NSMutableDictionary *errorDictionary = @{}.mutableCopy;
        if (error) {
            errorDictionary[@"errorCode"] = [NSNumber numberWithInteger:error.code];
            errorDictionary[@"errorDescription"] = error.localizedDescription;
        }
        if (response && [response isKindOfClass:[NSString class]]) {
            errorDictionary[@"errorCode"] = [NSNumber numberWithInteger:-1];
            errorDictionary[@"errorDescription"] = response;
        }
        failureBlock(AppsFlyerXAppleHelper::nsDictionary2ValueMap(errorDictionary));
    }];
}

void AppsFlyerXApple::logLocation(double longitude, double latitude) {
    [[AppsFlyerLib shared] logLocation:longitude latitude:latitude];
}

std::string AppsFlyerXApple::getAppsFlyerUID() {
    return [[[AppsFlyerLib shared] getAppsFlyerUID] UTF8String];
}

void AppsFlyerXApple::handleOpenURL(const std::string& url, const std::string& sourceApplication) {
    NSURL *nsurl = [NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]];
    [[AppsFlyerLib shared] handleOpenURL:nsurl
                                  sourceApplication:[NSString stringWithUTF8String:sourceApplication.c_str()]];
}

void AppsFlyerXApple::handleOpenURL(std::string url, std::string sourceApplication, void* annotation) {
    // annotation unused
    NSURL *nsurl = [NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]];
    [[AppsFlyerLib shared] handleOpenURL:nsurl sourceApplication:[NSString stringWithUTF8String:sourceApplication.c_str()] withAnnotation:nil];
}

void AppsFlyerXApple::handleOpenURL(std::string url, cocos2d::ValueMap options) {
    NSURL *nsurl = [NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]];
    [[AppsFlyerLib shared] handleOpenUrl:nsurl options:AppsFlyerXAppleHelper::valueMap2nsDictionary(options)];
}

/*
 - (BOOL)continueUserActivity:(NSUserActivity *)userActivity restorationHandler:(void (^)(NSArray *))restorationHandler;
 - (void)didUpdateUserActivity:(NSUserActivity *)userActivity NS_AVAILABLE_IOS(9_0);
 */

void AppsFlyerXApple::handlePushNotification(cocos2d::ValueMap pushPayload) {
    [[AppsFlyerLib shared] handlePushNotification: AppsFlyerXAppleHelper::valueMap2nsDictionary(pushPayload)];
}

void AppsFlyerXApple::registerUninstall(void* deviceToken, unsigned long length) {
    [[AppsFlyerLib shared] registerUninstall: [NSData dataWithBytes:deviceToken length:length]];
}


void AppsFlyerXApple::remoteDebuggingCallWithData(const std::string& data) {
    [[AppsFlyerLib shared] remoteDebuggingCallWithData:[NSString stringWithUTF8String:data.c_str()]];
}

void AppsFlyerXApple::setHost(const std::string& host) {
    return [[AppsFlyerLib shared] setHost:[NSString stringWithUTF8String:host.c_str()]];
}

std::string AppsFlyerXApple::getHost() {
    return [[[AppsFlyerLib shared] host] UTF8String];
}

void AppsFlyerXApple::setMinTimeBetweenSessions(unsigned long minTimeBetweenSessions) {
    [[AppsFlyerLib shared] setMinTimeBetweenSessions:minTimeBetweenSessions];
}

unsigned long AppsFlyerXApple::getMinTimeBetweenSessions() {
    return [[AppsFlyerLib shared] minTimeBetweenSessions];
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

void AppsFlyerXApple::stop(bool stop) {
    [[AppsFlyerLib shared] setIsStopped:stop];
}

void AppsFlyerXApple::sharingFilter(std::vector<std::string> partners){
    NSMutableArray *filteredPartners = [NSMutableArray new];
    for (auto partner : partners) {
        [filteredPartners addObject:[NSString stringWithUTF8String:partner.c_str()]];
    }
    [[AppsFlyerLib shared] setSharingFilter:filteredPartners];
}

void AppsFlyerXApple::sharingFilterForAllPartners() {
    [[AppsFlyerLib shared] setSharingFilterForAllPartners];
}

void AppsFlyerXApple::disableSKAdNetwork(bool shouldDisable){
    [[AppsFlyerLib shared] setDisableSKAdNetwork:shouldDisable];
}

bool AppsFlyerXApple::isDisabledSKAdNetwork(){
     return [[AppsFlyerLib shared] disableSKAdNetwork];
}

#ifndef AFSDK_NO_IDFA
void  AppsFlyerXApple::waitForATTUserAuthorizationWithTimeoutInterval(double timeoutInterval){
    if (@available(iOS 14, *)) {
        [[AppsFlyerLib shared] waitForATTUserAuthorizationWithTimeoutInterval:timeoutInterval];
    }
}
#endif

void AppsFlyerXApple::setPhoneNumber(const std::string& phoneNumber){
    NSString *phone = [NSString stringWithUTF8String:phoneNumber.c_str()];
     [[AppsFlyerLib shared] setPhoneNumber:phone];
}

void AppsFlyerXApple::setDidResolveDeepLink(void(*callback)(AppsFlyerXDeepLinkResult result)) {
    static dispatch_once_t onceToken;
       static AppsFlyerXApple *xApple = nil;
       static AppsFlyerXAppleDeepLinkDelegate *deepLinkDelegate = nil;
       
       dispatch_once(&onceToken, ^{
           
           xApple = AppsFlyerXApple::getInstance();
           deepLinkDelegate = [[AppsFlyerXAppleDeepLinkDelegate alloc] init];
           xApple->deepLinkDelegate = deepLinkDelegate;
       });
       [[AppsFlyerLib shared] setDeepLinkDelegate: deepLinkDelegate];
    static_cast<AppsFlyerXAppleDeepLinkDelegate *>(AppsFlyerXApple::getInstance()->deepLinkDelegate).didResolveDeepLinkCallback = callback;
}

void AppsFlyerXApple::setPartnerData(const std::string& partnerId, cocos2d::ValueMap data){
    [[AppsFlyerLib shared]  setPartnerDataWithPartnerId:[NSString stringWithUTF8String:partnerId.c_str()] partnerInfo:AppsFlyerXAppleHelper::valueMap2nsDictionary(data)];
}

void AppsFlyerXApple::setOneLinkCustomDomains(std::vector<std::string> domains){
    NSMutableArray *customDomains = [NSMutableArray new];
    for (auto domain : domains) {
        [customDomains addObject:[NSString stringWithUTF8String:domain.c_str()]];
    }
    [AppsFlyerLib shared].oneLinkCustomDomains = customDomains;
}

void AppsFlyerXApple::setCurrentDeviceLanguage(const std::string& language) {
    [AppsFlyerLib shared].currentDeviceLanguage = [NSString stringWithUTF8String:language.c_str()];
}

void AppsFlyerXApple::setSharingFilterForPartners(std::vector<std::string> partners) {
    NSMutableArray *sharingFilter = [NSMutableArray new];
    for (auto partner : partners) {
        [sharingFilter addObject:[NSString stringWithUTF8String:partner.c_str()]];
    }
    [[AppsFlyerLib shared] setSharingFilterForPartners: sharingFilter];
}

void AppsFlyerXApple::logInvite(const std::string& channel, cocos2d::ValueMap parameters) {
    NSString *lChannel = [NSString stringWithUTF8String:channel.c_str()];
    [AppsFlyerShareInviteHelper logInvite:lChannel parameters:AppsFlyerXAppleHelper::valueMap2nsDictionary(parameters)];
}

void AppsFlyerXApple::generateUserInviteLink(cocos2d::ValueMap parameters, std::function<void(std::string url)> callback) {
    [AppsFlyerShareInviteHelper generateInviteUrlWithLinkGenerator:^AppsFlyerLinkGenerator * _Nonnull(AppsFlyerLinkGenerator * _Nonnull generator) {
        return AppsFlyerXAppleHelper::valueMap2LinkGenerator(parameters,  generator);
    } completionHandler:^(NSURL * _Nullable url) {
        NSLog(@"url is %@", [url absoluteString]);
        callback([url.absoluteString UTF8String]);
    }];
};
