#!/bin/bash
set -euo pipefail

echo ""
echo ""
echo "Building PNSLR_Intrinsics-iOS-ARM64"
clang -c -std=c11 -target arm64-apple-ios14.0 ./Intrinsics.c -o Prebuilt/intrinsics-ios-arm64.o
if [ $? -ne 0 ]; then
    echo "Failed to build PNSLR_Intrinsics-iOS-ARM64"
else
    echo "Successfully built PNSLR_Intrinsics-iOS-ARM64"
fi

echo ""
echo ""
echo "Building PNSLR_Intrinsics-OSX-ARM64"
clang -c -std=c11 -target arm64-apple-macos11 ./Intrinsics.c -o Prebuilt/intrinsics-osx-arm64.o
if [ $? -ne 0 ]; then
    echo "Failed to build PNSLR_Intrinsics-OSX-ARM64"
else
    echo "Successfully built PNSLR_Intrinsics-OSX-ARM64"
fi
