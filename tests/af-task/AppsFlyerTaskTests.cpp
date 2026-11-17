#include "AppsFlyer/generated/AppsFlyerRpcSchema.generated.h"
#include "AppsFlyer/generated/AppsFlyerPluginMetadata.generated.h"
#include "AppsFlyer/AppsFlyerDeepLinkData.h"
#include "AppsFlyer/AppsFlyerRpc.h"

#include <atomic>
#include <cstdio>
#include <string>
#include <thread>
#include <vector>

namespace {

std::atomic<int> g_gameThreadDepth{0};
std::vector<std::string> g_rpcCallOrder;

appsflyer::rpc::RpcResult successEnvelope() {
    appsflyer::rpc::RpcResult r;
    r.success = true;
    r.dataJson = "{\"success\":true}";
    return r;
}

appsflyer::rpc::RpcResult failureEnvelope() {
    appsflyer::rpc::RpcResult r;
    r.success = false;
    r.errorCode = 500;
    r.errorMessage = "mock failure";
    r.dataJson = "{\"success\":false}";
    return r;
}

void syncGameThread(const std::function<void()>& fn) {
    g_gameThreadDepth++;
    fn();
    g_gameThreadDepth--;
}

} // namespace

#define EXPECT(cond)                                                                               \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);                   \
            std::exit(1);                                                                          \
        }                                                                                          \
    } while (0)

static void test_task_completes_on_game_thread() {
    appsflyer::detail::resetTestHooks();
    appsflyer::detail::setGameThreadPosterForTests(syncGameThread);
    appsflyer::detail::setRpcExecutorForTests(
        [](const std::string& method, const std::string&) -> appsflyer::rpc::RpcResult {
            g_rpcCallOrder.push_back(method);
            return successEnvelope();
        });

    std::atomic<bool> completed{false};
    appsflyer::isSessionReady([&completed](const AppsFlyerResult& r) {
        completed = true;
        EXPECT(r.success);
        EXPECT(g_gameThreadDepth.load() > 0);
    });

    for (int i = 0; i < 200 && !completed.load(); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    EXPECT(completed.load());
    EXPECT(g_rpcCallOrder.size() == 1);
    EXPECT(g_rpcCallOrder[0] == "isSessionReady");
}

static void test_deep_link_register_rollback_on_failure() {
    appsflyer::detail::resetTestHooks();
    g_rpcCallOrder.clear();
    appsflyer::detail::setGameThreadPosterForTests(syncGameThread);
    appsflyer::detail::setRpcExecutorForTests(
        [](const std::string& method, const std::string&) -> appsflyer::rpc::RpcResult {
            g_rpcCallOrder.push_back(method);
            if (method == "subscribeForDeepLink") {
                return failureEnvelope();
            }
            return successEnvelope();
        });

    bool firstCallback = false;
    appsflyer::setDeepLinkCallback([](appsflyer::DeepLinkData) { /* prior */ });

    std::atomic<bool> completed{false};
    appsflyer::registerDeepLinkListener(
        [&firstCallback](appsflyer::DeepLinkData) { firstCallback = true; },
        [&completed](const AppsFlyerResult& r) {
            completed = true;
            EXPECT(!r.success);
        });

    for (int i = 0; i < 400 && !completed.load(); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    EXPECT(completed.load());
    EXPECT(!firstCallback);
    EXPECT(!appsflyer::deepLinkHandlerSnapshot());
}

static void test_chain_order_deep_link_before_init() {
    appsflyer::detail::resetTestHooks();
    g_rpcCallOrder.clear();
    appsflyer::detail::setGameThreadPosterForTests(syncGameThread);
    appsflyer::detail::setRpcExecutorForTests(
        [](const std::string& method, const std::string&) -> appsflyer::rpc::RpcResult {
            g_rpcCallOrder.push_back(method);
            return successEnvelope();
        });

    std::atomic<int> step{0};
    appsflyer::registerDeepLinkListener([](appsflyer::DeepLinkData) {})
        .whenComplete([&step](const AppsFlyerResult& r) {
            EXPECT(r.success);
            step = 1;
            appsflyer::InitParams params;
            params.devKey = "key";
            appsflyer::init(params).whenComplete([&step](const AppsFlyerResult& r2) {
                EXPECT(r2.success);
                step = 2;
            });
        });

    for (int i = 0; i < 800 && step.load() < 2; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    EXPECT(step.load() == 2);
    EXPECT(g_rpcCallOrder.size() >= 3);
    EXPECT(g_rpcCallOrder[0] == "subscribeForDeepLink");
    EXPECT(g_rpcCallOrder[1] == "setPluginInfo");
    EXPECT(g_rpcCallOrder[2] == "init");
}

static void test_deep_link_status_normalization() {
    EXPECT(appsflyer::deepLinkStatusFromWire("FOUND") == appsflyer::DeepLinkStatus::Found);
    EXPECT(appsflyer::deepLinkStatusFromWire("found") == appsflyer::DeepLinkStatus::Found);
    EXPECT(appsflyer::deepLinkStatusFromWire("NOT_FOUND") == appsflyer::DeepLinkStatus::NotFound);
    EXPECT(appsflyer::deepLinkStatusFromWire("notFound") == appsflyer::DeepLinkStatus::NotFound);
    EXPECT(appsflyer::deepLinkStatusFromWire("ERROR") == appsflyer::DeepLinkStatus::Error);
    EXPECT(appsflyer::deepLinkStatusFromWire("failure") == appsflyer::DeepLinkStatus::Error);
}

static void test_decode_conversion_event_payload() {
    const std::string event =
        R"({"event":"onConversionDataSuccess","data":{"af_status":"Non-organic","campaign":"c1"}})";
    const cocos2d::ValueMap map = appsflyer::detail::decodeJsonObjectToValueMap(
        appsflyer::detail::extractJsonObjectField(event, "data"));
    EXPECT(map.count("af_status") == 1);
    EXPECT(map.at("af_status").asString() == "Non-organic");
    EXPECT(map.at("campaign").asString() == "c1");
}

static void test_start_await_response_wires_param() {
    appsflyer::detail::resetTestHooks();
    g_rpcCallOrder.clear();
    appsflyer::detail::setGameThreadPosterForTests(syncGameThread);
    std::string capturedParams;
    appsflyer::detail::setRpcExecutorForTests(
        [&capturedParams](const std::string& method, const std::string& params) -> appsflyer::rpc::RpcResult {
            g_rpcCallOrder.push_back(method);
            if (method == "start") {
                capturedParams = params;
            }
            return successEnvelope();
        });

    std::atomic<bool> done{false};
    appsflyer::start(true, [&done](const AppsFlyerResult& r) {
        EXPECT(r.success);
        done = true;
    });
    for (int i = 0; i < 400 && !done.load(); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    EXPECT(done.load());
    EXPECT(capturedParams.find("awaitResponse") != std::string::npos);
    EXPECT(capturedParams.find("true") != std::string::npos);
}

static void test_log_ad_revenue_mediation_network_schema_value_map() {
    EXPECT(appsflyer::detail::schemaKey_MediationNetwork(appsflyer::MediationNetwork::ApplovinMax) ==
           "applovinMax");
    EXPECT(appsflyer::detail::resolveValueMap_logAdRevenue_mediationNetwork(
               appsflyer::detail::schemaKey_MediationNetwork(appsflyer::MediationNetwork::ApplovinMax)) ==
           "applovin_max");
    EXPECT(appsflyer::detail::resolveValueMap_logAdRevenue_mediationNetwork(
               appsflyer::detail::schemaKey_MediationNetwork(appsflyer::MediationNetwork::CustomMediation)) ==
           "custom_mediation");
    EXPECT(appsflyer::detail::resolveValueMap_logAdRevenue_mediationNetwork("unknown_wire") ==
           "unknown_wire");

    appsflyer::detail::resetTestHooks();
    appsflyer::detail::setGameThreadPosterForTests(syncGameThread);
    std::string capturedParams;
    appsflyer::detail::setRpcExecutorForTests(
        [&capturedParams](const std::string& method, const std::string& params) -> appsflyer::rpc::RpcResult {
            if (method == "logAdRevenue") {
                capturedParams = params;
            }
            return successEnvelope();
        });

    std::atomic<bool> done{false};
    appsflyer::LogAdRevenueParams revenue;
    revenue.monetizationNetwork = "qa";
    revenue.mediationNetwork = appsflyer::MediationNetwork::ApplovinMax;
    revenue.currencyIso4217Code = "USD";
    revenue.revenue = 1.5;
    appsflyer::logAdRevenue(revenue, [&done](const AppsFlyerResult& r) {
        EXPECT(r.success);
        done = true;
    });
    for (int i = 0; i < 400 && !done.load(); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    EXPECT(done.load());
    EXPECT(capturedParams.find("\"mediationNetwork\":\"applovin_max\"") != std::string::npos);
}

static void test_set_log_level_android_wire_transform() {
    EXPECT(appsflyer::detail::resolveTransform_androidLogLevel("debug") == "DEBUG");
    EXPECT(appsflyer::detail::resolveTransform_androidLogLevel("none") == "NONE");

    appsflyer::detail::resetTestHooks();
    appsflyer::detail::setGameThreadPosterForTests(syncGameThread);
    std::string capturedParams;
    appsflyer::detail::setRpcExecutorForTests(
        [&capturedParams](const std::string& method, const std::string& params) -> appsflyer::rpc::RpcResult {
            if (method == "setLogLevel") {
                capturedParams = params;
            }
            return successEnvelope();
        });

    std::atomic<bool> done{false};
    appsflyer::setLogLevel(appsflyer::LogLevel::Debug, [&done](const AppsFlyerResult& r) {
        EXPECT(r.success);
        done = true;
    });
    for (int i = 0; i < 400 && !done.load(); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    EXPECT(done.load());
    EXPECT(capturedParams.find("\"logLevel\":\"DEBUG\"") != std::string::npos);
}

static void test_validate_purchase_android_wire_transform() {
    EXPECT(appsflyer::detail::resolveTransform_androidPurchaseType("oneTimePurchase") == "one_time_purchase");
    EXPECT(appsflyer::detail::resolveTransform_androidPurchaseType("subscription") == "subscription");

    appsflyer::detail::resetTestHooks();
    appsflyer::detail::setGameThreadPosterForTests(syncGameThread);
    std::string capturedParams;
    appsflyer::detail::setRpcExecutorForTests(
        [&capturedParams](const std::string& method, const std::string& params) -> appsflyer::rpc::RpcResult {
            if (method == "validateAndLogInAppPurchase") {
                capturedParams = params;
            }
            return successEnvelope();
        });

    std::atomic<bool> done{false};
    appsflyer::ValidateAndLogInAppPurchaseParams params;
    params.purchase.purchaseType = appsflyer::PurchaseType::OneTimePurchase;
    params.purchase.productId = "sku_qa";
    params.purchase.purchaseToken = "token_qa";
    appsflyer::validateAndLogInAppPurchase(params, [&done](const AppsFlyerResult& r) {
        EXPECT(r.success);
        done = true;
    });
    for (int i = 0; i < 400 && !done.load(); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    EXPECT(done.load());
    EXPECT(capturedParams.find("\"purchaseType\":\"one_time_purchase\"") != std::string::npos);
    EXPECT(capturedParams.find("\"productId\":\"sku_qa\"") != std::string::npos);
    EXPECT(capturedParams.find("\"purchaseToken\":\"token_qa\"") != std::string::npos);
}

static void test_set_plugin_info_params_match_metadata() {
    appsflyer::detail::resetTestHooks();
    g_rpcCallOrder.clear();
    appsflyer::detail::setGameThreadPosterForTests(syncGameThread);
    std::string capturedPluginInfo;
    appsflyer::detail::setRpcExecutorForTests(
        [&capturedPluginInfo](const std::string& method, const std::string& params) -> appsflyer::rpc::RpcResult {
            g_rpcCallOrder.push_back(method);
            if (method == "setPluginInfo") {
                capturedPluginInfo = params;
            }
            return successEnvelope();
        });

    std::atomic<bool> done{false};
    appsflyer::InitParams params;
    params.devKey = "key";
    appsflyer::init(params, [&done](const AppsFlyerResult& r) {
        EXPECT(r.success);
        done = true;
    });
    for (int i = 0; i < 400 && !done.load(); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    EXPECT(done.load());
    const std::string pluginNeedle =
        std::string("\"plugin\":\"") + appsflyer::kAppsFlyerPluginId + "\"";
    const std::string versionNeedle = std::string("\"pluginVersion\":\"") +
                                      appsflyer::kAppsFlyerPluginVersion + "\"";
    EXPECT(capturedPluginInfo.find(pluginNeedle) != std::string::npos);
    EXPECT(capturedPluginInfo.find(versionNeedle) != std::string::npos);
}

int main() {
    test_task_completes_on_game_thread();
    test_deep_link_register_rollback_on_failure();
    test_chain_order_deep_link_before_init();
    test_deep_link_status_normalization();
    test_set_plugin_info_params_match_metadata();
    test_decode_conversion_event_payload();
    test_start_await_response_wires_param();
    test_log_ad_revenue_mediation_network_schema_value_map();
    test_set_log_level_android_wire_transform();
    test_validate_purchase_android_wire_transform();
    std::printf("All AppsFlyer task tests passed.\n");
    return 0;
}
