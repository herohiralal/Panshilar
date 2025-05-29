@echo off

SetLocal EnableDelayedExpansion

if "%1" == "-firsttime" (
    call setup.bat
)

call build-libs.bat
call build-tools.bat
