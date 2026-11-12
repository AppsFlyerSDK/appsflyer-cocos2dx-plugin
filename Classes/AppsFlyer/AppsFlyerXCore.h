#ifndef AppsFlyerXCore_h
#define AppsFlyerXCore_h

#include "AppsFlyerXDeepLinkResult.h"

#include "cocos2d.h"

#include <functional>
#include <string>
#include <vector>

#include "EmailCryptTypeX.h"

class AppsFlyerXConsent;
struct AFXAdRevenueData;
class AFSDKXPurchaseDetails;
struct AFSDKXValidateAndLogResult;

namespace appsflyer {

// --- Phase 1 (RPC) ---
void setDevKey(const std::string& devKey);
std::string devKey();

void setAppleAppId(const std::string& appleAppId);
std::string appleAppId();

void setPendingIsDebug(bool isDebug);
bool pendingIsDebug();

void registerSessionReadyListener(void (*callback)());
void setOnConversionSuccess(void (*callback)(cocos2d::ValueMap));
void setOnConversionFail(void (*callback)(cocos2d::ValueMap));
void setOnDeepLinking(void (*callback)(AppsFlyerXDeepLinkResult));

void initSdk();
/** Schema: `start` RPC with optional `awaitResponse` (waits for native completion when true). */
void start(bool awaitResponse = false, std::function<void(bool success)> onComplete = nullptr);
void stop(bool shouldStop);
/** Schema: `logEvent` RPC with optional `awaitResponse`. */
void logEvent(const std::string& eventName, cocos2d::ValueMap eventValues, bool awaitResponse = false,
              std::function<void(bool success)> onComplete = nullptr);

// --- Phase 1+ stubs (not wired to RPC yet) ---
void enableTCFDataCollection(bool shouldCollectConsentData);
void setConsentData(const AppsFlyerXConsent& consentData);

void setCustomerUserID(const std::string& customerUserID);
std::string customerUserID();

void setAdditionalData(cocos2d::ValueMap customData);
cocos2d::ValueMap customData();

void setCurrencyCode(const std::string& currencyCode);
std::string currencyCode();

void setDisableAdvertisingIdentifiers(bool shouldDisable);
void disableAdvertisingIdentifier(bool shouldDisable);
bool isDisabledAdvertisingIdentifier();

void didEnterBackground();

void setShouldCollectDeviceName(bool isShouldCollectDeviceName);
bool isShouldCollectDeviceName();

void setAppInviteOneLink(const std::string& appInviteOneLinkID);
std::string appInviteOneLinkID();

void anonymizeUser(bool shouldAnonymize);
bool isAnonymizedUser();

void setDisableCollectASA(bool shouldDisable);
bool isDisabledCollectASA();

void setUseReceiptValidationSandbox(bool useReceiptValidationSandbox);
bool isUseReceiptValidationSandbox();

void setUseUninstallSandbox(bool useUninstallSandbox);
bool isUseUninstallSandbox();

void setUserEmails(std::vector<std::string> userEmails, EmailCryptTypeX type);

void logAdRevenue(const AFXAdRevenueData& adRevenueData, cocos2d::ValueMap additionalParameters);

void validateAndLogInAppPurchase(const std::string& productIdentifier, const std::string& price,
                                 const std::string& currency, const std::string& transactionId,
                                 cocos2d::ValueMap params,
                                 std::function<void(cocos2d::ValueMap)> successBlock,
                                 std::function<void(cocos2d::ValueMap)> failureBlock);

void validateAndLogInAppPurchase(const std::string& publicKey, const std::string& signature,
                                 const std::string& purchaseData, const std::string& price,
                                 const std::string& currency, cocos2d::ValueMap additionalParameters);

void validateAndLogInAppPurchase(AFSDKXPurchaseDetails& details, cocos2d::ValueMap params,
                                 std::function<void(cocos2d::ValueMap, cocos2d::ValueMap)> completionHandler);

void validateAndLogInAppPurchase(AFSDKXPurchaseDetails& details, cocos2d::ValueMap params,
                                 std::function<void(AFSDKXValidateAndLogResult)> completionHandler);

void logLocation(double longitude, double latitude);

std::string getAppsFlyerUID();

void handleOpenURL(const std::string& url, const std::string& sourceApplication);
void handleOpenURL(std::string url, std::string sourceApplication, void* annotation);
void handleOpenURL(std::string url, cocos2d::ValueMap options);

void handlePushNotification(cocos2d::ValueMap pushPayload);
void registerUninstall(void* deviceToken, unsigned long length);
void registerUninstall(const std::string& token);

void setHost(const std::string& host);
std::string getHost();

void setMinTimeBetweenSessions(unsigned long minTimeBetweenSessions);
unsigned long getMinTimeBetweenSessions();

void setOnAppOpenAttribution(void (*callback)(cocos2d::ValueMap));
void setOnAppOpenAttributionFailure(void (*callback)(cocos2d::ValueMap));

void sharingFilter(std::vector<std::string> partners);
void sharingFilterForAllPartners();

void disableSKAdNetwork(bool shouldDisable);
bool isDisabledSKAdNetwork();

void waitForATTUserAuthorizationWithTimeoutInterval(double timeoutInterval);

void setPhoneNumber(const std::string& phoneNumber);

void setPartnerData(const std::string& partnerId, cocos2d::ValueMap data);
void setOneLinkCustomDomains(std::vector<std::string> domains);
void setCurrentDeviceLanguage(const std::string& language);
void setSharingFilterForPartners(std::vector<std::string> partners);

void logInvite(const std::string& channel, cocos2d::ValueMap parameters);

void generateInviteUrl(cocos2d::ValueMap parameters, void (*onResponse)(std::string url),
                       void (*onResponseError)(std::string error));

void generateUserInviteLink(cocos2d::ValueMap parameters,
                            std::function<void(std::string url)> callback);

void setDisableNetworkData(bool disable);

} // namespace appsflyer

#endif
