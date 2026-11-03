import Foundation

private let awaitTimeoutSeconds: TimeInterval = 30
private let logTag = "AppsFlyer_Cocos2dX_RPC"

@objc(AppsFlyerCocosRpcBridge)
public final class AppsFlyerCocosRpcBridge: NSObject {
    private static let pipelineOwner = AppsFlyerCocosRpcBridge()
    private static var eventPipelineInstalled = false

    /// Call from AppController before Cocos native startup (mirrors Android Activity attach).
    @objc public static func installEventPipeline() {
        guard !eventPipelineInstalled else { return }
        eventPipelineInstalled = true
        AppsFlyerCocosEventBus.attach { json in
            nativeDeliverRpcEvent(json)
        }
        AFCocosRPCBridge.setEventHandler(owner: pipelineOwner) { json in
            AppsFlyerCocosEventBus.publish(json)
        }
    }

    /// Blocks calling thread until RPC completes. C++ invokes this from a worker thread (POC).
    @objc(executeRpcBlocking:)
    public static func executeRpcBlocking(_ requestJson: String) -> String {
        guard let data = requestJson.data(using: .utf8),
              let request = try? JSONSerialization.jsonObject(with: data) as? [String: Any],
              let method = request["method"] as? String else {
            return AppsFlyerCocosRpcResponse.normalizeError(code: 400, message: "Invalid RPC request JSON")
        }
        let params = (request["params"] as? [String: Any]) ?? [:]
        let wireMethod = mapWireMethod(method)

        guard let envelope = buildEnvelope(method: wireMethod, params: params) else {
            return AppsFlyerCocosRpcResponse.normalizeError(code: 500, message: "Failed to encode RPC envelope")
        }

        let semaphore = DispatchSemaphore(value: 0)
        var responseJson = AppsFlyerCocosRpcResponse.normalizeError(code: 504, message: "RPC timed out")

        AFCocosRPCBridge.executeJson(envelope) { raw in
            responseJson = AppsFlyerCocosRpcResponse.normalizeBridgeResponse(raw)
            semaphore.signal()
        }

        if semaphore.wait(timeout: .now() + awaitTimeoutSeconds) == .timedOut {
            NSLog("%@: executeRpcBlocking timed out for %@", logTag, wireMethod)
        }
        return responseJson
    }

    private static func mapWireMethod(_ method: String) -> String {
        if method == "init" { return "initialize" }
        return method
    }

    private static func buildEnvelope(method: String, params: [String: Any]) -> String? {
        let envelope: [String: Any] = ["method": method, "params": params]
        guard let data = try? JSONSerialization.data(withJSONObject: envelope, options: []),
              let json = String(data: data, encoding: .utf8) else { return nil }
        return json
    }

    private static func nativeDeliverRpcEvent(_ eventJson: String) {
        eventJson.withCString { AppsFlyerCocosRpcNativeDeliverEvent($0) }
    }
}
