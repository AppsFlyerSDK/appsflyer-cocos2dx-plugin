//
//  AppsFlyerXAppleDelegate.m
//  AppsFlyerCocos2dX-mobile
//
//  Created by AndreyG-AF on 10/7/17.
//

#import "AppsFlyerXAppleDelegate.h"
#include "AppsFlyerXAppleHelper.h"

@implementation AppsFlyerXAppleDelegate

- (void)onConversionDataReceived:(NSDictionary*)installData {
    if (_onConversionDataReceivedCallback) {
        _onConversionDataReceivedCallback(AppsFlyerXAppleHelper::nsDictionary2ValueMap(installData));
    }
}

- (void)onConversionDataRequestFailure:(NSError *)error {
    if (_onConversionDataRequestFailureCallback) {
        NSDictionary * errorDictionary = @{@"errorCode":[NSNumber numberWithInteger:error.code],
                                           @"errorDescription":error.localizedDescription};
        _onConversionDataRequestFailureCallback(AppsFlyerXAppleHelper::nsDictionary2ValueMap(errorDictionary));
    }
}

- (void)onAppOpenAttribution:(NSDictionary*)attributionData {
    if (_onAppOpenAttributionCallback) {
        _onAppOpenAttributionCallback(AppsFlyerXAppleHelper::nsDictionary2ValueMap(attributionData));
    }
}

- (void) onAppOpenAttributionFailure:(NSError *)error {
    if (_onAppOpenAttributionFailureCallback) {
        NSDictionary * errorDictionary = @{@"errorCode":[NSNumber numberWithInteger:error.code],
                                           @"errorDescription":error.localizedDescription};
        _onAppOpenAttributionFailureCallback(AppsFlyerXAppleHelper::nsDictionary2ValueMap(errorDictionary));
    }
}

@end
