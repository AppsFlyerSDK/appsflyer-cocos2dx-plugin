import Foundation
import AppsFlyerRPC

private let awaitTimeoutSeconds: TimeInterval = 30
private let rpcLogTag = "AppsFlyer_Cocos2dX_RPC"
private let eventBufferMax = 64

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
            NSLog("%@: executeRpcBlocking timed out for %@", rpcLogTag, wireMethod)
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

// MARK: - AppsFlyerRPC (main-thread isolated; C++ may call from a worker thread)

private enum AFCocosRPCBridge {
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

// MARK: - Event bus (pre-install buffer + main-queue publish)

private enum AppsFlyerCocosEventBus {
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
            if buffer.count >= eventBufferMax {
                buffer.removeFirst()
                NSLog("%@: RPC event buffer overflow; dropped oldest event", rpcLogTag)
            }
            buffer.append(eventJson)
            lock.unlock()
        }
    }
}

// MARK: - RPC response envelope (Android-aligned success/error JSON for C++)

private enum AppsFlyerCocosRpcResponse {
    static func normalizeError(code: Int, message: String) -> String {
        let error: [String: Any] = ["code": code, "message": message]
        let obj: [String: Any] = ["success": false, "error": error]
        return jsonString(obj) ?? "{\"success\":false,\"error\":{\"code\":500,\"message\":\"encode failed\"}}"
    }

    /// Maps AppsFlyerRPC `{result|error}` envelope to Android-bridge `{success,data|error}` for C++.
    static func normalizeBridgeResponse(_ responseJson: String) -> String {
        guard let data = responseJson.data(using: .utf8),
              let parsed = try? JSONSerialization.jsonObject(with: data) as? [String: Any] else {
            return normalizeError(code: 500, message: "Failed to parse RPC response")
        }

        if let envelopeError = parsed["error"] as? [String: Any], !envelopeError.isEmpty {
            let code = intCode(envelopeError["code"]) ?? 500
            let message = (envelopeError["message"] as? String) ?? "RPC error"
            return normalizeError(code: code, message: message)
        }

        guard let resultObj = parsed["result"] as? [String: Any] else {
            return "{\"success\":true,\"data\":null}"
        }

        let success = (resultObj["success"] as? NSNumber)?.boolValue ?? true
        if !success {
            let code = intCode(resultObj["errorCode"]) ?? 500
            let message = (resultObj["error"] as? String)
                ?? (resultObj["message"] as? String)
                ?? "RPC operation failed"
            return normalizeError(code: code, message: message)
        }

        let dataField = resultObj["data"] ?? NSNull()
        let out: [String: Any] = ["success": true, "data": dataField]
        return jsonString(out) ?? "{\"success\":true,\"data\":null}"
    }

    private static func intCode(_ value: Any?) -> Int? {
        if let n = value as? NSNumber { return n.intValue }
        if let s = value as? String, let i = Int(s) { return i }
        return nil
    }

    private static func jsonString(_ obj: [String: Any]) -> String? {
        guard let data = try? JSONSerialization.data(withJSONObject: obj, options: []),
              let s = String(data: data, encoding: .utf8) else { return nil }
        return s
    }
}
