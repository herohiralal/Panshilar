@echo off

setlocal EnableDelayedExpansion

call warnings-setup.bat

rem Build ==========================================================================

set FAILED_PLATFORMS=

rem Windows-x64
set WINDOWS_TOOLCHAIN="Toolchains\Windows-x64\bin\Hostx64\x64"
if exist %WINDOWS_TOOLCHAIN%\cl.exe (
    echo.
    echo.
    echo Compiling for Windows-x64...

    "%WINDOWS_TOOLCHAIN%\cl.exe" /nologo /c /std:c11 Source/zzzz_Unity.c /DPNSLR_IMPLEMENTATION /DPNSLR_WINDOWS=1 /DPNSLR_X64=1 /FoTemp/unity-windows-x64.obj /IToolchains\WindowsSdkIncludes\um\ /IToolchains\WindowsSdkIncludes\shared\ /IToolchains\WindowsSdkIncludes\winrt\ /IToolchains\WindowsSdkIncludes\ucrt\ /IToolchains\WindowsIncludes\ %MSVC_WARNINGS%
    if errorlevel 1 (
        echo ERROR: Windows-x64 compilation failed!
        set FAILED_PLATFORMS=!FAILED_PLATFORMS! Windows-x64
    ) else (
        "%WINDOWS_TOOLCHAIN%\lib.exe" /NOLOGO Temp/unity-windows-x64.obj Source/Dependencies/PNSLR_Intrinsics/Prebuilt/intrinsics-windows-x64.obj /OUT:Libraries/panshilar-windows-x64.lib
        if errorlevel 1 (
            echo ERROR: Windows-x64 library creation failed!
            set FAILED_PLATFORMS=!FAILED_PLATFORMS! Windows-x64
        ) else (
            echo SUCCESS: Static library panshilar-windows-x64.lib created successfully!
        )
    )
)

rem Linux-x64
set LINUX_X64_TOOLCHAIN="Toolchains\Linux-x64\bin"
if exist "%LINUX_X64_TOOLCHAIN%\clang.exe" (
    echo.
    echo.
    echo Compiling for Linux-x64...

    "%LINUX_X64_TOOLCHAIN%\clang.exe" -c -std=c11 -DPNSLR_IMPLEMENTATION -DPNSLR_LINUX=1 -DPNSLR_X64=1 Source/zzzz_Unity.c -o Temp/unity-linux-x64.o -IToolchains\Linux-x64\usr\include\ %CLANG_WARNINGS% --target=x86_64-unknown-linux-gnu
    if errorlevel 1 (
        echo ERROR: Linux-x64 compilation failed!
        set FAILED_PLATFORMS=!FAILED_PLATFORMS! Linux-x64
    ) else (
        "%LINUX_X64_TOOLCHAIN%\llvm-ar.exe" rcs Libraries/panshilar-linux-x64.a Temp/unity-linux-x64.o Source/Dependencies/PNSLR_Intrinsics/Prebuilt/intrinsics-linux-x64.o
        if errorlevel 1 (
            echo ERROR: Linux-x64 library creation failed!
            set FAILED_PLATFORMS=!FAILED_PLATFORMS! Linux-x64
        ) else (
            echo SUCCESS: Static library panshilar-linux-x64.a created successfully!
        )
    )
)

rem Linux-ARM64
set LINUX_ARM64_TOOLCHAIN="Toolchains\Linux-ARM64\bin"
if exist "%LINUX_ARM64_TOOLCHAIN%\clang.exe" (
    echo.
    echo.
    echo Compiling for Linux-ARM64...

    "%LINUX_ARM64_TOOLCHAIN%\clang.exe" -c -std=c11 -DPNSLR_IMPLEMENTATION -DPNSLR_LINUX=1 -DPNSLR_ARM64=1 Source/zzzz_Unity.c -o Temp/unity-linux-arm64.o -IToolchains\Linux-ARM64\usr\include\ %CLANG_WARNINGS% --target=aarch64-unknown-linux-gnu
    if errorlevel 1 (
        echo ERROR: Linux-ARM64 compilation failed!
        set FAILED_PLATFORMS=!FAILED_PLATFORMS! Linux-ARM64
    ) else (
        "%LINUX_ARM64_TOOLCHAIN%\llvm-ar.exe" rcs Libraries/panshilar-linux-arm64.a Temp/unity-linux-arm64.o Source/Dependencies/PNSLR_Intrinsics/Prebuilt/intrinsics-linux-arm64.o
        if errorlevel 1 (
            echo ERROR: Linux-ARM64 library creation failed!
            set FAILED_PLATFORMS=!FAILED_PLATFORMS! Linux-ARM64
        ) else (
            echo SUCCESS: Static library panshilar-linux-arm64.a created successfully!
        )
    )
)

rem Android-ARM64
set ANDROID_TOOLCHAIN="Toolchains\Android\toolchains\llvm\prebuilt\windows-x86_64\bin"
if exist "%ANDROID_TOOLCHAIN%\clang.exe" (
    echo.
    echo.
    echo Compiling for Android-ARM64...

    "%ANDROID_TOOLCHAIN%\clang.exe" --target=aarch64-linux-android28 -c -std=c11 -DPNSLR_IMPLEMENTATION -DPNSLR_ANDROID=1 -DPNSLR_ARM64=1 Source/zzzz_Unity.c -o Temp/unity-android-arm64.o -IToolchains\Android\toolchains\llvm\prebuilt\windows-x86_64\sysroot\usr\include\ %CLANG_WARNINGS%
    if errorlevel 1 (
        echo ERROR: Android-ARM64 compilation failed!
        set FAILED_PLATFORMS=!FAILED_PLATFORMS! Android-ARM64
    ) else (
        "%ANDROID_TOOLCHAIN%\llvm-ar.exe" rcs Libraries/panshilar-android-arm64.a Temp/unity-android-arm64.o Source/Dependencies/PNSLR_Intrinsics/Prebuilt/intrinsics-android-arm64.o
        if errorlevel 1 (
            echo ERROR: Android-ARM64 library creation failed!
            set FAILED_PLATFORMS=!FAILED_PLATFORMS! Android-ARM64
        ) else (
            echo SUCCESS: Static library panshilar-android-arm64.a created successfully!
        )
    )
)

rem Summary ========================================================================

rem Print failed platforms summary
echo.
if not "!FAILED_PLATFORMS!"=="" (
    echo Build failed for the following platforms:!FAILED_PLATFORMS!
    exit /b 1
) else (
    echo All platforms built successfully!
    exit /b 0
)
