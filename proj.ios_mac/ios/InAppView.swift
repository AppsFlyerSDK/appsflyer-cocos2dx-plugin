
//  InAppView.swift
//  AppsFlyerCocos2dX-mobile
//
//  Created by AndreyG-AF on 12/7/17.
//

import UIKit
import SwiftyStoreKit

let unknown = "unknown"

enum RegisteredPurchase: String {
    case consumablePurchase = "one"
    case nonConsumablePurchase = "non.cons"
    case autoRenewablePurchase = "auto.renew"
    case nonRenewingPurchase = "non.renew"
}

enum PurchaseValidator {
    case apple
    case appsflyer
}


@objc class InAppView: LoadableView {

    weak var viewController: UIViewController!
    let appBundleId = "com.appsflyer.inapppurchase"
    var cachedPurchases = [String:String]()
    
    @IBAction func onButton1Pressed(_ sender: Any) {
        getInfo(.consumablePurchase)
    }
    
    @IBAction func onButton2Pressed(_ sender: Any) {
        purchase(.consumablePurchase)
    }
    
    @IBAction func onButton3Pressed(_ sender: Any) {
        getInfo(.nonConsumablePurchase)
    }
    
    @IBAction func onButton4Pressed(_ sender: Any) {
        purchase(.nonConsumablePurchase)
    }
    
    
    func getInfo(_ purchase: RegisteredPurchase) {
        
        SwiftyStoreKit.retrieveProductsInfo([appBundleId + "." + purchase.rawValue]) {
            [unowned self] in
            
            self.viewController.showAlert(self.viewController.alertForProductRetrievalInfo($0))
        }
    }
    
    func purchase(_ registeredPurchase: RegisteredPurchase) {
        
        SwiftyStoreKit.purchaseProduct(appBundleId + "." + registeredPurchase.rawValue, atomically: true) {
            [unowned self] in
            
            if case .success(let purchase) = $0 {
                // Deliver content from server, then:
                if purchase.needsFinishTransaction {
                    SwiftyStoreKit.finishTransaction(purchase.transaction)
                }
            }
            if let alert = self.viewController.alertForPurchaseResult($0) {
                self.viewController.showAlert(alert)
            }
            /// Added caching logic. We can merge it with code at top..
            switch $0 {
            case .success(let purchase):
                self.cachedPurchases[registeredPurchase.rawValue] = purchase.transaction.transactionIdentifier ?? unknown
            case .error:
                self.cachedPurchases[registeredPurchase.rawValue] = nil
            }
        }
    }
    
    @objc func verifyPurchase(callback:@escaping (NSDictionary) -> Void) {
        //TODO:
        let productId = self.appBundleId + "." + "one"
        
        guard let transactionId = cachedPurchases["one"] else {
            fatalError("You cannot call verify before Purchase were made")
        }
        
        SwiftyStoreKit.retrieveProductsInfo([productId]) {
            
            if let product = $0.retrievedProducts.first {
                let priceStr = product.price.stringValue
                let productId = product.productIdentifier
                let currency = "USD"
                
                callback(["priceStr"     :priceStr,
                          "productId"    :productId,
                          "currency"     :currency,
                          "transactionId":transactionId])
            }
        }
    }
}
