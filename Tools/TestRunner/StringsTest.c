#include "zzzz_TestRunner.h"

MAIN_TEST_FN(ctx)
{
    // --- CString Length ---
    char* helloCstr = "hello";
    Assert(PNSLR_GetCStringLength(helloCstr) == 5);

    // --- UTF8STR from CString ---
    PNSLR_UTF8STR helloStr = PNSLR_StringFromCString("hello");
    Assert(helloStr.count == 5);
    Assert(helloStr.data[0] == 'h');

    // --- Clone string to Cstr ---
    char* clonedCstr = PNSLR_CStringFromString(helloStr, ctx->testAllocator);
    AssertMsg(PNSLR_GetCStringLength(clonedCstr) == 5, "Cloned C string length mismatch");

    // --- Clone string ---
    PNSLR_UTF8STR clonedStr = PNSLR_CloneString(helloStr, ctx->testAllocator);
    Assert(clonedStr.count == 5);

    // --- Concatenate strings ---
    PNSLR_UTF8STR worldStr = PNSLR_StringFromCString("world");
    PNSLR_UTF8STR concatStr = PNSLR_ConcatenateStrings(helloStr, worldStr, ctx->testAllocator);
    Assert(concatStr.count == 10);

    // --- Casing ---
    PNSLR_UTF8STR upper = PNSLR_UpperString(worldStr, ctx->testAllocator);
    PNSLR_UTF8STR lower = PNSLR_LowerString(upper, ctx->testAllocator);
    Assert(PNSLR_AreStringsEqual(worldStr, lower, PNSLR_StringComparisonType_CaseSensitive));

    // --- Comparisons ---
    Assert(PNSLR_AreStringsEqual(helloStr, clonedStr, PNSLR_StringComparisonType_CaseSensitive));
    Assert(PNSLR_AreStringAndCStringEqual(helloStr, "hello", PNSLR_StringComparisonType_CaseSensitive));
    Assert(PNSLR_AreCStringsEqual("foo", "foo", PNSLR_StringComparisonType_CaseSensitive));

    // --- Prefix/Suffix ---
    Assert(PNSLR_StringStartsWith(helloStr, PNSLR_StringFromCString("he"), PNSLR_StringComparisonType_CaseSensitive));
    Assert(PNSLR_StringEndsWith(worldStr, PNSLR_StringFromCString("ld"), PNSLR_StringComparisonType_CaseSensitive));

    // --- Search ---
    PNSLR_UTF8STR foobarStr = PNSLR_StringFromCString("foobarfoo");
    PNSLR_UTF8STR fooStr = PNSLR_StringFromCString("foo");
    Assert(PNSLR_SearchFirstIndexInString(foobarStr, fooStr, PNSLR_StringComparisonType_CaseSensitive) == 0);
    Assert(PNSLR_SearchLastIndexInString(foobarStr, fooStr, PNSLR_StringComparisonType_CaseSensitive) == 6);

    // --- Replace ---
    PNSLR_UTF8STR barStr = PNSLR_StringFromCString("bar");
    PNSLR_UTF8STR replaced = PNSLR_ReplaceInString(foobarStr, fooStr, barStr, ctx->testAllocator, PNSLR_StringComparisonType_CaseSensitive);
    Log("Replaced string created");
    Assert(replaced.count == 9); // "barbarbar"
}
