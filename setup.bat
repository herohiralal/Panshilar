@echo off

setlocal EnableDelayedExpansion

rem Gather toolchains ==============================================================

set LINUX_X64_TOOLCHAIN_FOUND=0
set LINUX_ARM64_TOOLCHAIN_FOUND=0
set ANDROID_TOOLCHAIN_FOUND=0

rem Linux Toolchains

set LINUX_X64_TOOLCHAIN=%LINUX_MULTIARCH_ROOT%x86_64-unknown-linux-gnu
if exist "%LINUX_X64_TOOLCHAIN%\bin\clang.exe" (
    set LINUX_X64_TOOLCHAIN_FOUND=1
    echo INFO: Linux-x64 toolchain found at %LINUX_X64_TOOLCHAIN%!
) else (
    echo WARNING: Linux-x64 toolchain not found at %LINUX_X64_TOOLCHAIN%!
)

set LINUX_ARM64_TOOLCHAIN=%LINUX_MULTIARCH_ROOT%aarch64-unknown-linux-gnueabi
if exist "%LINUX_ARM64_TOOLCHAIN%\bin\clang.exe" (
    set LINUX_ARM64_TOOLCHAIN_FOUND=1
    echo INFO: Linux-ARM64 toolchain found at %LINUX_ARM64_TOOLCHAIN%!
) else (
    echo WARNING: Linux-ARM64 toolchain not found at %LINUX_ARM64_TOOLCHAIN%!
)

rem Android Toolchain

if exist "%LOCALAPPDATA%\Android\Sdk\ndk" (
    for /d %%i in ("%LOCALAPPDATA%\Android\Sdk\ndk\*") do set NDK_ROOT=%%i\
)
if "!NDK_ROOT!" equ "" (
    for /f "tokens=*" %%i in ('where /r "%LOCALAPPDATA%" ndk-build.cmd 2^>nul') do set NDK_ROOT=%%~dpi
    echo WARNING: Searching for NDK in %LOCALAPPDATA% may take a while...
)
if "!NDK_ROOT!" equ "" (
    for /f "tokens=*" %%i in ('where /r "%PROGRAMFILES%" ndk-build.cmd 2^>nul') do set NDK_ROOT=%%~dpi
    echo WARNING: Searching for NDK in %PROGRAMFILES% may take a while...
)

set ANDROID_TOOLCHAIN=!NDK_ROOT!
if "!NDK_ROOT!" neq "" (
    set ANDROID_TOOLCHAIN_FOUND=1
    echo INFO: Android toolchain found at %ANDROID_TOOLCHAIN%!
) else (
    echo WARNING: Android toolchain not found!
)

rem Set up symlinks for toolchains =================================================

rem Remove existing symlinks in Toolchains directory
for /f "delims=" %%L in ('dir Toolchains\ /a:l /b 2^>nul') do (
    if exist "Toolchains\%%L\" (
        rmdir "Toolchains\%%L" /s /q >nul 2>&1 || (
            echo WARNING: Failed to remove existing symlink "Toolchains\%%L". It may be in use or require admin privileges.
        )
    )
)

set FAILED_SYMLINKS=

if "!LINUX_X64_TOOLCHAIN_FOUND!"=="1" (
    mklink /D "Toolchains\Linux-x64"          "!LINUX_X64_TOOLCHAIN!"                      >nul 2>&1 || FAILED_SYMLINKS=!FAILED_SYMLINKS! Linux-x64;
)

if "!LINUX_ARM64_TOOLCHAIN_FOUND!"=="1" (
    mklink /D "Toolchains\Linux-ARM64"        "!LINUX_ARM64_TOOLCHAIN!"                    >nul 2>&1 || FAILED_SYMLINKS=!FAILED_SYMLINKS! Linux-ARM64;
)

if "!ANDROID_TOOLCHAIN_FOUND!"=="1" (
    mklink /D "Toolchains\Android"            "!ANDROID_TOOLCHAIN!"                        >nul 2>&1 || FAILED_SYMLINKS=!FAILED_SYMLINKS! Android;
)

rem Summary ========================================================================

if "!FAILED_SYMLINKS!" neq "" (
    echo ERROR: Failed to create the following symlinks:!FAILED_SYMLINKS!
    exit /b 1
) else (
    echo SUCCESS: All symlinks created successfully!
    exit /b 0
)
