//
//  AppsFlyerXAppleHelper.h
//  AppsFlyerCocos2dX
//
//  Created by Andrey Gagan on 10/5/17.
//  AppsFlyer

#ifndef AppsFlyerXAppleHelper_h
#define AppsFlyerXAppleHelper_h

#include "cocos2d.h"
#include <iostream>
#include "AppsFlyerXDeepLinkResult.h"
#import "libAppsFlyer/AppsFlyerLib.h"

//#import <UIKit/UIKit.h>

using namespace cocos2d;

class AppsFlyerXAppleHelper {
    
public:
    static ValueMap nsDictionary2ValueMap(NSDictionary *dic);
    static void nsDictionary2ValueMap(NSDictionary *dic, ValueMap &vm);
    
    static ValueVector nsArray2ValueVector(NSArray *array);
    static void nsArray2ValueVector(NSArray *array, ValueVector &vv);
    
    static NSDictionary *valueMap2nsDictionary(ValueMap &vm);
    static NSArray *valueVector2nsArray(ValueVector &vv);
    
    static ValueMap deepLinkResult2ValueMap(AppsFlyerDeepLinkResult *result);
    static void deepLinkResult2ValueMap(AppsFlyerDeepLinkResult *result, ValueMap &vm);
    
    static AppsFlyerXDeepLinkResult deepLinkResult2XDeepLinkResult(AppsFlyerDeepLinkResult *result);
    static void deepLinkResult2XDeepLinkResult(AppsFlyerDeepLinkResult *result,  AppsFlyerXDeepLinkResult &xresult);
    
    static AppsFlyerLinkGenerator*  valueMap2LinkGenerator(ValueMap vm, AppsFlyerLinkGenerator *generator);
    

};

#endif /* AppsFlyerXAppleHelper_h */
