#include "zzzz_TestRunner.h"

MAIN_TEST_FN(ctx)
{
    // --- CString Length ---
    cstring helloCstr = "hello";
    Assert(PNSLR_GetCStringLength(helloCstr) == 5);

    // --- UTF8STR from CString ---
    utf8str helloStr = PNSLR_StringFromCString("hello");
    Assert(helloStr.count == 5);
    Assert(helloStr.data[0] == 'h');

    // --- Clone string to Cstr ---
    cstring clonedCstr = PNSLR_CStringFromString(helloStr, ctx->testAllocator);
    AssertMsg(PNSLR_GetCStringLength(clonedCstr) == 5, "Cloned C string length mismatch");

    // --- Clone string ---
    utf8str clonedStr = PNSLR_CloneString(helloStr, ctx->testAllocator);
    Assert(clonedStr.count == 5);

    // --- Concatenate strings ---
    utf8str worldStr = PNSLR_StringFromCString("world");
    utf8str concatStr = PNSLR_ConcatenateStrings(helloStr, worldStr, ctx->testAllocator);
    Assert(concatStr.count == 10);

    // --- Casing ---
    utf8str upper = PNSLR_UpperString(worldStr, ctx->testAllocator);
    utf8str lower = PNSLR_LowerString(upper, ctx->testAllocator);
    Assert(PNSLR_AreStringsEqual(worldStr, lower, PNSLR_StringComparisonType_CaseSensitive));

    // --- Comparisons ---
    Assert(PNSLR_AreStringsEqual(helloStr, clonedStr, PNSLR_StringComparisonType_CaseSensitive));
    Assert(PNSLR_AreStringAndCStringEqual(helloStr, "hello", PNSLR_StringComparisonType_CaseSensitive));
    Assert(PNSLR_AreCStringsEqual("foo", "foo", PNSLR_StringComparisonType_CaseSensitive));

    // --- Prefix/Suffix ---
    Assert(PNSLR_StringStartsWith(helloStr, PNSLR_StringFromCString("he"), PNSLR_StringComparisonType_CaseSensitive));
    Assert(PNSLR_StringEndsWith(worldStr, PNSLR_StringFromCString("ld"), PNSLR_StringComparisonType_CaseSensitive));

    // --- Search ---
    utf8str foobarStr = PNSLR_StringFromCString("foobarfoo");
    utf8str fooStr = PNSLR_StringFromCString("foo");
    Assert(PNSLR_SearchFirstIndexInString(foobarStr, fooStr, PNSLR_StringComparisonType_CaseSensitive) == 0);
    Assert(PNSLR_SearchLastIndexInString(foobarStr, fooStr, PNSLR_StringComparisonType_CaseSensitive) == 6);

    // --- Replace ---
    utf8str barStr = PNSLR_StringFromCString("bar");
    utf8str replaced = PNSLR_ReplaceInString(foobarStr, fooStr, barStr, ctx->testAllocator, PNSLR_StringComparisonType_CaseSensitive);
    Log("Replaced string created");
    Assert(replaced.count == 9); // "barbarbar"
}
