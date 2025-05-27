@echo off

SetLocal EnableDelayedExpansion

set TOOLCHAINS=..\..\..\Toolchains

echo.
echo.
echo Building PNSLR_Intrinsics-Windows-x64
%TOOLCHAINS%\Windows-x64\bin\Hostx64\x64\cl.exe /nologo /c /std:c11 .\Intrinsics.c /FoPrebuilt\intrinsics-windows-x64.obj /I%TOOLCHAINS%\WindowsSdkIncludes\um\ /I%TOOLCHAINS%\WindowsSdkIncludes\shared\ /I%TOOLCHAINS%\WindowsSdkIncludes\winrt\ /I%TOOLCHAINS%\WindowsSdkIncludes\ucrt\ /I%TOOLCHAINS%\WindowsIncludes\
if %errorlevel% neq 0 (
    echo Failed to build PNSLR_Intrinsics-Windows-x64
) else (
    echo Successfully built PNSLR_Intrinsics-Windows-x64
)

echo.
echo.
echo Building PNSLR_Intrinsics-Linux-x64
%TOOLCHAINS%\Linux-x64\bin\clang.exe -c -std=c11 .\Intrinsics.c -o Prebuilt\intrinsics-linux-x64.o -I%TOOLCHAINS%\Linux-x64\usr\include\
if %errorlevel% neq 0 (
    echo Failed to build PNSLR_Intrinsics-Linux-x64
) else (
    echo Successfully built PNSLR_Intrinsics-Linux-x64
)

echo.
echo.
echo Building PNSLR_Intrinsics-Linux-ARM64
%TOOLCHAINS%\Linux-ARM64\bin\clang.exe -c -std=c11 .\Intrinsics.c -o Prebuilt\intrinsics-linux-arm64.o -I%TOOLCHAINS%\Linux-ARM64\usr\include\
if %errorlevel% neq 0 (
    echo Failed to build PNSLR_Intrinsics-Linux-ARM64
) else (
    echo Successfully built PNSLR_Intrinsics-Linux-ARM64
)

echo.
echo.
echo Building PNSLR_Intrinsics-Android-ARM64
%TOOLCHAINS%\Android\toolchains\llvm\prebuilt\windows-x86_64\bin\clang.exe --target=aarch64-linux-android28 -c -std=c11 .\Intrinsics.c -o Prebuilt\intrinsics-android-arm64.o -I%TOOLCHAINS%\Android\toolchains\llvm\prebuilt\windows-x86_64\sysroot\usr\include\
if %errorlevel% neq 0 (
    echo Failed to build PNSLR_Intrinsics-Android-ARM64
) else (
    echo Successfully built PNSLR_Intrinsics-Android-ARM64
)

