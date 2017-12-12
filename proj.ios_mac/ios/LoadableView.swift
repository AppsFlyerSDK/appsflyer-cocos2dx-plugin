//
//  LoadableView.swift
//  AppsFlyerCocos2dX-mobile
//
//  Created by AndreyG-AF on 12/7/17.
//

import Foundation
import UIKit

/// Protocol to define family of loadable views
public protocol NibLoadableProtocol : NSObjectProtocol {
    
    /// View that serves as a container for loadable view. Loadable views are added to container view in `setupNib(_:)` method.
    var nibContainerView: UIView { get }
    
    /// Method that loads view from single view xib with `nibName`.
    ///
    /// - returns: loaded from xib view
    func loadNib() -> UIView
    
    /// Method that is used to load and configure loadableView. It is then added to `nibContainerView` as a subview. This view receives constraints of same width and height as container view.
    func setupNib()
    
    /// Name of .xib file to load view from.
    var nibName : String { get }
}

extension UIView {
    /// View usually serves itself as a default container for loadable views
    @objc dynamic open var nibContainerView : UIView { return self }
    
    /// Default nibName for all UIViews, equal to name of the class.
    @objc dynamic open var nibName : String { return String(describing: type(of: self)) }
}

extension NibLoadableProtocol {
    /// Method that loads view from single view xib with `nibName`.
    ///
    /// - returns: loaded from xib view
    public func loadNib() -> UIView {
        let bundle = Bundle(for: type(of: self))
        let nib = UINib(nibName: nibName, bundle: bundle)
        let view = nib.instantiate(withOwner: self, options: nil).first as! UIView
        return view
    }
    
    public func setupView(_ view: UIView, inContainer container: UIView) {
        container.translatesAutoresizingMaskIntoConstraints = false
        container.backgroundColor = .clear
        container.addSubview(view)
        view.translatesAutoresizingMaskIntoConstraints = false
        let bindings = ["view": view]
        container.addConstraints(NSLayoutConstraint.constraints(withVisualFormat: "H:|[view]|", options:[], metrics:nil, views: bindings))
        container.addConstraints(NSLayoutConstraint.constraints(withVisualFormat: "V:|[view]|", options:[], metrics:nil, views: bindings))
    }
}

/// UIView subclass, that can be loaded into different xib or storyboard by simply referencing it's class.
open class LoadableView: UIView, NibLoadableProtocol {
    
    public override init(frame: CGRect) {
        super.init(frame: frame)
        setupNib()
    }
    
    public required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        setupNib()
    }
    
    open func setupNib() {
        setupView(loadNib(), inContainer: nibContainerView)
    }
}

