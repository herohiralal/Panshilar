#!/bin/bash
set -euo pipefail

# Gather toolchains ================================================================

TOOLS_FOUND=false
TOOLS_PATH="/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr"

PLATFORMS_PATH="/Applications/Xcode.app/Contents/Developer/Platforms"
IOS_SDK_PATH="$PLATFORMS_PATH/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk"
IOS_SDK_FOUND=false
IOS_SIM_SDK_PATH="$PLATFORMS_PATH/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk"
IOS_SIM_SDK_FOUND=false
OSX_SDK_PATH="$PLATFORMS_PATH/MacOSX.platform/Developer/SDKs/MacOSX.sdk"
OSX_SDK_FOUND=false

if [ -e $TOOLS_PATH/bin/clang ]; then
    TOOLS_FOUND=true
fi

if [ -e $IOS_SDK_PATH ]; then
    IOS_SDK_FOUND=true
    echo Info: iOS SDK found at $IOS_SDK_PATH!
else
    echo Error: iOS SDK not found!
fi

if [ -e $IOS_SIM_SDK_PATH ]; then
    IOS_SIM_SDK_FOUND=true
    echo Info: iOS Simulator SDK found at $IOS_SIM_SDK_PATH!
else
    echo Error: iOS Simulator SDK not found!
fi

if [ -e $OSX_SDK_PATH ]; then
    OSX_SDK_FOUND=true
    echo Info: macOS SDK found at $OSX_SDK_PATH!
else
    echo Error: macOS SDK not found!
fi

# Set up symlinks for toolchains ===================================================

# delete all symlinks inside ./Toolchains except .gitkeep file
if [ -d ./Toolchains ]; then
    echo Info: Removing old symlinks in ./Toolchains
    find ./Toolchains -mindepth 1 -maxdepth 1 ! -name '.gitkeep' -exec rm -f {} +
fi

FAILED_SYMLINKS=""

if [ "$TOOLS_FOUND" = true ]; then
    ln -s "$TOOLS_PATH" ./Toolchains/BuildTools 2>/dev/null || FAILED_SYMLINKS="$FAILED_SYMLINKS BuildTools"
fi

if [ "$IOS_SDK_FOUND" = true ]; then
    ln -s "$IOS_SDK_PATH" ./Toolchains/iOS 2>/dev/null || FAILED_SYMLINKS="$FAILED_SYMLINKS iOS"
fi

if [ "$IOS_SIM_SDK_FOUND" = true ]; then
    ln -s "$IOS_SIM_SDK_PATH" ./Toolchains/iOSSimulator 2>/dev/null || FAILED_SYMLINKS="$FAILED_SYMLINKS iOSSimulator"
fi

if [ "$OSX_SDK_FOUND" = true ]; then
    ln -s "$OSX_SDK_PATH" ./Toolchains/OSX 2>/dev/null || FAILED_SYMLINKS="$FAILED_SYMLINKS OSX"
fi

# Summary ==========================================================================

if [ -n "$FAILED_SYMLINKS" ]; then
    echo "ERROR: Failed to create the following symlinks:$FAILED_SYMLINKS"
    exit 1
else
    echo "SUCCESS: All symlinks created successfully!"
    exit 0
fi
