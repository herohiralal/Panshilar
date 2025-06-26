#include "IO.h"
#include "Allocators.h"
#include "Sync.h"

// internal allocator stuff ========================================================

static thread_local u64             G_PathsInternalAllocatorUsageCount = 0;
static thread_local PNSLR_Allocator G_PathsInternalAllocator           = {.data = nil, .procedure = nil};

static ArraySlice(PNSLR_Allocator) G_PathsInternalAllocators      = {.count = 0, .data = nil};
static u64                         G_PathsInternalAllocatorsCount = 0;
static PNSLR_Mutex                 G_PathsInternalAllocatorsMutex = {0};
static b8                          G_PathsInternalAllocatorsInit  = false;

static PNSLR_Allocator AcquirePathsInternalAllocator(void)
{
    if (G_PathsInternalAllocatorUsageCount == 0)
    {
        // this is weird and must be done preferably in a procedural/imperative way but
        // for the sake of convenience, we don't
        // although for most of the program's run, this variable will remain read-only
        // so maybe it's not that bad
        if (!G_PathsInternalAllocatorsInit)
        {
            G_PathsInternalAllocatorsMutex = PNSLR_CreateMutex();
            G_PathsInternalAllocatorsInit  = true;
        }

        b8 createNew = false;

        PNSLR_LockMutex(&G_PathsInternalAllocatorsMutex);

        if (G_PathsInternalAllocatorsCount == 0)
        {
            createNew = true;
        }
        else
        {
            // there are allocators in the pool
            G_PathsInternalAllocator = G_PathsInternalAllocators.data[G_PathsInternalAllocatorsCount - 1];

            // clear from pool
            G_PathsInternalAllocators.data[G_PathsInternalAllocatorsCount - 1] = PNSLR_NIL_ALLOCATOR;
            G_PathsInternalAllocatorsCount--;
        }

        PNSLR_UnlockMutex(&G_PathsInternalAllocatorsMutex);

        if (createNew)
        {
            G_PathsInternalAllocatorUsageCount = 0; // reset
            PNSLR_AllocatorError error         = PNSLR_AllocatorError_None;
            G_PathsInternalAllocator           = PNSLR_NewAllocator_Stack(PNSLR_DEFAULT_HEAP_ALLOCATOR, CURRENT_LOC(), &error);

            // on failure, fallback to the heap allocator
            if (error != PNSLR_AllocatorError_None) { G_PathsInternalAllocator = PNSLR_DEFAULT_HEAP_ALLOCATOR; }
        }
    }

    G_PathsInternalAllocatorUsageCount++;
    return G_PathsInternalAllocator;
}

static void ReleasePathsInternalAllocator()
{
    G_PathsInternalAllocatorUsageCount--;
    if (G_PathsInternalAllocatorUsageCount <= 0)
    {
        PNSLR_LockMutex(&G_PathsInternalAllocatorsMutex);

        // resize the array if needed
        if (G_PathsInternalAllocatorsCount >= G_PathsInternalAllocators.count)
        {
            // create new
            i64 newSize = (G_PathsInternalAllocatorsCount + 1) * 2;
            ArraySlice(PNSLR_Allocator) newAllocators = PNSLR_MakeSlice(PNSLR_Allocator, newSize, false, PNSLR_DEFAULT_HEAP_ALLOCATOR, nil);

            // copy old
            PNSLR_Intrinsic_MemCopy(newAllocators.data, G_PathsInternalAllocators.data, (i32)(G_PathsInternalAllocatorsCount * sizeof(PNSLR_Allocator)));

            // delete old
            PNSLR_FreeSlice(G_PathsInternalAllocators, PNSLR_DEFAULT_HEAP_ALLOCATOR, nil);

            // set to new
            G_PathsInternalAllocators = newAllocators;
        }

        // add to the pool
        G_PathsInternalAllocators.data[G_PathsInternalAllocatorsCount] = G_PathsInternalAllocator;
        G_PathsInternalAllocatorsCount++;

        PNSLR_UnlockMutex(&G_PathsInternalAllocatorsMutex);
    }
}

// actual function implementations =================================================

b8 PNSLR_PathExists(utf8str path, PNSLR_PathCheckType type)
{
    PNSLR_Allocator internalAllocator = AcquirePathsInternalAllocator();

    // copy the filename to a temporary buffer
    ArraySlice(char) tempBuffer2 = PNSLR_MakeSlice(char, (path.count + 1), false, internalAllocator, nil);
    PNSLR_Intrinsic_MemCopy(tempBuffer2.data, path.data, (i32) path.count);
    tempBuffer2.data[path.count] = '\0';

    b8 canBeFile      = (type == PNSLR_PathCheckType_Either || type == PNSLR_PathCheckType_File);
    b8 canBeDirectory = (type == PNSLR_PathCheckType_Either || type == PNSLR_PathCheckType_Directory);

    b8 result         = false;

    #if PNSLR_WINDOWS

        DWORD fileAttributes = GetFileAttributesA(tempBuffer2.data); // TODO: switch to UTF-16 strings for better compatibility
        if (fileAttributes == INVALID_FILE_ATTRIBUTES)
        {
            result = false; // File does not exist
        }
        else if (canBeFile && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            result = true; // It's a file
        }
        else if (canBeDirectory && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            result = true; // It's a directory
        }

    #elif PNSLR_UNIX

        struct stat fileStat;
        if (stat(tempBuffer2.data, &fileStat) != 0)
        {
            result = false; // File does not exist
        }
        else if (canBeFile && S_ISREG(fileStat.st_mode))
        {
            result = true; // It's a file
        }
        else if (canBeDirectory && S_ISDIR(fileStat.st_mode))
        {
            result = true; // It's a directory
        }

    #endif

    PNSLR_FreeSlice(tempBuffer2, internalAllocator, nil);
    ReleasePathsInternalAllocator();

    return result;
}

i64 PNSLR_GetFileTimestamp(utf8str path)
{
    PNSLR_Allocator internalAllocator = AcquirePathsInternalAllocator();

    // copy the filename to a temporary buffer
    ArraySlice(char) tempBuffer2 = PNSLR_MakeSlice(char, (path.count + 1), false, internalAllocator, nil);
    PNSLR_Intrinsic_MemCopy(tempBuffer2.data, path.data, (i32) path.count);
    tempBuffer2.data[path.count] = '\0';

    i64 timestamp = 0;
    #if PNSLR_WINDOWS

        HANDLE fileHandle = CreateFileA(tempBuffer2.data, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (fileHandle != INVALID_HANDLE_VALUE)
        {
            FILETIME fileTime;
            if (GetFileTime(fileHandle, NULL, NULL, &fileTime))
            {
                // Convert FILETIME to nanoseconds since epoch
                ULARGE_INTEGER ull;
                ull.LowPart = fileTime.dwLowDateTime;
                ull.HighPart = fileTime.dwHighDateTime;
                timestamp = (i64)(ull.QuadPart - 116444736000000000ULL) * 100; // Convert to nanoseconds
            }
            CloseHandle(fileHandle);
        }

    #elif PNSLR_UNIX

        struct stat fileStat;
        if (stat(tempBuffer2.data, &fileStat) == 0)
        {
            // Convert to nanoseconds since epoch
            timestamp = (i64)fileStat.st_mtime * 1000000000LL; // Convert seconds to nanoseconds
        }

    #endif

    PNSLR_FreeSlice(tempBuffer2, internalAllocator, nil);
    ReleasePathsInternalAllocator();

    return timestamp;
}
