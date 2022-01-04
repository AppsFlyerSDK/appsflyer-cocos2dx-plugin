//
//  AppsFlyerXAppleHelper.m
//  AppsFlyerCocos2dX
//
//  Created by Andrey Gagan on 10/5/17.
//  AppsFlyer

#include "AppsFlyerXAppleHelper.h"
#include "AppsFlyerXDeepLinkResult.h"


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

AppsFlyerXDeepLinkResult AppsFlyerXAppleHelper::deepLinkResult2XDeepLinkResult(AppsFlyerDeepLinkResult* result){
    AppsFlyerXDeepLinkResult xresult;
    //AppsFlyerXAppleDeepLink *deepLink = [[AppsFlyerXAppleDeepLink alloc] init];
    xresult.deepLink = {};
    deepLinkResult2XDeepLinkResult(result, xresult);
    return xresult;
}

void AppsFlyerXAppleHelper::deepLinkResult2XDeepLinkResult(AppsFlyerDeepLinkResult *result, AppsFlyerXDeepLinkResult &xresult) {
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
        ValueMap deepLink;
        id clickEvent = [result deepLink].clickEvent;
        xresult.deepLink = AppsFlyerXAppleHelper::nsDictionary2ValueMap(clickEvent);
    }
    DeepLinkError xerror = NONE;
    if ([result error] != nil){
        std::string error = std::string([[result error].localizedDescription UTF8String]);
        if (error == "TIMEOUT")
            xerror = TIMEOUT;
        else if (error == "NETWORK")
            xerror = NETWORK;
        else if (error == "HTTP_STATUS_CODE")
            xerror = HTTP_STATUS_CODE;
    }
    xresult.error = xerror;
}


AppsFlyerLinkGenerator*  AppsFlyerXAppleHelper::valueMap2LinkGenerator(ValueMap vm, AppsFlyerLinkGenerator *generator){
    
    NSMutableDictionary *dictionary = AppsFlyerXAppleHelper::valueMap2nsDictionary(vm).mutableCopy;
    NSArray* generatorKeys = @[@"channel", @"customerID", @"campaign", @"referrerName", @"referrerImageUrl", @"deeplinkPath", @"baseDeeplink", @"brandDomain"];
    
    NSMutableDictionary* mutableDictionary = [dictionary mutableCopy];
    
    [generator setChannel:[dictionary objectForKey: @"channel"]];
    [generator setReferrerCustomerId:[dictionary objectForKey: @"customerID"]];
    [generator setCampaign:[dictionary objectForKey: @"campaign"]];
    [generator setReferrerName:[dictionary objectForKey: @"referrerName"]];
    [generator setReferrerImageURL:[dictionary objectForKey: @"referrerImageUrl"]];
    [generator setDeeplinkPath:[dictionary objectForKey: @"deeplinkPath"]];
    [generator setBaseDeeplink:[dictionary objectForKey: @"baseDeeplink"]];
    [generator setBrandDomain:[dictionary objectForKey: @"brandDomain"]];
    
    
    [mutableDictionary removeObjectsForKeys:generatorKeys];
    
    [generator addParameters:mutableDictionary];
    return generator;
}
