#/****************************************************************************
# Copyright (c) 2013-2014 cocos2d-x.org
# Copyright (c) 2015-2017 Chukong Technologies Inc.
#
# http://www.cocos2d-x.org
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# ****************************************************************************/

cmake_minimum_required(VERSION 3.6)

set(APP_NAME AppsFlyerCocos2dX)

project(${APP_NAME} LANGUAGES CXX C)
set(CMAKE_XCODE_GENERATE_SCHEME TRUE)

set(COCOS2DX_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cocos2d)
set(CMAKE_MODULE_PATH ${COCOS2DX_ROOT_PATH}/cmake/Modules/)

# Print the values
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "COCOS2DX_ROOT_PATH: ${COCOS2DX_ROOT_PATH}")
message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
message(STATUS "COCOS2DX_ROOT_PATH: ${COCOS2DX_ROOT_PATH}")
message(STATUS "Is ANDROID?: ${ANDROID}")

include(CocosBuildSet)
add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos ${ENGINE_BINARY_PATH}/cocos/core)

# record sources, headers, resources...
set(GAME_SOURCE)
set(GAME_HEADER)

set(GAME_RES_FOLDER
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
    )
if(APPLE OR WINDOWS)
    cocos_mark_multi_resources(common_res_files RES_TO "Resources" FOLDERS ${GAME_RES_FOLDER})
endif()

# add cross-platforms source files and header files
list(APPEND GAME_SOURCE
     Classes/AppDelegate.cpp
     Classes/HelloWorldScene.cpp
     Classes/AppsFlyer/AFSDKXPurchaseDetails.cpp
     Classes/AppsFlyer/AFSDKXValidateAndLogResult.cpp
     Classes/AppsFlyer/AFXAdRevenueData.cpp
     Classes/AppsFlyer/AppsFlyerX.cpp
     Classes/AppsFlyer/AppsFlyerXConsent.cpp
     Classes/AppsFlyer/AppsFlyerXDeepLinkResult.cpp
     Classes/AppsFlyer/rpc/AppsFlyerCocosThread.cpp
     Classes/AppsFlyer/rpc/AppsFlyerRpcClient.cpp
     Classes/AppsFlyer/AppsFlyerXCore.cpp
)

list(APPEND GAME_HEADER
     Classes/AppDelegate.h
     Classes/HelloWorldScene.h
     Classes/AppsFlyer/AFSDKXPurchaseDetails.h
     Classes/AppsFlyer/AFSDKXValidateAndLogResult.h
     Classes/AppsFlyer/AFXAdRevenueData.h
     Classes/AppsFlyer/AppsFlyerX.h
     Classes/AppsFlyer/AppsFlyerXCore.h
     Classes/AppsFlyer/AppsFlyerXDeepLinkResult.h
     Classes/AppsFlyer/AppsFlyerXMacro.h
     Classes/AppsFlyer/EmailCryptTypeX.h
)

if(ANDROID)
    list(APPEND GAME_HEADER
        Classes/AppsFlyer/AppsFlyerXAndroid.h
    )

    # change APP_NAME to the share library name for Android, it's value depend on AndroidManifest.xml
    set(APP_NAME AppsFlyerCocos2dX)
    list(APPEND GAME_SOURCE
         proj.android-studio/app/jni/hellocpp/main.cpp
         )
elseif(LINUX)
    list(APPEND GAME_SOURCE
         proj.linux/main.cpp
         )
elseif(WINDOWS)
    list(APPEND GAME_HEADER
         proj.win32/main.h
         proj.win32/resource.h
         )
    list(APPEND GAME_SOURCE
         proj.win32/main.cpp
         proj.win32/game.rc
         ${common_res_files}
         )
elseif(APPLE)
    if(IOS)
        list(APPEND GAME_SOURCE
             proj.ios_mac/ios/rpc/AppsFlyerCocosRpcBridge.mm
             proj.ios_mac/ios/rpc/AFCocosRPCBridge.swift
             proj.ios_mac/ios/rpc/AppsFlyerCocosEventBus.swift
             proj.ios_mac/ios/rpc/AppsFlyerCocosRpcResponse.swift
             proj.ios_mac/ios/rpc/AppsFlyerCocosRpcBridge.swift
             )
        list(APPEND GAME_HEADER
             Classes/AppsFlyer/AppsFlyerXApple.h
             proj.ios_mac/ios/rpc/AppsFlyerCocosRpcBridge.h
             )
    else()
        list(APPEND GAME_SOURCE
             Classes/AppsFlyer/AppsFlyerXApple.mm
             Classes/AppsFlyer/AppsFlyerXAppleDeepLinkDelegate.mm
             Classes/AppsFlyer/AppsFlyerXAppleDelegate.mm
             Classes/AppsFlyer/AppsFlyerXAppleHelper.mm
             )
        list(APPEND GAME_HEADER
             Classes/AppsFlyer/AppsFlyerXApple.h
             Classes/AppsFlyer/AppsFlyerXAppleDeepLinkDelegate.h
             Classes/AppsFlyer/AppsFlyerXAppleDelegate.h
             Classes/AppsFlyer/AppsFlyerXAppleHelper.h
             )
    endif()
    if(IOS)
        list(APPEND GAME_HEADER
             proj.ios_mac/ios/AppController.h
             proj.ios_mac/ios/RootViewController.h
             )

        set(APP_UI_RES
            proj.ios_mac/ios/LaunchScreen.storyboard
            proj.ios_mac/ios/LaunchScreenBackground.png
            proj.ios_mac/ios/Images.xcassets
            )
            
        list(APPEND GAME_SOURCE
             proj.ios_mac/ios/main.m
             proj.ios_mac/ios/AppController.mm
             proj.ios_mac/ios/RootViewController.mm
             proj.ios_mac/ios/Prefix.pch
             proj.ios_mac/ios/SwiftBridge.swift
             proj.ios_mac/ios/SwiftCompatibilityStubs.m
             ${APP_UI_RES}
             )
    elseif(MACOSX)
        set(APP_UI_RES
            proj.ios_mac/mac/Icon.icns
            proj.ios_mac/mac/Info.plist
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/mac/main.cpp
             proj.ios_mac/mac/Prefix.pch
             ${APP_UI_RES}
             )
    endif()
    list(APPEND GAME_SOURCE ${common_res_files})
endif()

# mark app complie info and libs info
set(all_code_files
    ${GAME_HEADER}
    ${GAME_SOURCE}
    )
if(APPLE AND IOS)
    enable_language(Swift)
    set(CMAKE_Swift_LANGUAGE_VERSION 5.9)
    set(_ios_swift_rpc_sources
        proj.ios_mac/ios/rpc/AFCocosRPCBridge.swift
        proj.ios_mac/ios/rpc/AppsFlyerCocosEventBus.swift
        proj.ios_mac/ios/rpc/AppsFlyerCocosRpcResponse.swift
        proj.ios_mac/ios/rpc/AppsFlyerCocosRpcBridge.swift
        proj.ios_mac/ios/SwiftBridge.swift
    )
    set_source_files_properties(${_ios_swift_rpc_sources} PROPERTIES LANGUAGE Swift)
endif()

if(NOT ANDROID)
    add_executable(${APP_NAME} ${all_code_files})
else()
    add_library(${APP_NAME} SHARED ${all_code_files})
    add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos/platform/android ${ENGINE_BINARY_PATH}/cocos/platform)
    target_link_libraries(${APP_NAME} -Wl,--whole-archive cpp_android_spec -Wl,--no-whole-archive)
endif()

target_link_libraries(${APP_NAME} cocos2d)
target_include_directories(${APP_NAME}
        PRIVATE Classes
        PRIVATE Classes/AppsFlyer/libAppsFlyer
        PRIVATE ${COCOS2DX_ROOT_PATH}/cocos/audio/include/
)
if(APPLE AND IOS)
    target_include_directories(${APP_NAME} PRIVATE proj.ios_mac/ios/rpc)
endif()

# iOS SDK 7 RPC (AppsFlyerRPC via CocoaPods — run scripts/setup-ios-rpc-pods.sh after cmake)
if(APPLE AND IOS)
    target_link_libraries(${APP_NAME}
        "-framework AdSupport"
        "-framework AdServices"
        "-framework StoreKit"
    )

    set_target_properties(${APP_NAME} PROPERTIES
        XCODE_ATTRIBUTE_ALWAYS_EMBED_SWIFT_STANDARD_LIBRARIES "$(inherited)"
        XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "$(inherited) @executable_path/Frameworks"
        XCODE_ATTRIBUTE_SWIFT_VERSION "5.9"
        XCODE_ATTRIBUTE_PRODUCT_MODULE_NAME "${APP_NAME}"
        XCODE_ATTRIBUTE_SWIFT_OBJC_INTERFACE_HEADER_NAME "${APP_NAME}-Swift.h"
        XCODE_ATTRIBUTE_SWIFT_OBJC_BRIDGING_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/ios/AppsFlyerCocos2dX-Bridging-Header.h"
        XCODE_ATTRIBUTE_OTHER_LDFLAGS "$(inherited) -ObjC"
    )

    # CocoaPods sets baseConfigurationReference on `pod install`; do not override via CMake.
endif()

# mark app resources
setup_cocos_app_config(${APP_NAME})
if(APPLE)
    # Do not set DERIVED_DATA_PATH on iOS when using CocoaPods — it splits Pod
    # framework output from the app linker search paths (Pods_AppsFlyerCocos2dX not found).
    set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")
    if(MACOSX)
        set_target_properties(${APP_NAME} PROPERTIES
                              MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/mac/Info.plist"
                              )
    elseif(IOS)
        #cocos_pak_xcode(${APP_NAME} INFO_PLIST "iOSBundleInfo.plist.in")
        # set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
        # Set signing configuration
        set_xcode_property(${APP_NAME} CODE_SIGN_STYLE "Automatic")
        set_xcode_property(${APP_NAME} DEVELOPMENT_TEAM "")
        set_xcode_property(${APP_NAME} CODE_SIGN_IDENTITY "Apple Development")
        set_xcode_property(${APP_NAME} PRODUCT_BUNDLE_IDENTIFIER "com.appsflyer.AppsFlyerCocos2dX")
        # libAppsFlyerLib.a: arm64 = device, x86_64 = simulator — on Apple Silicon simulators use x86_64 slice.
        set_xcode_property(${APP_NAME} "EXCLUDED_ARCHS[sdk=iphonesimulator*]" "arm64")

        # Info.plist fields
        set_target_properties(${APP_NAME} PROPERTIES
            MACOSX_BUNDLE_BUNDLE_NAME "AppsFlyerCocos2dX"
            MACOSX_BUNDLE_GUI_IDENTIFIER "com.appsflyer.AppsFlyerCocos2dX"
        )
        set_xcode_property(${APP_NAME} INFOPLIST_KEY_CFBundleDisplayName "AppsFlyer Cocos2dX")
    endif()
elseif(WINDOWS)
    cocos_copy_target_dll(${APP_NAME})
endif()

if(LINUX OR WINDOWS)
    set(APP_RES_DIR "$<TARGET_FILE_DIR:${APP_NAME}>/Resources")
    cocos_copy_target_res(${APP_NAME} COPY_TO ${APP_RES_DIR} FOLDERS ${GAME_RES_FOLDER})
endif()
