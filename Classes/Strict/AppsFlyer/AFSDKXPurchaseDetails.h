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


class AFSDKXPurchaseDetails {
public:
    // Deleted default constructor to prevent usage.
    AFSDKXPurchaseDetails() = delete;

    // Constructor with parameters to initialize the properties.
    AFSDKXPurchaseDetails(const std::string &productId,
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

private:
    // Private member variables.
    std::string productId_;
    std::string price_;
    std::string currency_;
    std::string transactionId_;
};
#endif /* AFSDKXPurchaseDetails_h */
