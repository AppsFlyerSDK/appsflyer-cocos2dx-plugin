import Foundation

enum AppsFlyerCocosRpcResponse {
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
