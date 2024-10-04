//
//  AFSDKXValidateAndLogResult.h
//  MyGame
//
//  Created by ivan.obodovskyi on 03.10.2024.
//

#ifndef AFSDKXValidateAndLogResult_h
#define AFSDKXValidateAndLogResult_h

#include <cocos2d.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <optional>

// Assuming an error class similar to NSError exists in your C++ code.
class Error;

// Enum to represent validation status
enum class AFSDKXValidateAndLogStatus {
    Success,
    Failure,
    Error
};

class AFSDKXValidateAndLogResult {
public:
    // Deleting default constructor to prevent usage.
    AFSDKXValidateAndLogResult() = delete;

    // Constructor with initialization list.
    AFSDKXValidateAndLogResult(AFSDKXValidateAndLogStatus status,
                              const cocos2d::ValueMap& result,
                              const cocos2d::ValueMap& errorData,
                              const std::shared_ptr<Error>& error);

    // Getter methods
    AFSDKXValidateAndLogStatus getStatus() const;
    cocos2d::ValueMap getResult() const;
    cocos2d::ValueMap getErrorData() const;
    std::shared_ptr<Error> getError() const;

private:
    // Member variables.
    AFSDKXValidateAndLogStatus status_;
    cocos2d::ValueMap result_;
    cocos2d::ValueMap errorData_;
    std::shared_ptr<Error> error_;
};

#endif /* AFSDKXValidateAndLogResult_h */
