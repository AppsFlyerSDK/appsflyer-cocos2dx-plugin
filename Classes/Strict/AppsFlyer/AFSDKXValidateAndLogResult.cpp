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
    const std::shared_ptr<Error>& error)
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

std::shared_ptr<Error> AFSDKXValidateAndLogResult::getError() const {
    return error_;
}
