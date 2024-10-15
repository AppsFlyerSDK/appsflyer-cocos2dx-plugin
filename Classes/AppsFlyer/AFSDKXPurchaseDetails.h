//
//  AFSDKXPurchaseDetails.h
//  MyGame
//
//  Created by ivan.obodovskyi on 02.10.2024.
//

#ifndef AFSDKXPurchaseDetails_h
#define AFSDKXPurchaseDetails_h

#include <string>
#include "cocos2d.h"
#include "AFSDKXValidateAndLogResult.h"

enum class AFXPurchaseType {
    SUBSCRIPTION,
    ONE_TIME_PURCHASE,
    APPLE
};


class AFSDKXPurchaseDetails {
public:
    // Deleted default constructor to prevent usage.
    AFSDKXPurchaseDetails() = delete;

    // Constructor with parameters to initialize the properties.
    AFSDKXPurchaseDetails(const AFXPurchaseType &purchaseType,
                                                 const std::string &purchaseToken,
                                                 const std::string &productId,
                                                 const std::string &price,
                                                 const std::string &currency,
                                                 const std::string &transactionId);

    // Default destructor.
    virtual ~AFSDKXPurchaseDetails() = default;

    // Public assignment and copy constructors
    AFSDKXPurchaseDetails(const AFSDKXPurchaseDetails &) = default;
    AFSDKXPurchaseDetails &operator=(const AFSDKXPurchaseDetails &) = default;

    // Getters and setters for private member variables.
    std::string getProductId() const;
    void setProductId(const std::string &productId);

    std::string getPrice() const;
    void setPrice(const std::string &price);

    std::string getCurrency() const;
    void setCurrency(const std::string &currency);

    std::string getTransactionId() const;
    void setTransactionId(const std::string &transactionId);

    // Propperties related to Android impl
    // For Apple implementation, please pass APPLE, it will be interpreted as an empty string
    std::string purchaseTypeToString() const;
    std::string getPurchaseType() const;
    void setPurchaseType(AFXPurchaseType &purchaseType);

    // Android purchase token, for iOS impl, please use empty string
    std::string getPurchaseToken() const;
    void setPurchaseToken(const std::string &purchaseToken);

private:
    // Private member variables.
    std::string productId_;
    std::string price_;
    std::string currency_;
    std::string transactionId_;
    std::string purchaseToken_;
    AFXPurchaseType purchaseType_;
};
#endif /* AFSDKXPurchaseDetails_h */
