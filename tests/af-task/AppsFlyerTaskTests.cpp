#include "AppsFlyer/rpc/AppsFlyerRpcClient.h"

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
    appsflyer::setDeepLinkCallback([](AppsFlyerXDeepLinkResult) { /* prior */ });

    std::atomic<bool> completed{false};
    appsflyer::registerDeepLinkListener(
        [&firstCallback](AppsFlyerXDeepLinkResult) { firstCallback = true; },
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
    appsflyer::registerDeepLinkListener([](AppsFlyerXDeepLinkResult) {})
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

int main() {
    test_task_completes_on_game_thread();
    test_deep_link_register_rollback_on_failure();
    test_chain_order_deep_link_before_init();
    std::printf("All AppsFlyer task tests passed.\n");
    return 0;
}
