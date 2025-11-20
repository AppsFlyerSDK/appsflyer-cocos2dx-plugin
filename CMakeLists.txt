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

project(${APP_NAME})
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
     Classes/AppsFlyer/AppsFlyerXApple.mm
     Classes/AppsFlyer/AppsFlyerXAppleDeepLinkDelegate.mm
     Classes/AppsFlyer/AppsFlyerXAppleDelegate.mm
     Classes/AppsFlyer/AppsFlyerXAppleHelper.mm
     Classes/AppsFlyer/AppsFlyerXConsent.cpp
     Classes/AppsFlyer/AppsFlyerXDeepLinkResult.cpp
)

list(APPEND GAME_HEADER
     Classes/AppDelegate.h
     Classes/HelloWorldScene.h
     Classes/AppsFlyer/AFSDKXPurchaseDetails.h
     Classes/AppsFlyer/AFSDKXValidateAndLogResult.h
     Classes/AppsFlyer/AFXAdRevenueData.h
     Classes/AppsFlyer/AppsFlyerX.h
     Classes/AppsFlyer/AppsFlyerXApple.h
     Classes/AppsFlyer/AppsFlyerXAppleDeepLinkDelegate.h
     Classes/AppsFlyer/AppsFlyerXAppleDelegate.h
     Classes/AppsFlyer/AppsFlyerXAppleHelper.h
     Classes/AppsFlyer/AppsFlyerXDeepLinkResult.h
     Classes/AppsFlyer/AppsFlyerXMacro.h
     Classes/AppsFlyer/EmailCryptTypeX.h
)

if(ANDROID)
    list(APPEND GAME_SOURCE
    Classes/AppsFlyer/AppsFlyerXAndroid.cpp
    Classes/AppsFlyer/AppsFlyerProxyX.cpp
    )
    
    list(APPEND GAME_HEADER
    Classes/AppsFlyer/AppsFlyerXAndroid.h
    Classes/AppsFlyer/AppsFlyerProxyX.h
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
        PRIVATE ${COCOS2DX_ROOT_PATH}/cocos/audio/include/
)
if(APPLE AND IOS)
    string(FIND "${CMAKE_OSX_SYSROOT}" "iPhoneSimulator" _is_simulator_sysroot)
    if(_is_simulator_sysroot GREATER -1)
        set(_swift_platform_folder "iphonesimulator")
    else()
        set(_swift_platform_folder "iphoneos")
    endif()

    if(NOT DEFINED APPLE_DEVELOPER_ROOT)
        execute_process(COMMAND xcode-select -p OUTPUT_VARIABLE APPLE_DEVELOPER_ROOT OUTPUT_STRIP_TRAILING_WHITESPACE)
    endif()
    set(_swift_lib_root "${APPLE_DEVELOPER_ROOT}/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/${_swift_platform_folder}")

    foreach(_swift_lib swiftCompatibility50 swiftCompatibility51 swiftCompatibilityConcurrency swiftCompatibilityDynamicReplacements)
        string(TOUPPER "${_swift_lib}" _swift_lib_upper)
        set(_swift_var "SWIFT_LIB_${_swift_lib_upper}")
        find_library(${_swift_var} ${_swift_lib} HINTS "${_swift_lib_root}" NO_DEFAULT_PATH)
        if(${_swift_var})
            target_link_libraries(${APP_NAME} "${${_swift_var}}")
        else()
            message(WARNING "Could not locate ${_swift_lib} inside ${_swift_lib_root}")
        endif()
    endforeach()
endif()

# mark app resources
setup_cocos_app_config(${APP_NAME})
if(APPLE)
    if(IOS)
        # Force DerivedData into the build tree so sandboxed environments can write to it
        set(CMAKE_XCODE_ATTRIBUTE_DERIVED_DATA_PATH "${PROJECT_BINARY_DIR}/DerivedData")
    endif()
    set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")
    if(MACOSX)
        set_target_properties(${APP_NAME} PROPERTIES
                              MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/mac/Info.plist"
                              )
    elseif(IOS)
        #cocos_pak_xcode(${APP_NAME} INFO_PLIST "iOSBundleInfo.plist.in")
        # set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
        set_xcode_property(${APP_NAME} DEVELOPMENT_TEAM "")
        set_xcode_property(${APP_NAME} CODE_SIGN_IDENTITY "iPhone Developer")
        set_xcode_property(${APP_NAME} PRODUCT_BUNDLE_IDENTIFIER "com.appsflyer.AppsFlyerCocos2dX")
    endif()
elseif(WINDOWS)
    cocos_copy_target_dll(${APP_NAME})
endif()

if(LINUX OR WINDOWS)
    set(APP_RES_DIR "$<TARGET_FILE_DIR:${APP_NAME}>/Resources")
    cocos_copy_target_res(${APP_NAME} COPY_TO ${APP_RES_DIR} FOLDERS ${GAME_RES_FOLDER})
endif()
