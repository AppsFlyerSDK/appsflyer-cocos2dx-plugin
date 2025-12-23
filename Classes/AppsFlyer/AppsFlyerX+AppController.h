//
//  AppController+AppsFlyerX_AppController.h
//  AppsFlyerCocos2dX-mobile
//
//  Created by AndreyG-AF on 10/17/17.
//

#if __has_include("AppController.h")
#import "AppController.h"
#elif __has_include("proj.ios_mac/ios/AppController.h")
#import "proj.ios_mac/ios/AppController.h"
#elif __has_include("../../proj.ios_mac/ios/AppController.h")
#import "../../proj.ios_mac/ios/AppController.h"
#elif __has_include("../../../proj.ios_mac/ios/AppController.h")
#import "../../../proj.ios_mac/ios/AppController.h"
#else
#error "AppController.h is required by the AppsFlyer category."
#endif

@interface AppController (AppsFlyerX)

@end
