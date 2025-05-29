@echo off

SetLocal EnableDelayedExpansion

call ..\..\..\setup-vsdevcmd.bat
call ..\..\..\warnings-setup.bat

set TOOLCHAINS=..\..\..\Toolchains

echo.
echo.
echo Building PNSLR_Intrinsics-Windows-x64
cl.exe /Brepro /nologo /c /std:c11 .\Intrinsics.c /FoPrebuilt\intrinsics-windows-x64.obj %MSVC_WARNINGS%
if %errorlevel% neq 0 (
    echo Failed to build PNSLR_Intrinsics-Windows-x64
) else (
    echo Successfully built PNSLR_Intrinsics-Windows-x64
)

echo.
echo.
echo Building PNSLR_Intrinsics-Linux-x64
%TOOLCHAINS%\Linux-x64\bin\clang.exe -c -std=c11 .\Intrinsics.c -o Prebuilt\intrinsics-linux-x64.o --sysroot=%TOOLCHAINS%\Linux-x64\ %CLANG_WARNINGS% --target=x86_64-unknown-linux-gnu
if %errorlevel% neq 0 (
    echo Failed to build PNSLR_Intrinsics-Linux-x64
) else (
    echo Successfully built PNSLR_Intrinsics-Linux-x64
)

echo.
echo.
echo Building PNSLR_Intrinsics-Linux-ARM64
%TOOLCHAINS%\Linux-ARM64\bin\clang.exe -c -std=c11 .\Intrinsics.c -o Prebuilt\intrinsics-linux-arm64.o --sysroot=%TOOLCHAINS%\Linux-ARM64\ %CLANG_WARNINGS% --target=aarch64-unknown-linux-gnu
if %errorlevel% neq 0 (
    echo Failed to build PNSLR_Intrinsics-Linux-ARM64
) else (
    echo Successfully built PNSLR_Intrinsics-Linux-ARM64
)

echo.
echo.
echo Building PNSLR_Intrinsics-Android-ARM64
%TOOLCHAINS%\Android\toolchains\llvm\prebuilt\windows-x86_64\bin\clang.exe --target=aarch64-linux-android28 -c -std=c11 .\Intrinsics.c -o Prebuilt\intrinsics-android-arm64.o --sysroot=%TOOLCHAINS%\Android\toolchains\llvm\prebuilt\windows-x86_64\sysroot\ %CLANG_WARNINGS%
if %errorlevel% neq 0 (
    echo Failed to build PNSLR_Intrinsics-Android-ARM64
) else (
    echo Successfully built PNSLR_Intrinsics-Android-ARM64
)

