//
//  AppController+AppsFlyerX_AppController.m
//  AppsFlyerCocos2dX-mobile
//
//  Created by AndreyG-AF on 10/17/17.
//

#import "AppsFlyerX+AppController.h"
#import <objc/runtime.h>
#import "AppsFlyer/libAppsFlyer/AppsFlyerLib.h"

@implementation AppController (AppsFlyerX)

static BOOL isOriginalImpementationExist;

+ (void)load {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        Class class = [self class];
        
        SEL originalSelector = @selector(application: continueUserActivity: restorationHandler:);
        SEL swizzledSelector = @selector(x_application: continueUserActivity: restorationHandler:);
        
        Method originalMethod = class_getInstanceMethod(class, originalSelector);
        Method swizzledMethod = class_getInstanceMethod(class, swizzledSelector);
        
        isOriginalImpementationExist = [class instancesRespondToSelector:originalSelector];
        
        BOOL didAddMethod =
        class_addMethod(class,
                        originalSelector,
                        method_getImplementation(swizzledMethod),
                        method_getTypeEncoding(swizzledMethod));
        
        if (didAddMethod) {
            class_replaceMethod(class,
                                swizzledSelector,
                                method_getImplementation(originalMethod),
                                method_getTypeEncoding(originalMethod));
        } else {
            method_exchangeImplementations(originalMethod, swizzledMethod);
        }
    });
}

#pragma mark - Method Swizzling

- (BOOL)x_application:(UIApplication *)application continueUserActivity:(NSUserActivity *)userActivity restorationHandler:(void (^)(NSArray * _Nullable))restorationHandler {
    [[AppsFlyerLib shared] continueUserActivity:userActivity restorationHandler:restorationHandler];
    
    if (isOriginalImpementationExist) {
        [self x_application:application continueUserActivity:userActivity restorationHandler:restorationHandler];
    }
    return YES;
}

@end
