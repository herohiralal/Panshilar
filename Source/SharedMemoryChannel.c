#define PNSLR_IMPLEMENTATION
#include "SharedMemoryChannel.h"
#include "Allocators.h"
#include "Memory.h"
#include "Strings.h"
#include "Logger.h"


#if PNSLR_WINDOWS
    static_assert(sizeof(PNSLR_SharedMemoryChannelHandle) >= sizeof(HANDLE), "");
    typedef HANDLE PNSLR_Internal_NativeReadChannelHandle;
#elif PNSLR_LINUX || PNSLR_OSX
    static_assert(sizeof(PNSLR_SharedMemoryChannelHandle) >= sizeof(i32), "");
    typedef i32 PNSLR_Internal_NativeReadChannelHandle;
#else
    static_assert(sizeof(PNSLR_SharedMemoryChannelHandle) >= sizeof(rawptr), "");
    typedef rawptr PNSLR_Internal_NativeReadChannelHandle;
#endif

static PNSLR_SharedMemoryChannelHandle PNSLR_Internal_MakeReadChannelHandle(PNSLR_Internal_NativeReadChannelHandle h) { return (PNSLR_SharedMemoryChannelHandle) { .handle = (i64) h }; }
static PNSLR_Internal_NativeReadChannelHandle PNSLR_Internal_BreakReadChannelHandle(PNSLR_SharedMemoryChannelHandle h) { return (PNSLR_Internal_NativeReadChannelHandle) h.handle; }

#define PNSLR_INTERNAL_OFFSET_TO_SMC_HEADER_OS_SPECIFIC (64) // 1-2 cachelines
#define PNSLR_INTERNAL_OFFSET_TO_SMC_MSG_QUEUE_HEADER   (128)
#define PNSLR_INTERNAL_OFFSET_TO_SMC_MSG_DATA           (256)

#define PNSLR_INTERNAL_SMC_PER_MESSAGE_OVERHEAD_SIZE    (16) // 8-byte per message magic stamp, 8-byte length; magic stamp helps with debugging

// NOTE: all length stamps are non-negative numbers, so our wrapping sentinel being negative
// means it'll not be mistaken for a valid length; but being boring an writing -1 is not good since
// that's a fill patatern someone else might use; so we use a more interesting value
#define PNSLR_INTERNAL_SMC_WRAPPING_SENTINEL ((i64) 0xcafebabef00dface) // if reader sees this in length slot, wrap to start of buffer

static_assert(sizeof(PNSLR_SharedMemoryChannelHeader) <= PNSLR_INTERNAL_OFFSET_TO_SMC_HEADER_OS_SPECIFIC, "");
static_assert(sizeof(PNSLR_SharedMemoryChannelPlatformHeader) + PNSLR_INTERNAL_OFFSET_TO_SMC_HEADER_OS_SPECIFIC < PNSLR_INTERNAL_OFFSET_TO_SMC_MSG_QUEUE_HEADER, "");

#define PNSLR_INTERNAL_SMC_CHANNEL_HEADER_MAGIC_NUM  (*(u32*) "RDJX")
#if PNSLR_WINDOWS
    #define PNSLR_INTERNAL_SMC_PLATFORM_HEADER_MAGIC_NUM (*(u32*) "WNDW")
#elif PNSLR_LINUX || PNSLR_OSX
    #define PNSLR_INTERNAL_SMC_PLATFORM_HEADER_MAGIC_NUM (*(u32*) "POSX")
#else
    #define PNSLR_INTERNAL_SMC_PLATFORM_HEADER_MAGIC_NUM (*(u32*) "UNKN")
#endif
#define PNSLR_INTERNAL_SMC_MESSAGE_MAGIC_NUM (*(i64*) "COOLMSG!")

#define PNSLR_INTERNAL_SMC_MSG_SYS_VERSION (1)

#if PNSLR_DESKTOP
    static b8 PNSLR_Internal_GetSizeOfFullSMCMappingFromStubHeader(rawptr ptr, i64* outSize)
    {
        if (outSize) *outSize = 0;
        if (!ptr) return false;

        // initially size of region is unknown so map header
        // then look in header to find full region and remap it

        PNSLR_SharedMemoryChannelHeader hdrCpy = *(PNSLR_SharedMemoryChannelHeader*) ptr;
        u32 expectedMagic = PNSLR_INTERNAL_SMC_CHANNEL_HEADER_MAGIC_NUM;

        if (hdrCpy.magicNum != expectedMagic)
        {
            PNSLR_LogEf(
                PNSLR_StringLiteral("Magic num mismatch. Expected: $, actual: $"),
                PNSLR_FmtArgs(
                    PNSLR_FmtU32(expectedMagic, PNSLR_IntegerBase_HexaDecimal),
                    PNSLR_FmtU32(hdrCpy.magicNum, PNSLR_IntegerBase_HexaDecimal)
                ),
                PNSLR_GET_LOC()
            );
            return false;
        }
        else if (hdrCpy.version != PNSLR_INTERNAL_SMC_MSG_SYS_VERSION)
        {
            PNSLR_LogEf(
                PNSLR_StringLiteral("Version mismatch. Expected: $, actual: $"),
                PNSLR_FmtArgs(
                    PNSLR_FmtU32(PNSLR_INTERNAL_SMC_MSG_SYS_VERSION, PNSLR_IntegerBase_Decimal),
                    PNSLR_FmtU32(hdrCpy.version, PNSLR_IntegerBase_Decimal)
                ),
                PNSLR_GET_LOC()
            );
            return false;
        }

        if (outSize) *outSize = hdrCpy.fullMemRegionSize;
        if (hdrCpy.fullMemRegionSize < PNSLR_INTERNAL_OFFSET_TO_SMC_MSG_DATA)
        {
            PNSLR_LogEf(
                PNSLR_StringLiteral("Shared memory channel region size too small. Minimum is $, actual is $"),
                PNSLR_FmtArgs(
                    PNSLR_FmtI64(PNSLR_INTERNAL_OFFSET_TO_SMC_MSG_DATA, PNSLR_IntegerBase_Decimal),
                    PNSLR_FmtI64(hdrCpy.fullMemRegionSize, PNSLR_IntegerBase_Decimal)
                ),
                PNSLR_GET_LOC()
            );
            return false;
        }

        return true;
    }
#endif

static b8 PNSLR_Internal_CreatePlatformSMC(utf8str name, i64 size, u8** outData, PNSLR_Internal_NativeReadChannelHandle* chHandle)
{
    if (outData) *outData = nil;
    if (chHandle) *chHandle = 0;

    if (name.count > 255) name.count = 255;
    if (!name.count) return false;

    #if PNSLR_WINDOWS
    {
        WCHAR wName[256];
        MultiByteToWideChar(CP_UTF8, 0, (cstring) name.data, (i32) name.count, wName, (i32) 256);
        WCHAR* nativeName = wName;

        i64 totalBytes = size + PNSLR_INTERNAL_OFFSET_TO_SMC_MSG_DATA;

        HANDLE mapping = CreateFileMappingW(
            INVALID_HANDLE_VALUE,
            NULL,
            PAGE_READWRITE,
            (DWORD) (totalBytes >> 32),
            (DWORD) (totalBytes & 0xFFFFFFFF),
            nativeName
        );

        if (mapping == NULL)
        {
            PNSLR_LogEf(
                PNSLR_StringLiteral("CreateFileMappingW failed with error code $"),
                PNSLR_FmtArgs(
                    PNSLR_FmtU32((u32) GetLastError(), PNSLR_IntegerBase_Decimal)
                ),
                PNSLR_GET_LOC()
            );
            return false;
        }

        rawptr ptr = MapViewOfFile(
            mapping,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            (SIZE_T) totalBytes
        );

        if (!ptr)
        {
            PNSLR_LogEf(
                PNSLR_StringLiteral("MapViewOfFile failed with error code $"),
                PNSLR_FmtArgs(
                    PNSLR_FmtU32((u32) GetLastError(), PNSLR_IntegerBase_Decimal)
                ),
                PNSLR_GET_LOC()
            );
            CloseHandle(mapping);
            return false;
        }

        if (outData) *outData = (u8*) ptr;
        if (chHandle) *chHandle = mapping;
        return true;
    }
    #elif PNSLR_LINUX || PNSLR_OSX
    {
        char nameBuf[256];
        PNSLR_MemCopy(nameBuf, name.data, (i32) name.count);
        nameBuf[name.count] = '\0';
        cstring nativeName = nameBuf;

        i64 totalBytes = size + PNSLR_INTERNAL_OFFSET_TO_SMC_MSG_DATA;
        i32 shm = shm_open(nativeName, O_CREAT | O_RDWR, 0777);
        if (shm == -1)
        {
            PNSLR_LogEf(
                PNSLR_StringLiteral("shm_open failed with error code $"),
                PNSLR_FmtArgs(
                    PNSLR_FmtI32(errno, PNSLR_IntegerBase_Decimal)
                ),
                PNSLR_GET_LOC()
            );
            return false;
        }

        b8 truncate = true;
        #if PNSLR_OSX
            // TODO
        #endif

        if (truncate)
        {
            i32 truncateResult = ftruncate(shm, totalBytes);
            if (truncateResult == -1)
            {
                PNSLR_LogEf(
                    PNSLR_StringLiteral("ftruncate failed with error code $"),
                    PNSLR_FmtArgs(
                        PNSLR_FmtI32(errno, PNSLR_IntegerBase_Decimal)
                    ),
                    PNSLR_GET_LOC()
                );
                return false;
            }
        }

        i32 flags = MAP_SHARED;
        #if PNSLR_LINUX
            flags |= MAP_POPULATE;
        #endif

        rawptr ptr = mmap(NULL, (size_t) totalBytes, PROT_READ | PROT_WRITE, flags, shm, 0);
        if (ptr == MAP_FAILED)
        {
            PNSLR_LogEf(
                PNSLR_StringLiteral("mmap failed with error code $"),
                PNSLR_FmtArgs(
                    PNSLR_FmtI32(errno, PNSLR_IntegerBase_Decimal)
                ),
                PNSLR_GET_LOC()
            );
            close(shm);
            return false;
        }

        if (outData) *outData = (u8*) ptr;
        if (chHandle) *chHandle = shm;
        return true;
    }
    #else
    {
        return false;
    }
    #endif
}

static PNSLR_SharedMemoryChannelMessageQueueHeader* PNSLR_Internal_GetSMCMsgQueueHeader(PNSLR_SharedMemoryChannelHeader* hdr)
{
    if (!hdr) return nil;
    return (PNSLR_SharedMemoryChannelMessageQueueHeader*) (((u8*) hdr) + hdr->offsetToMsgQueueHeader);
}

b8 PNSLR_CreateSharedMemoryChannelReader(utf8str name, i64 size, PNSLR_SharedMemoryChannelReader* reader)
{
    if (reader) *reader = (PNSLR_SharedMemoryChannelReader) {0};

    PNSLR_Internal_NativeReadChannelHandle handle; u8* data;
    if (!PNSLR_Internal_CreatePlatformSMC(name, size, &data, &handle))
        return false;

    PNSLR_SharedMemoryChannelHeader* hdr = (PNSLR_SharedMemoryChannelHeader*) data;

    if (reader)
    {
        reader->header = hdr;
        reader->handle = PNSLR_Internal_MakeReadChannelHandle(handle);
    }

    PNSLR_SharedMemoryChannelPlatformHeader* pltHdr = (PNSLR_SharedMemoryChannelPlatformHeader*) (data + PNSLR_INTERNAL_OFFSET_TO_SMC_HEADER_OS_SPECIFIC);
    pltHdr->magicNum = PNSLR_INTERNAL_SMC_PLATFORM_HEADER_MAGIC_NUM;

    hdr->magicNum     = PNSLR_INTERNAL_SMC_CHANNEL_HEADER_MAGIC_NUM;
    hdr->version      = PNSLR_INTERNAL_SMC_MSG_SYS_VERSION;
    hdr->readerStatus = PNSLR_SharedMemoryChannelStatus_Active;

    hdr->offsetToOsSpecificHeader = PNSLR_INTERNAL_OFFSET_TO_SMC_HEADER_OS_SPECIFIC;
    hdr->offsetToMsgQueueHeader   = PNSLR_INTERNAL_OFFSET_TO_SMC_MSG_QUEUE_HEADER;
    hdr->offsetToMsgData          = PNSLR_INTERNAL_OFFSET_TO_SMC_MSG_DATA;

    hdr->fullMemRegionSize = size + PNSLR_INTERNAL_OFFSET_TO_SMC_MSG_DATA;
    hdr->dataSize          = size;

    PNSLR_SharedMemoryChannelMessageQueueHeader* mq = PNSLR_Internal_GetSMCMsgQueueHeader(hdr);
    mq->readCursor  = PNSLR_INTERNAL_OFFSET_TO_SMC_MSG_DATA;
    mq->writeCursor = PNSLR_INTERNAL_OFFSET_TO_SMC_MSG_DATA;

    return true;
}

b8 PNSLR_ReadSharedMemoryChannelMessage(PNSLR_SharedMemoryChannelReader* reader, PNSLR_SharedMemoryChannelMessage* message, b8* fatalError)
{
    if (!reader) return false;
    if (message) *message = (PNSLR_SharedMemoryChannelMessage) {0};
    if (fatalError) *fatalError = false;

    PNSLR_SharedMemoryChannelHeader* hdr = reader->header;

    PNSLR_SharedMemoryChannelMessageQueueHeader* mq = PNSLR_Internal_GetSMCMsgQueueHeader(hdr);
    if (!mq)
    {
        if (fatalError) *fatalError = true;
        return false;
    }

    i64 readCursor = mq->readCursor;
    if (readCursor == mq->writeCursor)
    {
        // no data
        return false;
    }

    u8* data = (u8*) hdr;

    i64* magicNumPtr = (i64*) (data + readCursor);
    i64* lengthPtr   = (i64*) (data + readCursor + sizeof(i64));

    if (*magicNumPtr != PNSLR_INTERNAL_SMC_MESSAGE_MAGIC_NUM)
    {
        if (*magicNumPtr == PNSLR_INTERNAL_SMC_WRAPPING_SENTINEL)
        {
            // wrap the buffer

            if (readCursor == (i64) hdr->offsetToMsgData)
            {
                PNSLR_LogE(
                    PNSLR_StringLiteral("Reader saw wrapping sentinel at start of buffer"),
                    PNSLR_GET_LOC()
                );

                if (fatalError) *fatalError = true;
                return false;
            }

            mq->readCursor = hdr->offsetToMsgData;
            return PNSLR_ReadSharedMemoryChannelMessage(reader, message, fatalError);
        }

        PNSLR_LogEf(
            PNSLR_StringLiteral("Message magic number mismatch. Expected: $, actual: $"),
            PNSLR_FmtArgs(
                PNSLR_FmtI64(PNSLR_INTERNAL_SMC_MESSAGE_MAGIC_NUM, PNSLR_IntegerBase_HexaDecimal),
                PNSLR_FmtI64(*magicNumPtr, PNSLR_IntegerBase_HexaDecimal)
            ),
            PNSLR_GET_LOC()
        );

        if (fatalError) *fatalError = true;
        return false;
    }

    i64 msgLength = *lengthPtr;
    if (msgLength < PNSLR_INTERNAL_SMC_PER_MESSAGE_OVERHEAD_SIZE)
    {
        PNSLR_LogEf(
            PNSLR_StringLiteral("Message length too small. Minimum is $, actual is $"),
            PNSLR_FmtArgs(
                PNSLR_FmtI64(PNSLR_INTERNAL_SMC_PER_MESSAGE_OVERHEAD_SIZE, PNSLR_IntegerBase_Decimal),
                PNSLR_FmtI64(msgLength, PNSLR_IntegerBase_Decimal)
            ),
            PNSLR_GET_LOC()
        );

        if (fatalError) *fatalError = true;
        return false;
    }

    i64 end = (i64) (hdr->offsetToMsgData) + hdr->dataSize;
    i64 remaining = end - readCursor;
    if (msgLength > remaining)
    {
        PNSLR_LogEf(
            PNSLR_StringLiteral("Message length $ exceeds remaining buffer size $"),
            PNSLR_FmtArgs(
                PNSLR_FmtI64(msgLength, PNSLR_IntegerBase_Decimal),
                PNSLR_FmtI64(remaining, PNSLR_IntegerBase_Decimal)
            ),
            PNSLR_GET_LOC()
        );

        if (fatalError) *fatalError = true;
        return false;
    }

    if (message)
    {
        message->channel = reader;
        message->offset  = readCursor;
        message->size    = msgLength;

        message->readPtr  = data + readCursor + PNSLR_INTERNAL_SMC_PER_MESSAGE_OVERHEAD_SIZE;
        message->readSize = msgLength - PNSLR_INTERNAL_SMC_PER_MESSAGE_OVERHEAD_SIZE;
    }

    return true;
}

b8 PNSLR_AcknowledgeSharedMemoryChannelMessage(PNSLR_SharedMemoryChannelMessage* message)
{
    if (!message) return false;

    PNSLR_SharedMemoryChannelReader* rd = message->channel;
    if (!rd) FORCE_DBG_TRAP;

    PNSLR_SharedMemoryChannelHeader* hdr = rd->header;

    PNSLR_SharedMemoryChannelMessageQueueHeader* mq = PNSLR_Internal_GetSMCMsgQueueHeader(hdr);
    if (!mq) FORCE_DBG_TRAP;

    i64 readCursor = mq->readCursor;
    readCursor += message->size;

    i64 end = (i64) (hdr->offsetToMsgData) + hdr->dataSize;
    if (end - readCursor < PNSLR_INTERNAL_SMC_PER_MESSAGE_OVERHEAD_SIZE)
    {
        // wrap
        readCursor = PNSLR_INTERNAL_OFFSET_TO_SMC_MSG_DATA;
    }

    mq->readCursor = readCursor;
    return true;
}

b8 PNSLR_DestroySharedMemoryChannelReader(PNSLR_SharedMemoryChannelReader* reader)
{
    if (!reader) return false;

    u8* data = (u8*) reader->header;
    if (!data) return false;

    PNSLR_Internal_NativeReadChannelHandle h = PNSLR_Internal_BreakReadChannelHandle(reader->handle);
    #if PNSLR_WINDOWS
    {
        UnmapViewOfFile(data);
        if (h) CloseHandle(h);
        return true;
    }
    #elif PNSLR_LINUX || PNSLR_OSX
    {
        munmap(data, (size_t) reader->header->fullMemRegionSize);
        if (h) close(h);
        return true;
    }
    #else
    {
        ((void) h);
        return false;
    }
    #endif
}

b8 PNSLR_TryConnectSharedMemoryChannelWriter(utf8str name, PNSLR_SharedMemoryChannelWriter* writer)
{
    if (name.count > 255) name.count = 255;
    if (!name.count) return false;
    if (writer) *writer = (PNSLR_SharedMemoryChannelWriter) {0};

    PNSLR_Internal_NativeReadChannelHandle handle = {0};
    u8* data = nil;
    #if PNSLR_WINDOWS
    {
        WCHAR wName[256];
        MultiByteToWideChar(CP_UTF8, 0, (cstring) name.data, (i32) name.count, wName, (i32) 256);
        WCHAR* nativeName = wName;

        HANDLE mapping = OpenFileMappingW(FILE_MAP_ALL_ACCESS, false, nativeName);
        if (!mapping)
        {
            if (GetLastError() == ERROR_FILE_NOT_FOUND)
            {
                // doesn't exist
                return false;
            }

            PNSLR_LogEf(
                PNSLR_StringLiteral("OpenFileMappingW failed with error code $"),
                PNSLR_FmtArgs(
                    PNSLR_FmtU32((u32) GetLastError(), PNSLR_IntegerBase_Decimal)
                ),
                PNSLR_GET_LOC()
            );
            return false;
        }

        rawptr headerPtr = MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, PNSLR_INTERNAL_OFFSET_TO_SMC_MSG_DATA);
        if (!headerPtr)
        {
            PNSLR_LogEf(
                PNSLR_StringLiteral("MapViewOfFile failed with error code $"),
                PNSLR_FmtArgs(
                    PNSLR_FmtU32((u32) GetLastError(), PNSLR_IntegerBase_Decimal)
                ),
                PNSLR_GET_LOC()
            );
            CloseHandle(mapping);
            return false;
        }

        i64 actualSize;
        b8 headerSuccess = PNSLR_Internal_GetSizeOfFullSMCMappingFromStubHeader(headerPtr, &actualSize);
        UnmapViewOfFile(headerPtr);
        if (!headerSuccess)
        {
            CloseHandle(mapping);
            return false;
        }

        rawptr fullPtr = MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, (SIZE_T) actualSize);
        if (!fullPtr)
        {
            PNSLR_LogEf(
                PNSLR_StringLiteral("MapViewOfFile failed with error code $"),
                PNSLR_FmtArgs(
                    PNSLR_FmtU32((u32) GetLastError(), PNSLR_IntegerBase_Decimal)
                ),
                PNSLR_GET_LOC()
            );
            CloseHandle(mapping);
            return false;
        }

        handle = mapping;
        data = (u8*) fullPtr;
    }
    #elif PNSLR_LINUX || PNSLR_OSX
    {
        char nameBuf[256];
        PNSLR_MemCopy(nameBuf, name.data, (i32) name.count);
        nameBuf[name.count] = '\0';
        cstring nativeName = nameBuf;

        i32 shm = shm_open(nativeName, O_RDWR, 0);
        if (shm == -1)
        {
            if (errno == ENOENT)
            {
                // doesn't exist
                return false;
            }

            PNSLR_LogEf(
                PNSLR_StringLiteral("shm_open failed with error code $"),
                PNSLR_FmtArgs(
                    PNSLR_FmtI32(errno, PNSLR_IntegerBase_Decimal)
                ),
                PNSLR_GET_LOC()
            );
            return false;
        }

        rawptr headerPtr = mmap(nil, sizeof(PNSLR_SharedMemoryChannelHeader), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
        if (headerPtr == MAP_FAILED)
        {
            PNSLR_LogEf(
                PNSLR_StringLiteral("mmap failed with error code $"),
                PNSLR_FmtArgs(
                    PNSLR_FmtI32(errno, PNSLR_IntegerBase_Decimal)
                ),
                PNSLR_GET_LOC()
            );
            close(shm);
            return false;
        }

        i64 actualSize;
        b8 headerSuccess = PNSLR_Internal_GetSizeOfFullSMCMappingFromStubHeader(headerPtr, &actualSize);
        munmap(headerPtr, sizeof(PNSLR_SharedMemoryChannelHeader));
        if (!headerSuccess)
        {
            close(shm);
            return false;
        }

        rawptr fullPtr = mmap(nil, (size_t) actualSize, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
        if (fullPtr == MAP_FAILED)
        {
            PNSLR_LogEf(
                PNSLR_StringLiteral("mmap failed with error code $"),
                PNSLR_FmtArgs(
                    PNSLR_FmtI32(errno, PNSLR_IntegerBase_Decimal)
                ),
                PNSLR_GET_LOC()
            );
            close(shm);
            return false;
        }

        handle = shm;
        data = (u8*) fullPtr;
    }
    #else
    {
        // no op
    }
    #endif

    if (!data) return false;

    PNSLR_SharedMemoryChannelPlatformHeader* pltHdr = (PNSLR_SharedMemoryChannelPlatformHeader*) (data + PNSLR_INTERNAL_OFFSET_TO_SMC_HEADER_OS_SPECIFIC);
    pltHdr->magicNum = PNSLR_INTERNAL_SMC_PLATFORM_HEADER_MAGIC_NUM;

    if (writer)
    {
        writer->header = (PNSLR_SharedMemoryChannelHeader*) data;
        writer->handle = PNSLR_Internal_MakeReadChannelHandle(handle);
    }

    return true;
}

b8 PNSLR_PrepareSharedMemoryChannelMessage(PNSLR_SharedMemoryChannelWriter* writer, i64 size, PNSLR_SharedMemoryChannelReservedMessage* reservedMessage)
{
    if (!writer) return false;
    if (reservedMessage) *reservedMessage = (PNSLR_SharedMemoryChannelReservedMessage) {0};
    if (size <= 0) return false;

    i64 totalSize = size + PNSLR_INTERNAL_SMC_PER_MESSAGE_OVERHEAD_SIZE;

    PNSLR_SharedMemoryChannelHeader* hdr = writer->header;

    i64 arenaStart = (i64) (hdr->offsetToMsgData);
    i64 arenaEnd   = arenaStart + hdr->dataSize;

    PNSLR_SharedMemoryChannelMessageQueueHeader* mq = PNSLR_Internal_GetSMCMsgQueueHeader(hdr);
    if (!mq) return false;

    i64 targetPos       = 0; // unset; all valid offsets are > 0, so any 0 means place for this message
    i64 wrapSentinelPos = 0; // this at 0 means we wrappedd, so tell reader

    i64 writeCursor = mq->writeCursor;
    i64 readCursor  = mq->readCursor;

    if (writeCursor >= readCursor)
    {
        // test against end of buffer
        i64 remaining = arenaEnd - writeCursor;

        if (remaining >= totalSize)
        {
            targetPos = writeCursor;
        }
        else
        {
            i64 end = hdr->offsetToMsgData + hdr->dataSize;
            if (end - writeCursor >= PNSLR_INTERNAL_SMC_PER_MESSAGE_OVERHEAD_SIZE)
            {
                // we have space to write a wrapping sentinel
                wrapSentinelPos = writeCursor;
            }

            writeCursor = hdr->offsetToMsgData;

            if (totalSize >= hdr->dataSize - 1)
            {
                // message too large for buffer
                return false;
            }
        }
    }

    if (!targetPos)
    {
        // test for space from beginning to read cursor
        if (writeCursor > readCursor) FORCE_DBG_TRAP; // reader caught up while writer is wrapping
        i64 remaining = readCursor - writeCursor - 1; // -1 so can't meet reada cursor from bottom

        if (remaining >= totalSize)
        {
            targetPos = hdr->offsetToMsgData;
        }
        else
        {
            // no space
            return false;
        }
    }

    if (wrapSentinelPos)
    {
        if (wrapSentinelPos <= hdr->offsetToMsgData) FORCE_DBG_TRAP; // can't wrap at start of buffer
        u8* destination = ((u8*) hdr) + wrapSentinelPos;
        *(i64*) destination = PNSLR_INTERNAL_SMC_WRAPPING_SENTINEL;
    }

    if (targetPos <= 0) FORCE_DBG_TRAP;

    u8* dataStart = (u8*) hdr;

    if (reservedMessage)
    {
        reservedMessage->channel  = writer;
        reservedMessage->offset   = writeCursor;
        reservedMessage->size     = totalSize;
        reservedMessage->writePtr = dataStart + writeCursor + PNSLR_INTERNAL_SMC_PER_MESSAGE_OVERHEAD_SIZE;
    }

    return true;
}

b8 PNSLR_CommitSharedMemoryChannelMessage(PNSLR_SharedMemoryChannelWriter* writer, PNSLR_SharedMemoryChannelReservedMessage reservedMessage)
{
    if (!writer) return false;
    if (reservedMessage.channel != writer) return false;

    PNSLR_SharedMemoryChannelMessageQueueHeader* mq = PNSLR_Internal_GetSMCMsgQueueHeader(writer->header);
    if (!mq) return false;

    u8* data = (u8*) writer->header;

    i64* magicNumPtr = (i64*) (data + reservedMessage.offset);
    i64* lengthPtr   = (i64*) (data + reservedMessage.offset + sizeof(i64));

    if (data + reservedMessage.offset + sizeof(i64) + sizeof(i64) != reservedMessage.writePtr) FORCE_DBG_TRAP;

    *magicNumPtr = PNSLR_INTERNAL_SMC_MESSAGE_MAGIC_NUM;
    *lengthPtr   = reservedMessage.size;

    mq->writeCursor = reservedMessage.offset + reservedMessage.size;

    return true;
}

b8 PNSLR_DisconnectSharedMemoryChannelWriter(PNSLR_SharedMemoryChannelWriter* writer)
{
    if (!writer) return false;

    #if PNSLR_WINDOWS
    {
        UnmapViewOfFile(writer->header);
        HANDLE h = PNSLR_Internal_BreakReadChannelHandle(writer->handle);
        if (h) CloseHandle(h);
        return true;
    }
    #elif PNSLR_LINUX || PNSLR_OSX
    {
        munmap(writer->header, (size_t) writer->header->fullMemRegionSize);
        i32 shm = PNSLR_Internal_BreakReadChannelHandle(writer->handle);
        if (shm) close(shm);
        return true;
    }
    #else
    {
        return false;
    }
    #endif
}

#undef PNSLR_INTERNAL_SMC_MSG_SYS_VERSION
#undef PNSLR_INTERNAL_SMC_MESSAGE_MAGIC_NUM
#undef PNSLR_INTERNAL_SMC_PLATFORM_HEADER_MAGIC_NUM
#undef PNSLR_INTERNAL_SMC_CHANNEL_HEADER_MAGIC_NUM
#undef PNSLR_INTERNAL_SMC_WRAPPING_SENTINEL
#undef PNSLR_INTERNAL_SMC_PER_MESSAGE_OVERHEAD_SIZE
#undef PNSLR_INTERNAL_OFFSET_TO_SMC_MSG_DATA
#undef PNSLR_INTERNAL_OFFSET_TO_SMC_MSG_QUEUE_HEADER
#undef PNSLR_INTERNAL_OFFSET_TO_SMC_HEADER_OS_SPECIFIC
