@echo off

SetLocal EnableDelayedExpansion

rem Gather toolchains ==============================================================

set WINDOWS_TOOLCHAIN_FOUND=0
set LINUX_X64_TOOLCHAIN_FOUND=0
set LINUX_ARM64_TOOLCHAIN_FOUND=0
set ANDROID_TOOLCHAIN_FOUND=0

rem Windows Toolchain

where /Q cl.exe >nul 2>&1
if errorlevel 1 (
    set __VSCMD_ARG_NO_LOGO=1
    for /f "tokens=*" %%i in ('"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath') do set VS=%%i
    if "!VS!" neq "" (
        call "!VS!\Common7\Tools\vsdevcmd.bat" -arch=x64 -host_arch=x64
    )
)

if '!VCTOOLSINSTALLDIR!' neq "" (
    set WINDOWS_TOOLCHAIN=!VCTOOLSINSTALLDIR!
    if "!WINDOWS_TOOLCHAIN:~-1!" == "\" (
        set WINDOWS_TOOLCHAIN=!WINDOWS_TOOLCHAIN:~0,-1!
    )
    set WINDOWS_TOOLCHAIN_FOUND=1
)

rem Linux Toolchains

if exist "%LINUX_MULTIARCH_ROOT%x86_64-unknown-linux-gnu\bin\clang.exe" (
    set LINUX_X64_TOOLCHAIN=%LINUX_MULTIARCH_ROOT%x86_64-unknown-linux-gnu
    set LINUX_X64_TOOLCHAIN_FOUND=1
)

if exist "%LINUX_MULTIARCH_ROOT%aarch64-unknown-linux-gnueabi\bin\clang.exe" (
    set LINUX_ARM64_TOOLCHAIN=%LINUX_MULTIARCH_ROOT%aarch64-unknown-linux-gnueabi
    set LINUX_ARM64_TOOLCHAIN_FOUND=1
)

rem Android Toolchain

if "!ANDROID_NDK_HOME!" equ "" (
    if exist "%LOCALAPPDATA%\Android\Sdk" (
        set ANDROID_SDK_ROOT=%LOCALAPPDATA%\Android\Sdk
        set ANDROID_HOME=!ANDROID_SDK_ROOT!

        if exist "!ANDROID_HOME!\ndk" (
            for /d %%i in ("!ANDROID_HOME!\ndk\*") do set ANDROID_NDK_HOME=%%i
        )

        if "!ANDROID_NDK_HOME!" neq "" (
            set ANDROID_TOOLCHAIN=!ANDROID_NDK_HOME!\toolchains\llvm\prebuilt\windows-x86_64
            set ANDROID_TOOLCHAIN_FOUND=1
        )
    )
)

rem Run Build Script ===============================================================

python build.py %*
