//
//  AFSDKXValidateAndLogResult.cpp
//  MyGame
//
//  Created by ivan.obodovskyi on 03.10.2024.
//

#include <stdio.h>
#include "AFSDKXValidateAndLogResult.h"

// Constructor implementation.
AFSDKXValidateAndLogResult::AFSDKXValidateAndLogResult(
    AFSDKXValidateAndLogStatus status,
    const cocos2d::ValueMap& result,
    const cocos2d::ValueMap& errorData,
    const cocos2d::ValueMap& error)
    : status_(status), result_(result), errorData_(errorData), error_(error) {
}

// Getter methods implementation.
AFSDKXValidateAndLogStatus AFSDKXValidateAndLogResult::getStatus() const {
    return status_;
}

cocos2d::ValueMap AFSDKXValidateAndLogResult::getResult() const {
    return result_;
}

cocos2d::ValueMap AFSDKXValidateAndLogResult::getErrorData() const {
    return errorData_;
}

cocos2d::ValueMap AFSDKXValidateAndLogResult::getError() const {
    return error_;
}

// Function to map Objective-C enum to C++ enum
AFSDKXValidateAndLogStatus AFSDKXValidateAndLogResult::objcEnumToCppEnum(int objcStatus) {
    switch (objcStatus) {
        case 0:
            return AFSDKXValidateAndLogStatus::Success;
        case 1:
            return AFSDKXValidateAndLogStatus::Failure;
        case 2:
            return AFSDKXValidateAndLogStatus::Error;
        default:
            // Handle unknown cases, or return a default value
            return AFSDKXValidateAndLogStatus::Error;
    }
}
