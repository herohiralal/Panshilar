#!/bin/bash
set -euo pipefail

echo ""
echo ""
echo "Building PNSLR_Intrinsics-iOS-ARM64"
../../../Toolchains/BuildTools/bin/clang -c -std=c11 ./Intrinsics.c -o Prebuilt/intrinsics-ios-arm64.o -miphoneos-version-min=16.0 -target arm64-apple-ios16.0 -arch arm64 --sysroot ../../../Toolchains/iOS/
if [ $? -ne 0 ]; then
    echo "Failed to build PNSLR_Intrinsics-iOS-ARM64"
else
    echo "Successfully built PNSLR_Intrinsics-iOS-ARM64"
fi

echo ""
echo ""
echo "Building PNSLR_Intrinsics-iOSSimulator-ARM64"
../../../Toolchains/BuildTools/bin/clang -c -std=c11 ./Intrinsics.c -o Prebuilt/intrinsics-iossimulator-arm64.o -miphoneos-version-min=16.0 -target arm64-apple-ios16.0-simulator --sysroot ../../../Toolchains/iOSSimulator/
if [ $? -ne 0 ]; then
    echo "Failed to build PNSLR_Intrinsics-iOSSimulator-ARM64"
else
    echo "Successfully built PNSLR_Intrinsics-iOSSimulator-ARM64"
fi

echo ""
echo ""
echo "Building PNSLR_Intrinsics-OSX-ARM64"
../../../Toolchains/BuildTools/bin/clang -c -std=c11 ./Intrinsics.c -o Prebuilt/intrinsics-osx-arm64.o -target arm64-apple-macos11.0 --sysroot ../../../Toolchains/OSX/
if [ $? -ne 0 ]; then
    echo "Failed to build PNSLR_Intrinsics-OSX-ARM64"
else
    echo "Successfully built PNSLR_Intrinsics-OSX-ARM64"
fi
