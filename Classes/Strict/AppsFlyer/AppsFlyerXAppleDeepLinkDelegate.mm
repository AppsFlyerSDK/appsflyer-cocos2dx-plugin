//
//  AppsFlyerXAppleDeepLinkDelegate.m
//  AppsFlyerCocos2dX
//
//  Created by Margot Guetta on 17/11/2020.
//

#import "AppsFlyerXAppleDeepLinkDelegate.h"


@implementation AppsFlyerXAppleDeepLinkDelegate

- (void)didResolveDeepLink:(AppsFlyerDeepLinkResult *_Nonnull)result {
    if (_didResolveDeepLinkCallback){
        _didResolveDeepLinkCallback(AppsFlyerXAppleHelper::deepLinkResult2XDeepLinkResult(result));
    }
}

@end
