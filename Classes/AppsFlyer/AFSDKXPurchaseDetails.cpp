//
//  AFSDKXPurchaseDetails.cpp
//  MyGame
//
//  Created by ivan.obodovskyi on 02.10.2024.
//

#include <stdio.h>
#include "AFSDKXPurchaseDetails.h"

AFSDKXPurchaseDetails::AFSDKXPurchaseDetails(const AFXPurchaseType &purchaseType,
                                             const std::string &purchaseToken,
                                             const std::string &productId,
                                             const std::string &price,
                                             const std::string &currency,
                                             const std::string &transactionId)
    :purchaseType_(purchaseType),
    purchaseToken_(purchaseToken),
    productId_(productId),
    price_(price),
    currency_(currency),
    transactionId_(transactionId) {
}

std::string AFSDKXPurchaseDetails::getProductId() const {
    return productId_;
}

void AFSDKXPurchaseDetails::setProductId(const std::string &productId) {
    productId_ = productId;
}

std::string AFSDKXPurchaseDetails::getPrice() const {
    return price_;
}

void AFSDKXPurchaseDetails::setPrice(const std::string &price) {
    price_ = price;
}

std::string AFSDKXPurchaseDetails::getCurrency() const {
    return currency_;
}

void AFSDKXPurchaseDetails::setCurrency(const std::string &currency) {
    currency_ = currency;
}

std::string AFSDKXPurchaseDetails::getTransactionId() const {
    return transactionId_;
}

void AFSDKXPurchaseDetails::setTransactionId(const std::string &transactionId) {
    transactionId_ = transactionId;
}

std::string AFSDKXPurchaseDetails::getPurchaseToken() const {
    return purchaseToken_;
}

void AFSDKXPurchaseDetails::setPurchaseToken(const std::string &purchaseToken) {
    purchaseToken_ = purchaseToken;
}

std::string AFSDKXPurchaseDetails::getPurchaseType() const {
    std::string purchaseTypeStr = purchaseTypeToString();
    return purchaseTypeStr;
}

void AFSDKXPurchaseDetails::setPurchaseType(AFXPurchaseType &purchaseType) {
    purchaseType_ = purchaseType;
}

// Function to get the string value of the enum
std::string AFSDKXPurchaseDetails::purchaseTypeToString() const{
    switch (purchaseType_) {
        case AFXPurchaseType::SUBSCRIPTION:
            return "subscription";
        case AFXPurchaseType::ONE_TIME_PURCHASE:
            return "one_time_purchase";
        case AFXPurchaseType::APPLE:
            return "";
        default:
            return "";
    }
}
