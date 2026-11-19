import AppsFlyerLib
import Foundation

/// UIKit deep-link entry points for Cocos. `handleOpen` / `continueUserActivity` buffer until
/// `AppsFlyerCocosRpcBridge` completes a successful `initialize` RPC; `handleLaunchOptions` forwards immediately.
@objc(AppsFlyerCocosAttribution)
public final class AppsFlyerCocosAttribution: NSObject {

    @objc public static let shared = AppsFlyerCocosAttribution()

    private let lock = NSLock()
    private var _bridgeReady = false

    @objc public var bridgeReady: Bool {
        get { lock.withCriticalScope { _bridgeReady } }
        set {
            lock.withCriticalScope { _bridgeReady = newValue }
            if newValue { flushPending() }
        }
    }

    private var pendingUserActivity: NSUserActivity?
    private var pendingUrl: URL?
    private var pendingOptions: [AnyHashable: Any] = [:]

    private override init() {}

    @objc public func continueUserActivity(
        _ userActivity: NSUserActivity,
        restorationHandler: (([Any]?) -> Void)? = nil
    ) {
        let shouldBuffer: Bool = lock.withCriticalScope {
            guard !_bridgeReady else { return false }
            pendingUserActivity = userActivity
            return true
        }
        guard !shouldBuffer else { return }
        AppsFlyerLib.shared().continue(userActivity, restorationHandler: restorationHandler)
    }

    @objc public func handleOpen(_ url: URL, options: [AnyHashable: Any] = [:]) {
        let shouldBuffer: Bool = lock.withCriticalScope {
            guard !_bridgeReady else { return false }
            pendingUrl = url
            pendingOptions = options
            return true
        }
        guard !shouldBuffer else { return }
        AppsFlyerLib.shared().handleOpen(url, options: options)
    }

    @objc public func handleLaunchOptions(_ launchOptions: [AnyHashable: Any]?) {
        AppsFlyerLib.shared().handleLaunchOptions(launchOptions)
    }

    private func flushPending() {
        let (url, options, userActivity): (URL?, [AnyHashable: Any], NSUserActivity?) = lock.withCriticalScope {
            defer {
                pendingUrl = nil
                pendingOptions = [:]
                pendingUserActivity = nil
            }
            return (pendingUrl, pendingOptions, pendingUserActivity)
        }
        if let url {
            AppsFlyerLib.shared().handleOpen(url, options: options)
        }
        if let userActivity {
            AppsFlyerLib.shared().continue(userActivity, restorationHandler: nil)
        }
    }
}

private extension NSLock {
    func withCriticalScope<T>(_ body: () -> T) -> T {
        lock()
        defer { unlock() }
        return body()
    }
}