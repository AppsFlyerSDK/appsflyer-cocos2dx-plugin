cmake_minimum_required(VERSION 3.6)
project(appsflyer_task_tests LANGUAGES CXX)

set(AF_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../Classes/AppsFlyer)
set(STUB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/stubs)

add_executable(appsflyer_task_tests
    AppsFlyerTaskTests.cpp
    stubs/RpcTransportStubs.cpp
    ${AF_ROOT}/AppsFlyerTask.cpp
    ${AF_ROOT}/rpc/AppsFlyerRpcClient.cpp
    ${AF_ROOT}/rpc/AppsFlyerRpcJson.cpp
    ${AF_ROOT}/generated/AppsFlyerRpcSchema.generated.cpp
)

target_compile_definitions(appsflyer_task_tests PRIVATE
    CC_TARGET_PLATFORM=CC_PLATFORM_ANDROID
    AF_RPC_HEADLESS_TEST=1
)
target_include_directories(appsflyer_task_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Classes
    ${STUB_ROOT}
)

target_compile_options(appsflyer_task_tests PRIVATE -include ${STUB_ROOT}/cocos2d.h)
