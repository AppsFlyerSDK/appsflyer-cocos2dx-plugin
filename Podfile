# SDK 7 RPC: do NOT run `pod install` from repo root or proj.ios_mac.
# CMake generates AppsFlyerCocos2dX.xcodeproj under your iOS build dir; this file is copied there by:
#   ./scripts/setup-ios-rpc-pods.sh build-device
#   ./scripts/setup-ios-rpc-pods.sh build-ios-rpc
# Open <build-dir>/AppsFlyerCocos2dX.xcworkspace in Xcode.

unless File.directory?('AppsFlyerCocos2dX.xcodeproj')
  raise <<~MSG

    AppsFlyerCocos2dX.xcodeproj not found in #{Dir.pwd}.

    Generate an iOS project with CMake, then install pods via the setup script (from repo root), e.g.:

      cmake -B build-device -GXcode \\
        -DCMAKE_SYSTEM_NAME=iOS \\
        -DCMAKE_OSX_SYSROOT=iphoneos \\
        -DCMAKE_OSX_DEPLOYMENT_TARGET=15.6
      ./scripts/setup-ios-rpc-pods.sh build-device

    Legacy proj.ios_mac/AppsFlyerCocos2dX.xcodeproj is not wired to this Podfile.

  MSG
end

pbxproj = 'AppsFlyerCocos2dX.xcodeproj/project.pbxproj'
if File.exist?(pbxproj) && File.read(pbxproj).include?('SDKROOT = macosx')
  raise <<~MSG

    This Podfile is for iOS SDK 7 RPC only. macOS desktop is not supported in SDK 7.

    Configure CMake with -DCMAKE_SYSTEM_NAME=iOS (see scripts/build-ios-cmake.sh).

  MSG
end

platform :ios, '13.0'

project 'AppsFlyerCocos2dX.xcodeproj'

target 'AppsFlyerCocos2dX' do
  use_frameworks! :linkage => :static
  pod 'AppsFlyerRPC/Main', '7.0.41'
  pod 'AppsFlyerFramework/Main', '7.0.4'
end

# CMake sets a custom CONFIGURATION_BUILD_DIR for the app; Pods still emit
# Pods_AppsFlyerCocos2dX.framework under $(PODS_CONFIGURATION_BUILD_DIR).
post_install do |installer|
  installer.pods_project.targets.each do |target|
    target.build_configurations.each do |config|
      config.build_settings['IPHONEOS_DEPLOYMENT_TARGET'] = '13.0'
    end
  end

  installer.aggregate_targets.each do |aggregate_target|
    aggregate_target.xcconfigs.each do |_config_name, xcconfig|
      next if xcconfig.attributes['FRAMEWORK_SEARCH_PATHS'].include?('$(PODS_CONFIGURATION_BUILD_DIR)')

      xcconfig.attributes['FRAMEWORK_SEARCH_PATHS'] = [
        xcconfig.attributes['FRAMEWORK_SEARCH_PATHS'],
        '$(PODS_CONFIGURATION_BUILD_DIR)',
      ].join(' ')
    end

    project = aggregate_target.user_project
    app_target = project.native_targets.find { |t| t.name == 'AppsFlyerCocos2dX' }
    next unless app_target

    app_target.build_configurations.each do |config|
      paths = config.build_settings['FRAMEWORK_SEARCH_PATHS'] || '$(inherited)'
      paths = [paths] if paths.is_a?(String)
      %w[
        $(PODS_CONFIGURATION_BUILD_DIR)
        $(BUILT_PRODUCTS_DIR)
      ].each do |entry|
        paths << entry unless paths.include?(entry)
      end
      config.build_settings['FRAMEWORK_SEARCH_PATHS'] = paths
    end

    project.save
  end
end
