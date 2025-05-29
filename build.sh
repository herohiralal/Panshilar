#!/bin/bash
set -euo pipefail

# Set up toolchains ================================================================

OSX_TOOLCHAIN_FOUND=0
IOS_TOOLCHAIN_FOUND=0

clang -target arm64-apple-ios14.0 -v 2>/dev/null | grep "Apple clang version" >/dev/null && IOS_TOOLCHAIN_FOUND=1
clang -target arm64-apple-macos11 -v 2>/dev/null | grep "Apple clang version" >/dev/null && OSX_TOOLCHAIN_FOUND=1

if [ $IOS_TOOLCHAIN_FOUND -eq 0 ]; then
    echo "WARNING: iOS-ARM64 toolchain not found!"
else
    echo "INFO: iOS-ARM64 toolchain found!"
fi

if [ $OSX_TOOLCHAIN_FOUND -eq 0 ]; then
    echo "WARNING: OSX-ARM64 toolchain not found!"
else
    echo "INFO: OSX-ARM64 toolchain found!"
fi

# Exit if nothing found

if [ $IOS_TOOLCHAIN_FOUND -eq 0 ] && [ $OSX_TOOLCHAIN_FOUND -eq 0 ]; then
    echo "ERROR: No valid toolchains found for iOS-ARM64 or OSX-ARM64!"
    exit 1
fi

# Build ============================================================================

FAILED_BUILDS=()

# iOS-ARM64
if [ $IOS_TOOLCHAIN_FOUND -eq 1 ]; then
    clang -c -std=c11 -target arm64-apple-ios14.0 -DPNSLR_IMPLEMENTATION -DPNSLR_IOS=1 -DPNSLR_ARM64 Source/zzzz_Unity.c -o Temp/unity-ios-arm64.o
    if [ $? -ne 0 ]; then
        echo "ERROR: iOS-ARM64 compilation failed!"
        FAILED_BUILDS+=("iOS-ARM64")
    else
        ar rcs Libraries/panshilar-ios-arm64.a Temp/unity-ios-arm64.o Source/Dependencies/PNSLR_Intrinsics/Prebuilt/intrinsics-ios-arm64.o
        if [ $? -ne 0 ]; then
            echo "ERROR: iOS-ARM64 library creation failed!"
            FAILED_BUILDS+=("iOS-ARM64")
        fi
        echo "SUCCESS: Static library panshilar-ios-arm64.a created successfully!"
    fi
fi

# OSX-ARM64
if [ $OSX_TOOLCHAIN_FOUND -eq 1 ]; then
    clang -c -std=c11 -target arm64-apple-macos11 -DPNSLR_IMPLEMENTATION -DPNSLR_OSX=1 -DPNSLR_ARM64 Source/zzzz_Unity.c -o Temp/unity-osx-arm64.o
    if [ $? -ne 0 ]; then
        echo "ERROR: OSX-ARM64 compilation failed!"
        FAILED_BUILDS+=("OSX-ARM64")
    else
        ar rcs Libraries/panshilar-osx-arm64.a Temp/unity-osx-arm64.o Source/Dependencies/PNSLR_Intrinsics/Prebuilt/intrinsics-osx-arm64.o
        if [ $? -ne 0 ]; then
            echo "ERROR: OSX-ARM64 library creation failed!"
            FAILED_BUILDS+=("OSX-ARM64")
        fi
        echo "SUCCESS: Static library panshilar-osx-arm64.a created successfully!"
    fi
fi

# Summary ==========================================================================

if [ ${#FAILED_BUILDS[@]} -ne 0 ]; then
    echo "ERROR: The following builds failed: ${FAILED_BUILDS[*]}"
    exit 1
else
    echo "INFO: All builds completed successfully!"
    exit 0
fi
