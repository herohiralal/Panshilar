#include "Dependencies/PNSLR_Intrinsics/Compiler.h"
PRAGMA_SUPPRESS_WARNINGS
#include <stdio.h>
PRAGMA_REENABLE_WARNINGS
#include "Panshilar.h"

b8 DirectoryStuffLister(void* payload, utf8str path, b8 directory)
{
    i32* listCount = (i32*) payload;
    printf("Found path %d (length: %d): %.*s\n", *listCount, (i32) path.count, (i32) path.count, path.data);
    (*listCount)++;
    return true;
}

void TestRunnerMain(ArraySlice(utf8str) args)
{
    for (i32 i = 0; i < args.count; ++i)
    {
        utf8str arg = args.data[i];
        printf("Argument %d (length: %d): %.*s\n", i, (i32) arg.count, (i32) arg.count, arg.data);
    }

    i32 listCount = 0;
    PNSLR_IterateDirectory(PNSLR_STRING_LITERAL("D:/Projects/Panshilar/Source"), true, &listCount, DirectoryStuffLister);
}

PNSLR_EXECUTABLE_ENTRY_POINT(TestRunnerMain)
