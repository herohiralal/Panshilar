@echo off

setlocal EnableDelayedExpansion

rem Build Tools ====================================================================

rem Building most stuff here in debug mode because performance is not really
rem critical, but rather being able to debug issues quickly, update and move on.

call setup-vsdevcmd.bat
call warnings-setup.bat

set FAILED_COMPILATIONS=

echo.
echo.
echo Building for Windows-x64...
where cl.exe >nul 2>&1
if %errorlevel% equ 0 (
    cl.exe /Brepro /nologo /std:c11 Tools/TestRunner/TestRunner.c Libraries/panshilar-windows-x64.lib /DPNSLR_WINDOWS=1 /DPNSLR_X64=1 /ISource\ /D_DEBUG /Od /Zi /DEBUG /FoTemp/test-runner-windows-x64.obj /FeBinaries/TestRunner-Windows-X64.exe /FdBinaries\TestRunner-Windows-X64.pdb %MSVC_WARNINGS%
    if errorlevel 1 (
        echo ERROR: Windows-x64 TestRunner compilation failed!
        set FAILED_COMPILATIONS=!FAILED_COMPILATIONS! TestRunner-Windows-X64;
    ) else (
        echo SUCCESS: TestRunner.exe created successfully!
    )

    cl.exe /Brepro /nologo /std:c11 Tools/BindGen/BindingsGenerator.c Libraries/panshilar-windows-x64.lib /DPNSLR_WINDOWS=1 /DPNSLR_X64=1 /ISource\ /D_DEBUG /Od /Zi /DEBUG /FoTemp/bindings-generator-windows-x64.obj /FeBinaries/BindingsGenerator-Windows-X64.exe /FdBinaries\BindingsGenerator-Windows-X64.pdb %MSVC_WARNINGS%
    if errorlevel 1 (
        echo ERROR: Windows-x64 BindingsGenerator compilation failed!
        set FAILED_COMPILATIONS=!FAILED_COMPILATIONS! BindingsGenerator-Windows-X64;
    ) else (
        echo SUCCESS: BindingsGenerator.exe created successfully!
    )
)

echo.
echo.
echo Building for Linux-x64...
set LINUX_X64_TOOLCHAIN="Toolchains\Linux-x64\bin"
if exist %LINUX_X64_TOOLCHAIN%\clang.exe (
    "%LINUX_X64_TOOLCHAIN%\clang.exe" -std=c11 -DPNSLR_LINUX=1 -DPNSLR_X64=1 Tools/TestRunner/TestRunner.c Libraries/panshilar-linux-x64.a -o Binaries/TestRunner-Linux-X64 %CLANG_WARNINGS% -ISource\ --target=x86_64-unknown-linux-gnu --sysroot=Toolchains\Linux-x64\
    if errorlevel 1 (
        echo ERROR: Linux-x64 TestRunner compilation failed!
        set FAILED_COMPILATIONS=!FAILED_COMPILATIONS! TestRunner-Linux-X64;
    ) else (
        echo SUCCESS: TestRunner-Linux-X64 created successfully!
    )

    "%LINUX_X64_TOOLCHAIN%\clang.exe" -std=c11 -DPNSLR_LINUX=1 -DPNSLR_X64=1 Tools/BindGen/BindingsGenerator.c Libraries/panshilar-linux-x64.a -o Binaries/BindingsGenerator-Linux-X64 %CLANG_WARNINGS% -ISource\ --target=x86_64-unknown-linux-gnu --sysroot=Toolchains\Linux-x64\
    if errorlevel 1 (
        echo ERROR: Linux-x64 BindingsGenerator compilation failed!
        set FAILED_COMPILATIONS=!FAILED_COMPILATIONS! BindingsGenerator-Linux-X64;
    ) else (
        echo SUCCESS: BindingsGenerator-Linux-X64 created successfully!
    )
)

rem Summary ========================================================================

rem Print failed compilations summary
echo.
if not "!FAILED_COMPILATIONS!"=="" (
    echo Build failed for the following tools:!FAILED_COMPILATIONS!
    exit /b 1
) else (
    echo All tools built successfully!
    exit /b 0
)
