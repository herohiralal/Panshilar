@echo off

set MSVC_WARNINGS=/W4 /WX /we4013 /we4020 /we4047 /we4024 /we4028 /we4013 /we4715 /we4700 /we4716 /we4013
set CLANG_WARNINGS=-Wall -Wextra -Wshadow -Wconversion -Wsign-conversion -Wdouble-promotion -Wfloat-equal -Wundef -Wswitch-enum -Wstrict-prototypes -Werror
