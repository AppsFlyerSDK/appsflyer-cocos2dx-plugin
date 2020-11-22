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

using namespace cocos2d;

class AppsFlyerXAppleHelper {
    
public:
    static ValueMap nsDictionary2ValueMap(NSDictionary *dic);
    static void nsDictionary2ValueMap(NSDictionary *dic, ValueMap &vm);
    
    static ValueVector nsArray2ValueVector(NSArray *array);
    static void nsArray2ValueVector(NSArray *array, ValueVector &vv);
    
    static NSDictionary *valueMap2nsDictionary(ValueMap &vm);
    static NSArray *valueVector2nsArray(ValueVector &vv);
};

#endif /* AppsFlyerXAppleHelper_h */
