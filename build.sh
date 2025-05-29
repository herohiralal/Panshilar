#!/bin/bash
set -euo pipefail

# Build ============================================================================

if [ -e ./Toolchains/BuildTools ]; then
    echo "INFO: BuildTools found."
else
    echo "ERROR: Toolchains/BuildTools directory not found!"
    echo "Please run the setup script first."
    exit 1
fi

FAILED_PLATFORMS=""

if [ -e ./Toolchains/iOS ]; then
    ./Toolchains/BuildTools/bin/clang -c -std=c11 -DPNSLR_IMPLEMENTATION -DPNSLR_IOS=1 -DPNSLR_ARM64=1 Source/zzzz_Unity.c -o Temp/unity-ios-arm64.o -miphoneos-version-min=16.0 -target arm64-apple-ios16.0 -arch arm64 --sysroot Toolchains/iOS/
    if [ $? -ne 0 ]; then
        echo "ERROR: iOS-ARM64 compilation failed!"
        FAILED_PLATFORMS="$FAILED_PLATFORMS iOS-ARM64"
    else
        ar rcs Libraries/panshilar-ios-arm64.a Temp/unity-ios-arm64.o Source/Dependencies/PNSLR_Intrinsics/Prebuilt/intrinsics-ios-arm64.o
        if [ $? -ne 0 ]; then
            echo "ERROR: iOS-ARM64 library creation failed!"
            FAILED_PLATFORMS="$FAILED_PLATFORMS iOS-ARM64"
        else
            echo "SUCCESS: Static library panshilar-ios-arm64.a created successfully!"
        fi
    fi
fi

if [ -e ./Toolchains/iOSSimulator ]; then
    ./Toolchains/BuildTools/bin/clang -c -std=c11 -DPNSLR_IMPLEMENTATION -DPNSLR_IOS=1 -DPNSLR_ARM64=1 Source/zzzz_Unity.c -o Temp/unity-iossimulator-arm64.o -miphoneos-version-min=16.0 -target arm64-apple-ios16.0-simulator --sysroot Toolchains/iOSSimulator/
    if [ $? -ne 0 ]; then
        echo "ERROR: iOSSimulator-ARM64 library creation failed!"
        FAILED_PLATFORMS="$FAILED_PLATFORMS iOSSimulator-ARM64"
    else
        ar rcs Libraries/panshilar-iossimulator-arm64.a Temp/unity-iossimulator-arm64.o Source/Dependencies/PNSLR_Intrinsics/Prebuilt/intrinsics-iossimulator-arm64.o
        if [ $? -ne 0 ]; then
            echo "ERROR: iOSSimulator-ARM64 library creation failed!"
            FAILED_PLATFORMS="$FAILED_PLATFORMS iOSSimulator-ARM64"
        else
            echo "SUCCESS: Static library panshilar-iossimulator-arm64.a created successfully!"
        fi
    fi
fi

if [ -e ./Toolchains/OSX ]; then
    ./Toolchains/BuildTools/bin/clang -c -std=c11 -DPNSLR_IMPLEMENTATION -DPNSLR_OSX=1 -DPNSLR_ARM64=1 Source/zzzz_Unity.c -o Temp/unity-osx-arm64.o -target arm64-apple-macos11.0 --sysroot Toolchains/OSX/
    if [ $? -ne 0 ]; then
        echo "ERROR: OSX-ARM64 compilation failed!"
        FAILED_PLATFORMS="$FAILED_PLATFORMS OSX-ARM64"
    else
        ar rcs Libraries/panshilar-osx-arm64.a Temp/unity-osx-arm64.o Source/Dependencies/PNSLR_Intrinsics/Prebuilt/intrinsics-osx-arm64.o
        if [ $? -ne 0 ]; then
            echo "ERROR: OSX-ARM64 library creation failed!"
            FAILED_PLATFORMS="$FAILED_PLATFORMS OSX-ARM64"
        else
            echo "SUCCESS: Static library panshilar-osx-arm64.a created successfully!"
        fi
    fi
fi

# Summary ==========================================================================

if [ -n "$FAILED_PLATFORMS" ]; then
    echo "ERROR: The following builds failed: $FAILED_PLATFORMS"
    exit 1
else
    echo "INFO: All builds completed successfully!"
    exit 0
fi
