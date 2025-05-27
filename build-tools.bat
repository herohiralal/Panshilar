@echo off

setlocal EnableDelayedExpansion

rem Build Tools ====================================================================

rem Building most stuff here in debug mode because performance is not really
rem critical, but rather being able to debug issues quickly, update and move on.

set FAILED_COMPILATIONS=

rem Windows-x64
set WINDOWS_TOOLCHAIN="Toolchains\Windows-x64\bin\Hostx64\x64"
if exist %WINDOWS_TOOLCHAIN%\cl.exe (
    "%WINDOWS_TOOLCHAIN%\cl.exe" /nologo /std:c11 Tools/TestRunner/TestRunner.c Libraries/panshilar-windows-x64.lib /DPNSLR_WINDOWS=1 /DPNSLR_X64=1 /IToolchains\WindowsSdkIncludes\um\ /IToolchains\WindowsSdkIncludes\shared\ /IToolchains\WindowsSdkIncludes\winrt\ /IToolchains\WindowsSdkIncludes\ucrt\ /IToolchains\WindowsIncludes\ /ISource\ /D_DEBUG /Od /Zi /DEBUG /FoTemp/test-runner-windows-x64.obj /FeBinaries/TestRunner-Windows-X64.exe /FdBinaries\TestRunner-Windows-X64.pdb /link /LIBPATH:Toolchains\WindowsSdkLibs\ucrt\x64\ /LIBPATH:Toolchains\WindowsSdkLibs\um\x64\ /LIBPATH:Toolchains\WindowsLibs\x64\
    if errorlevel 1 (
        echo ERROR: Windows-x64 TestRunner compilation failed!
        set FAILED_COMPILATIONS=!FAILED_COMPILATIONS! TestRunner-Windows-X64;
    ) else (
        echo SUCCESS: TestRunner.exe created successfully!
    )

    "%WINDOWS_TOOLCHAIN%\cl.exe" /nologo /std:c11 Tools/BindGen/BindingsGenerator.c Libraries/panshilar-windows-x64.lib /DPNSLR_WINDOWS=1 /DPNSLR_X64=1 /IToolchains\WindowsSdkIncludes\um\ /IToolchains\WindowsSdkIncludes\shared\ /IToolchains\WindowsSdkIncludes\winrt\ /IToolchains\WindowsSdkIncludes\ucrt\ /IToolchains\WindowsIncludes\ /D_DEBUG /Od /Zi /DEBUG /ISource\ /FoTemp/bindings-generator-windows-x64.obj /FeBinaries/BindingsGenerator-Windows-X64.exe /FdBinaries\BindingsGenerator-Windows-X64.pdb /link /LIBPATH:Toolchains\WindowsSdkLibs\ucrt\x64\ /LIBPATH:Toolchains\WindowsSdkLibs\um\x64\ /LIBPATH:Toolchains\WindowsLibs\x64\
    if errorlevel 1 (
        echo ERROR: Windows-x64 BindingsGenerator compilation failed!
        set FAILED_COMPILATIONS=!FAILED_COMPILATIONS! BindingsGenerator-Windows-X64;
    ) else (
        echo SUCCESS: BindingsGenerator.exe created successfully!
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
