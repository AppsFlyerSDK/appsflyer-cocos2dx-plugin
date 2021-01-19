//
//  AppsFlyerXAppleDeepLinkDelegate.h
//  AppsFlyerCocos2dX
//
//  Created by Margot Guetta on 17/11/2020.
//

#ifndef AppsFlyerXAppleDeepLinkDelegate_h
#define AppsFlyerXAppleDeepLinkDelegate_h

#include "libAppsFlyer/AppsFlyerLib.h"
#include "AppsFlyerXAppleHelper.h"
#include "AppsFlyerXDeepLinkResult.h"



@interface AppsFlyerXAppleDeepLinkDelegate: NSObject<AppsFlyerDeepLinkDelegate>

@property (nonatomic) void (* _Nullable didResolveDeepLinkCallback)(AppsFlyerXDeepLinkResult result);
@end
#endif /* AppsFlyerXAppleDeepLinkDelegate_h */
