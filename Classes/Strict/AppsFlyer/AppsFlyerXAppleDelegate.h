//
//  AppsFlyerXAppleDelegate.h
//  AppsFlyerCocos2dX
//
//  Created by AndreyG-AF on 10/7/17.
//
#ifndef AppsFlyerXAppleDelegate_h
#define AppsFlyerXAppleDelegate_h

#import "AppsFlyerLib.h"


@interface AppsFlyerXAppleDelegate: NSObject<AppsFlyerLibDelegate>

@property (nonatomic) void (* onConversionDataReceivedCallback)(cocos2d::ValueMap installData);
@property (nonatomic) void (* onConversionDataRequestFailureCallback)(cocos2d::ValueMap error);
@property (nonatomic) void (* onAppOpenAttributionCallback)(cocos2d::ValueMap attributionData);
@property (nonatomic) void (* onAppOpenAttributionFailureCallback)(cocos2d::ValueMap error);

@end
#endif /* AppsFlyerXAppleDelegate_h */
