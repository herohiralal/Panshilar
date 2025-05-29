#!/bin/bash
set -euo pipefail

# Build Tools ======================================================================

if [ -e ./Toolchains/OSX ]; then
    ./Toolchains/BuildTools/bin/clang -std=c11 ./Tools/TestRunner/TestRunner.c ./Libraries/panshilar-osx-arm64.a -o Binaries/TestRunner-osx-arm64 -target arm64-apple-macos11.0 --sysroot Toolchains/OSX/ -DPNSLR_OSX=1 -DPNSLR_ARM64=1 -I./Source/
    if [ $? -ne 0 ]; then
        echo "ERROR: TestRunner for OSX-ARM64 compilation failed!"
        exit 1
    else
        echo "SUCCESS: TestRunner for OSX-ARM64 created successfully!"
    fi

    ./Toolchains/BuildTools/bin/clang -std=c11 ./Tools/BindGen/BindingsGenerator.c ./Libraries/panshilar-osx-arm64.a -o Binaries/BindingsGenerator-osx-arm64 -target arm64-apple-macos11.0 --sysroot Toolchains/OSX/ -DPNSLR_OSX=1 -DPNSLR_ARM64=1 -I./Source/
    if [ $? -ne 0 ]; then
        echo "ERROR: BindingsGenerator for OSX-ARM64 compilation failed!"
        exit 1
    else
        echo "SUCCESS: BindingsGenerator for OSX-ARM64 created successfully!"
    fi
fi
