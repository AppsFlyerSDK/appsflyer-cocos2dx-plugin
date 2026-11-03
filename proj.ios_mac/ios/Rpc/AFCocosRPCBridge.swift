import Foundation
import AppsFlyerRPC

/// Same contract as Flutter's AFRPCBridge — synchronous RPC from the main thread.
enum AFCocosRPCBridge {
    @MainActor private static weak var eventHandlerOwner: AnyObject?

    static func executeJson(_ jsonRequest: String, completion: @escaping (String) -> Void) {
        onMainActor {
            AppsFlyerRPCBridge.shared.executeJson(jsonRequest) { response in
                onMainActor { completion(response) }
            }
        }
    }

    static func setEventHandler(owner: AnyObject, _ handler: @escaping (String) -> Void) {
        onMainActor {
            eventHandlerOwner = owner
            AppsFlyerRPCBridge.shared.setEventHandler { jsonEvent in
                DispatchQueue.main.async { handler(jsonEvent) }
            }
        }
    }

    static func removeEventHandler(owner: AnyObject) {
        onMainActor {
            guard eventHandlerOwner === owner else { return }
            eventHandlerOwner = nil
            AppsFlyerRPCBridge.shared.removeEventHandler()
        }
    }

    private static func onMainActor(_ body: @escaping @MainActor () -> Void) {
        if Thread.isMainThread {
            MainActor.assumeIsolated(body)
        } else {
            DispatchQueue.main.async {
                MainActor.assumeIsolated(body)
            }
        }
    }
}
