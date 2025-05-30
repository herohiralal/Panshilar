#!/bin/bash

set -euo pipefail

# Gather toolchains ================================================================

export IOS_SDK_FOUND=0
export IOS_SIM_SDK_FOUND=0
export OSX_SDK_FOUND=0

export BUILD_TOOLS_PATH="/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain"
BUILD_PLATFORMS_PATH="/Applications/Xcode.app/Contents/Developer/Platforms"

if [ ! -d $BUILD_TOOLS_PATH/usr/bin/clang ]; then
    echo "ERROR: Build tools not found at $BUILD_TOOLS_PATH"
    exit 1
fi

export IOS_SDK_PATH="$BUILD_PLATFORMS_PATH/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk"
if [ -e $IOS_SDK_PATH ]; then
    IOS_SDK_FOUND=1
fi

export IOS_SIM_SDK_PATH="$BUILD_PLATFORMS_PATH/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk"
if [ -e $IOS_SIM_SDK_PATH ]; then
    IOS_SIM_SDK_FOUND=1
fi

export OSX_SDK_PATH="$BUILD_PLATFORMS_PATH/MacOSX.platform/Developer/SDKs/MacOSX.sdk"
if [ -e $OSX_SDK_PATH ]; then
    OSX_SDK_FOUND=1
fi

# Run Build Script =================================================================

python build.py "$@"
