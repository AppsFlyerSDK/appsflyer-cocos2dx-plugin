//
//  AppsFlyerXConsent.h
//  install_test_2
//
//  Created by Moris Gateno on 04/03/2024.
//

#ifndef AppsFlyerXConsent_h
#define AppsFlyerXConsent_h

class AppsFlyerXConsent{
private:
    bool isUserSubjectToGDPR;
    bool hasConsentForDataUsage;
    bool hasConsentForAdsPersonalization;
    
    AppsFlyerXConsent(bool isConsentForDataUsage, bool isConsentForAdsPersonalization): isUserSubjectToGDPR(true), hasConsentForDataUsage(isConsentForDataUsage), hasConsentForAdsPersonalization(isConsentForAdsPersonalization){}

    AppsFlyerXConsent(): isUserSubjectToGDPR(false),
        hasConsentForDataUsage(false), hasConsentForAdsPersonalization(false) {}

public:
//     methods to get the private fields
    bool IsUserSubjectToGDPR() const { return isUserSubjectToGDPR; }
    bool HasConsentForDataUsage() const { return hasConsentForDataUsage; }
    bool HasConsentForAdsPersonalization() const { return hasConsentForAdsPersonalization; }
    
    static AppsFlyerXConsent initNonGDPRUser() {
        AppsFlyerXConsent c;
        return c;
    }
    
    static AppsFlyerXConsent initForGDPRUser(bool hasConsentForDataUsage, bool hasConsentForAdsPersonalization) {
        AppsFlyerXConsent b(hasConsentForDataUsage, hasConsentForAdsPersonalization);
        return b;
    }
};

#endif /* AppsFlyerXConsent_h */
