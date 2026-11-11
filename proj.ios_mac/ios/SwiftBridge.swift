// Ensures the Swift runtime is linked into the app target (legacy cocos sample pattern).
import Foundation

@objc(SwiftBridge)
final class SwiftBridge: NSObject {
    @objc static func linkSwiftRuntime() {}
}
