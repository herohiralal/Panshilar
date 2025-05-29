@echo off

set MSVC_WARNINGS=/Wall /WX /wd4100 /wd5045
set CLANG_WARNINGS=-Wall -Wextra -Wshadow -Wconversion -Wsign-conversion -Wdouble-promotion -Wfloat-equal -Wundef -Wswitch-enum -Wstrict-prototypes -Werror -Wno-unused-parameter
