//
//  AppsFlyerXAppleHelper.m
//  AppsFlyerCocos2dX
//
//  Created by Andrey Gagan on 10/5/17.
//  AppsFlyer

#include "AppsFlyerXAppleHelper.h"
#include "AppsFlyerXAppleDeepLinkResult.h"


cocos2d::ValueMap AppsFlyerXAppleHelper::nsDictionary2ValueMap(NSDictionary *dic) {
    cocos2d::ValueMap vm;
    nsDictionary2ValueMap(dic, vm);
    return vm;
}

void AppsFlyerXAppleHelper::nsDictionary2ValueMap(NSDictionary *dic, cocos2d::ValueMap &vm) {
    NSArray *keys = [dic allKeys];
    for (NSString *k : keys) {
        id obj = dic[k];
        std::string key([k cStringUsingEncoding:NSUTF8StringEncoding]);
        
        if ([obj isKindOfClass:[NSDictionary class]]) {
            //Dictionary
            cocos2d::ValueMap vmm;
            nsDictionary2ValueMap((NSDictionary *) obj, vmm);
            vm[key] = vmm;
        } else if ([obj isKindOfClass:[NSArray class]]) {
            //Array
            cocos2d::ValueVector vv;
            nsArray2ValueVector((NSArray *) obj, vv);
            vm[key] = vv;
        } else if ([obj isKindOfClass:[NSString class]]) {
            //String
            vm[key] = [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
        } else if ([obj isKindOfClass:[NSNumber class]]) {
            //Number
            vm[key] = [[(NSNumber *)obj stringValue] cStringUsingEncoding:NSUTF8StringEncoding];
        } else if ([obj isKindOfClass:[NSNull class]]) {
            //Null
            vm[key] = cocos2d::Value::Null;
        } else {
            NSLog(@"%s - Non supported type %@", __FUNCTION__, [obj class]);
        }
    }
}

ValueVector AppsFlyerXAppleHelper::nsArray2ValueVector(NSArray *array) {
    ValueVector vv;
    nsArray2ValueVector(array, vv);
    return vv;
}

void AppsFlyerXAppleHelper::nsArray2ValueVector(NSArray *array, ValueVector &vv) {
    for (id obj : array) {
        if ([obj isKindOfClass:[NSDictionary class]]) {
            //Dictionary
            ValueMap vm;
            nsDictionary2ValueMap((NSDictionary *) obj, vm);
            vv.push_back(Value(vm));
        } else if ([obj isKindOfClass:[NSArray class]]) {
            //Array
            ValueVector vvv;
            nsArray2ValueVector((NSArray *) obj, vvv);
            vv.push_back(Value(vvv));
        } else if ([obj isKindOfClass:[NSString class]]) {
            //String
            vv.push_back(Value([(NSString *)obj cStringUsingEncoding:NSUTF8StringEncoding]));
        } else if ([obj isKindOfClass:[NSNumber class]]) {
            //Number
            vv.push_back(Value([obj stringValue]));
        } else {
            NSLog(@"%s - Non supported type %@", __FUNCTION__, [obj class]);
        }
    }
}

NSDictionary *AppsFlyerXAppleHelper::valueMap2nsDictionary(ValueMap &vm) {
    NSMutableDictionary *dic = [NSMutableDictionary dictionary];
    for (auto i : vm) {
        NSString *key = [NSString stringWithUTF8String:i.first.c_str()];
        switch (i.second.getType()) {
            case Value::Type::MAP:
                dic[key] = valueMap2nsDictionary(i.second.asValueMap());
                break;
                
            case Value::Type::VECTOR:
                dic[key] = valueVector2nsArray(i.second.asValueVector());
                break;
                
            default:
            {   if([key isEqualToString:@"some_test_remove_it"])
            {
                NSString* pngPath = [NSString stringWithUTF8String:i.second.asString().c_str()];
                UIImage *image = [UIImage imageWithContentsOfFile:pngPath];
                NSData *data = UIImagePNGRepresentation(image);
                dic[key] = data;
                
            }
            else
            {
                dic[key] = [NSString stringWithUTF8String:i.second.asString().c_str()];
            }
                break;
            }
        }
    }
    return dic;
}

NSArray *AppsFlyerXAppleHelper::valueVector2nsArray(ValueVector &vv) {
    NSMutableArray *array = [NSMutableArray arrayWithCapacity:vv.size()];
    for (auto i : vv) {
        switch (i.getType()) {
            case Value::Type::MAP:
                [array addObject:valueMap2nsDictionary(i.asValueMap())];
                break;
            case Value::Type::VECTOR:
                [array addObject:valueVector2nsArray(i.asValueVector())];
                break;
            default:
                [array addObject:[NSString stringWithUTF8String:i.asString().c_str()]];
                break;
        }
    }
    return array;
}

ValueMap AppsFlyerXAppleHelper::deepLinkResult2ValueMap(AppsFlyerDeepLinkResult *result) {
    ValueMap vm;
    deepLinkResult2ValueMap(result, vm);
    return vm;
}


void AppsFlyerXAppleHelper::deepLinkResult2ValueMap(AppsFlyerDeepLinkResult *result, ValueMap &vm) {
    AFSDKDeepLinkResultStatus status = [result status];
    std::string statusDL;
    switch (status) {
        case AFSDKDeepLinkResultStatusNotFound:
            statusDL = "notFound";
            break;
        case AFSDKDeepLinkResultStatusFound:
            statusDL = "found";
            break;
        case AFSDKDeepLinkResultStatusFailure:
            statusDL = "failure";
            break;
        default:
            break;
    }
    vm["status"] = Value(statusDL);

    //AppsFlyerXAppleDeepLink *deepLink = [[AppsFlyerXAppleDeepLink alloc] init];
    ValueMap deepLink;
    id obj = [result deepLink].afSub1 ? [result deepLink].afSub1 : @"";
    deepLink["afSub1"] = [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
    obj = [result deepLink].afSub2 ? [result deepLink].afSub2 : @"";
    deepLink["afSub2"] =  [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
    obj = [result deepLink].afSub3  ? [result deepLink].afSub3 : @"";
    deepLink["afSub3"] =  [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
    obj = [result deepLink].afSub4 ? [result deepLink].afSub4 : @"";
    deepLink["afSub4"] =  [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
    obj = [result deepLink].afSub5 ?  [result deepLink].afSub5  : @"";
    deepLink["afSub5"] =  [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
    obj = [result deepLink].deeplinkValue ? [result deepLink].deeplinkValue : @"";
    deepLink["deeplinkValue"] = [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
    obj = [result deepLink].matchType  ? [result deepLink].matchType : @"";
    deepLink["matchType"] =  [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
    obj = [result deepLink].clickHTTPReferrer ? [result deepLink].clickHTTPReferrer : @"";
    deepLink["clickHTTPReferrer"] =  [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
    obj = [result deepLink].mediaSource ?  [result deepLink].mediaSource : @"";
    deepLink["mediaSource"] = [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
    obj = [result deepLink].campaign ?  [result deepLink].campaign : @"";
    deepLink["campaign"] =  [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
    obj = [result deepLink].campaignId ? [result deepLink].campaignId : @"";
    deepLink["campaignId"] =  [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
    deepLink["isDeferred"] = [result deepLink].isDeferred;
    obj = [result deepLink].clickEvent;
    ValueMap vmm;
    nsDictionary2ValueMap((NSDictionary *) obj, vmm);
    deepLink["clickEvent"] =  vmm;
    vm["deepLink"] = deepLink;
    vm["error"] = nil;
    if ([result error] != nil)
    {
    NSDictionary * errorDictionary = @{@"errorCode":[NSNumber numberWithInteger:[result error].code],
                                       @"errorDescription":[result error].localizedDescription};
    vm["error"] = errorDictionary;
    }
}

AppsFlyerXAppleDeepLinkResult AppsFlyerXAppleHelper::deepLinkResult2XDeepLinkResult(AppsFlyerDeepLinkResult* result){
    AppsFlyerXAppleDeepLinkResult xresult;
    //AppsFlyerXAppleDeepLink *deepLink = [[AppsFlyerXAppleDeepLink alloc] init];
    deepLinkResult2XDeepLinkResult(result, xresult);
    return xresult;
}

void AppsFlyerXAppleHelper::deepLinkResult2XDeepLinkResult(AppsFlyerDeepLinkResult *result, AppsFlyerXAppleDeepLinkResult xresult) {
    AFSDKDeepLinkResultStatus status = [result status];
    DeepLinkResultStatus xstatus;
    switch (status) {
        case AFSDKDeepLinkResultStatusNotFound:
            xstatus = NOTFOUND;
            break;
        case AFSDKDeepLinkResultStatusFound:
            xstatus = FOUND;
            break;
        case AFSDKDeepLinkResultStatusFailure:
            xstatus = FAILURE;
            break;
        default:
            break;
    }
    xresult.status = xstatus;
    if ([result deepLink] != nil)
    {
//        AppsFlyerXAppleDeepLink xdeepLink;
//        id obj = [result deepLink].afSub1 ? [result deepLink].afSub1 : @"";
//        xdeepLink.afSub1 = [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
//        obj = [result deepLink].afSub2 ? [result deepLink].afSub2 : @"";
//        xdeepLink.afSub2  =  [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
//        obj = [result deepLink].afSub3  ? [result deepLink].afSub3 : @"";
//        xdeepLink.afSub3 =  [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
//        obj = [result deepLink].afSub4 ? [result deepLink].afSub4 : @"";
//        xdeepLink.afSub4 =  [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
//        obj = [result deepLink].afSub5 ?  [result deepLink].afSub5  : @"";
//        xdeepLink.afSub5 =  [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
//        obj = [result deepLink].deeplinkValue ? [result deepLink].deeplinkValue : @"";
//        xdeepLink.deeplinkValue = [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
//        obj = [result deepLink].matchType  ? [result deepLink].matchType : @"";
//        xdeepLink.matchType  =  [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
//        obj = [result deepLink].clickHTTPReferrer ? [result deepLink].clickHTTPReferrer : @"";
//        xdeepLink.clickHTTPReferrer =  [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
//        obj = [result deepLink].mediaSource ?  [result deepLink].mediaSource : @"";
//        xdeepLink.mediaSource = [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
//        obj = [result deepLink].campaign ?  [result deepLink].campaign : @"";
//        xdeepLink.campaign =  [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
//        obj = [result deepLink].campaignId ? [result deepLink].campaignId : @"";
//        xdeepLink.campaignId =  [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
//        xdeepLink.isDeferred = [result deepLink].isDeferred;
//        ValueMap xclickEvent;
//        nsDictionary2ValueMap([result deepLink].clickEvent, xclickEvent);
//        xdeepLink.clickEvent = xclickEvent;
//        xresult.deepLink = &xdeepLink;
        ValueMap deepLink;
        id clickEvent = [result deepLink].clickEvent;
        for (NSString *key in clickEvent)
        {
            deepLink[[(NSString *) key cStringUsingEncoding:NSUTF8StringEncoding]] = clickEvent[key];
        }
    }
    if ([result error] != nil){
        std::string error = std::string([[result error].localizedDescription UTF8String]);
        DeepLinkError xerror;
       if (error == "TIMEOUT")
                xerror = TIMEOUT;
        else if (error == "NETWORK")
                xerror = NETWORK;
        else if (error == "HTTP_STATUS_CODE")
                xerror = HTTP_STATUS_CODE;
        xresult.error = xerror;
    }
//        id obj = [result deepLink].afSub1 ? [result deepLink].afSub1 : @"";
//        deepLink["afSub1"] = [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
//        obj = [result deepLink].afSub2 ? [result deepLink].afSub2 : @"";
//        deepLink["afSub2"] =  [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
//        obj = [result deepLink].afSub3  ? [result deepLink].afSub3 : @"";
//        deepLink["afSub3"] =  [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
//        obj = [result deepLink].afSub4 ? [result deepLink].afSub4 : @"";
//        deepLink["afSub4"] =  [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
//        obj = [result deepLink].afSub5 ?  [result deepLink].afSub5  : @"";
//        deepLink["afSub5"] =  [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
//        obj = [result deepLink].deeplinkValue ? [result deepLink].deeplinkValue : @"";
//        deepLink["deeplinkValue"] = [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
//        obj = [result deepLink].matchType  ? [result deepLink].matchType : @"";
//        deepLink["matchType"] =  [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
//        obj = [result deepLink].clickHTTPReferrer ? [result deepLink].clickHTTPReferrer : @"";
//        deepLink["clickHTTPReferrer"] =  [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
//        obj = [result deepLink].mediaSource ?  [result deepLink].mediaSource : @"";
//        deepLink["mediaSource"] = [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
//        obj = [result deepLink].campaign ?  [result deepLink].campaign : @"";
//        deepLink["campaign"] =  [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
//        obj = [result deepLink].campaignId ? [result deepLink].campaignId : @"";
//        deepLink["campaignId"] =  [(NSString *) obj cStringUsingEncoding:NSUTF8StringEncoding];
//        deepLink["isDeferred"] = [result deepLink].isDeferred;
//        obj = [result deepLink].clickEvent;
//        ValueMap vmm;
//        nsDictionary2ValueMap((NSDictionary *) obj, vmm);
//        deepLink["clickEvent"] =  vmm;
//        xresult.deepLink = deepLink;
        
        
 //   }
//    if ([result error] != nil)
//    {
//        ValueMap xError;
//        xError["errorCode"] = [NSNumber numberWithInteger:[result error].code];
//        xError["errorDescription"] = [result error].localizedDescription;
//    }
}
