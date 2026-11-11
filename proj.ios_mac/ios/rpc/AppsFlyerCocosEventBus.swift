import Foundation

private let maxBuffer = 64
private let logTag = "AppsFlyer_Cocos2dX_RPC"

enum AppsFlyerCocosEventBus {
    private static let lock = NSLock()
    private static var buffer: [String] = []
    private static var deliver: ((String) -> Void)?

    static func attach(_ handler: @escaping (String) -> Void) {
        lock.lock()
        deliver = handler
        let pending = buffer
        buffer.removeAll()
        lock.unlock()
        pending.forEach { handler($0) }
    }

    static func publish(_ eventJson: String) {
        DispatchQueue.main.async {
            lock.lock()
            if let deliver {
                lock.unlock()
                deliver(eventJson)
                return
            }
            if buffer.count >= maxBuffer {
                buffer.removeFirst()
                NSLog("%@: RPC event buffer overflow; dropped oldest event", logTag)
            }
            buffer.append(eventJson)
            lock.unlock()
        }
    }
}
