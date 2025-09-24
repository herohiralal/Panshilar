#define PNSLR_IMPLEMENTATION
#include "SharedMemoryChannel.h"
#include "Allocators.h"
#include "Memory.h"
#include "Strings.h"

// Internal constants
static const u32 PNSLR_INTERNAL_PROTOCOL_VERSION = 1;
static const u32 PNSLR_INTERNAL_CHANNEL_MAGIC = 0x436D6853; // "ShmC"
static const u32 PNSLR_INTERNAL_MESSAGE_MAGIC = 0x32303232; // "2022"
static const u64 PNSLR_INTERNAL_WRAPPING_SENTINEL = 0xcafebabef00dfaceULL;

static const u32 PNSLR_INTERNAL_OFFSET_TO_HEADER_OS_SPECIFIC = 64;
static const u32 PNSLR_INTERNAL_OFFSET_TO_MESSAGE_QUEUE_HEADER = 128;
static const u32 PNSLR_INTERNAL_OFFSET_TO_MESSAGE_DATA = 256;
static const u32 PNSLR_INTERNAL_PER_MESSAGE_OVERHEAD_SIZE = 16;

ENUM_START(PNSLR_Internal_ChannelStatus, u8)
    #define PNSLR_Internal_ChannelStatus_Disconnected     ((PNSLR_Internal_ChannelStatus) 0)
    #define PNSLR_Internal_ChannelStatus_Paused           ((PNSLR_Internal_ChannelStatus) 1)
    #define PNSLR_Internal_ChannelStatus_ActivelyReading  ((PNSLR_Internal_ChannelStatus) 2)
ENUM_END

typedef struct PNSLR_Internal_Header
{
    u32                           magic;
    u32                           version;
    PNSLR_Internal_ChannelStatus  readerStatus;
    PNSLR_Internal_ChannelStatus  writerStatus;
    u32                           offsetToOsSpecificHeader;
    u32                           offsetToMessageQueueHeader;
    u32                           offsetToMessageData;
    i64                           fullMemoryRegionSizeInBytes;
    i64                           dataSizeInBytes;
} PNSLR_Internal_Header;

typedef struct PNSLR_Internal_MessageQueueHeader
{
    i64 readCursor;
    u8  pad[56];
    i64 writeCursor;
} PNSLR_Internal_MessageQueueHeader;

typedef struct PNSLR_Internal_ChannelData
{
    PNSLR_Internal_Header* header;
    rawptr                 osHandle;
    rawptr                 mappedMemory;
    i64                    mappedSize;
} PNSLR_Internal_ChannelData;

typedef struct PNSLR_Internal_HeaderOsSpecific
{
    u32 magic;
} PNSLR_Internal_HeaderOsSpecific;

// Platform-specific functions
static b8 PNSLR_Internal_CreateChannelOs(utf8str name, i64 totalBytes, PNSLR_Internal_ChannelData* channelData);
static b8 PNSLR_Internal_ConnectToChannelOs(utf8str name, PNSLR_Internal_ChannelData* channelData);
static void PNSLR_Internal_ReleaseChannelOs(PNSLR_Internal_ChannelData* channelData);

// Helper functions
static PNSLR_Internal_MessageQueueHeader* PNSLR_Internal_GetMessageQueueHeader(PNSLR_Internal_Header* header)
{
    u8* base = (u8*)header;
    return (PNSLR_Internal_MessageQueueHeader*)(base + header->offsetToMessageQueueHeader);
}

static PNSLR_Internal_ChannelData* PNSLR_Internal_GetChannelData(u64 handle)
{
    return (PNSLR_Internal_ChannelData*) handle;
}

// Reader implementation
b8 PNSLR_CreateSharedMemoryChannelReader(utf8str name, i64 bytes, PNSLR_SharedMemoryChannelReader* reader)
{
    if (!reader) return false;

    PNSLR_Internal_ChannelData* channelData = PNSLR_New(PNSLR_Internal_ChannelData, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), nil);
    if (!channelData) return false;

    i64 totalBytes = bytes + PNSLR_INTERNAL_OFFSET_TO_MESSAGE_DATA;

    if (!PNSLR_Internal_CreateChannelOs(name, totalBytes, channelData))
    {
        PNSLR_Delete(channelData, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), nil);
        return false;
    }

    // Initialize header
    PNSLR_Internal_Header* header = channelData->header;

    header->magic = PNSLR_INTERNAL_CHANNEL_MAGIC;
    header->version = PNSLR_INTERNAL_PROTOCOL_VERSION;
    header->readerStatus = PNSLR_Internal_ChannelStatus_ActivelyReading;
    header->writerStatus = PNSLR_Internal_ChannelStatus_Disconnected;
    header->offsetToOsSpecificHeader = PNSLR_INTERNAL_OFFSET_TO_HEADER_OS_SPECIFIC;
    header->offsetToMessageQueueHeader = PNSLR_INTERNAL_OFFSET_TO_MESSAGE_QUEUE_HEADER;
    header->offsetToMessageData = PNSLR_INTERNAL_OFFSET_TO_MESSAGE_DATA;
    header->fullMemoryRegionSizeInBytes = totalBytes;
    header->dataSizeInBytes = bytes;

    // Initialize OS-specific header
    u8* data = (u8*)header;
    PNSLR_Internal_HeaderOsSpecific* headerOs = (PNSLR_Internal_HeaderOsSpecific*)(data + PNSLR_INTERNAL_OFFSET_TO_HEADER_OS_SPECIFIC);
    #if PNSLR_WINDOWS
    headerOs->magic = 0x57686D53; // "ShmW"
    #elif PNSLR_LINUX || PNSLR_OSX
    headerOs->magic = 0x50686D53; // "ShmP"
    #else
    headerOs->magic = 0x506D6853; // "ShmS"
    #endif

    // Initialize message queue
    PNSLR_Internal_MessageQueueHeader* mqh = PNSLR_Internal_GetMessageQueueHeader(header);
    mqh->readCursor = PNSLR_INTERNAL_OFFSET_TO_MESSAGE_DATA;
    mqh->writeCursor = PNSLR_INTERNAL_OFFSET_TO_MESSAGE_DATA;
    PNSLR_MemSet(mqh->pad, 0, sizeof(mqh->pad));

    reader->handle = (u64) channelData;
    return true;
}

b8 PNSLR_ReadSharedMemoryChannelMessage(PNSLR_SharedMemoryChannelReader* reader, PNSLR_SharedMemoryMessage* message, b8* fatalError)
{
    if (!reader || !message || !fatalError) return false;

    PNSLR_Internal_ChannelData* channelData = PNSLR_Internal_GetChannelData(reader->handle);
    if (!channelData || !channelData->header) return false;

    *fatalError = false;

    PNSLR_Internal_Header* header = channelData->header;
    PNSLR_Internal_MessageQueueHeader* mqh = PNSLR_Internal_GetMessageQueueHeader(header);

    i64 readCursor = mqh->readCursor;
    if (readCursor == mqh->writeCursor)
    {
        return false; // No messages available
    }

    u8* data = (u8*)header;
    u64* magicPtr = (u64*)(data + readCursor);
    i64* lengthPtr = (i64*)(data + readCursor + 8);

    u64 magic = *magicPtr;

    if (magic != PNSLR_INTERNAL_MESSAGE_MAGIC)
    {
        if (magic == PNSLR_INTERNAL_WRAPPING_SENTINEL)
        {
            // Wrap to beginning
            if (readCursor == header->offsetToMessageData)
            {
                *fatalError = true;
                return false; // Invalid sentinel at start
            }

            mqh->readCursor = header->offsetToMessageData;
            return PNSLR_ReadSharedMemoryChannelMessage(reader, message, fatalError);
        }

        *fatalError = true;
        return false; // Invalid magic
    }

    i64 length = *lengthPtr;
    if (length < PNSLR_INTERNAL_PER_MESSAGE_OVERHEAD_SIZE)
    {
        *fatalError = true;
        return false; // Invalid length
    }

    i64 end = header->offsetToMessageData + header->dataSizeInBytes;
    i64 remaining = end - readCursor;
    if (remaining < length)
    {
        *fatalError = true;
        return false; // Message too long
    }

    message->data = data + readCursor + PNSLR_INTERNAL_PER_MESSAGE_OVERHEAD_SIZE;
    message->size = length - PNSLR_INTERNAL_PER_MESSAGE_OVERHEAD_SIZE;
    message->internal = reader->handle;

    return true;
}

b8 PNSLR_AcknowledgeSharedMemoryChannelMessage(PNSLR_SharedMemoryMessage* message)
{
    if (!message) return false;

    PNSLR_Internal_ChannelData* channelData = PNSLR_Internal_GetChannelData(message->internal);
    if (!channelData || !channelData->header) return false;

    PNSLR_Internal_Header* header = channelData->header;
    PNSLR_Internal_MessageQueueHeader* mqh = PNSLR_Internal_GetMessageQueueHeader(header);

    i64 readCursor = mqh->readCursor;
    i64 totalBytes = message->size + PNSLR_INTERNAL_PER_MESSAGE_OVERHEAD_SIZE;

    readCursor += totalBytes;

    i64 end = header->offsetToMessageData + header->dataSizeInBytes;
    if (end - readCursor < PNSLR_INTERNAL_PER_MESSAGE_OVERHEAD_SIZE)
    {
        // Wrap to beginning
        readCursor = PNSLR_INTERNAL_OFFSET_TO_MESSAGE_DATA;
    }

    mqh->readCursor = readCursor;
    return true;
}

b8 PNSLR_DestroySharedMemoryChannelReader(PNSLR_SharedMemoryChannelReader* reader)
{
    if (!reader) return false;

    PNSLR_Internal_ChannelData* channelData = PNSLR_Internal_GetChannelData(reader->handle);
    if (!channelData) return false;

    PNSLR_Internal_ReleaseChannelOs(channelData);
    PNSLR_Delete(channelData, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), nil);

    reader->handle = 0;
    return true;
}

// Writer implementation
b8 PNSLR_TryConnectSharedMemoryChannelWriter(utf8str name, PNSLR_SharedMemoryChannelWriter* writer)
{
    if (!writer) return false;

    PNSLR_Internal_ChannelData* channelData = PNSLR_New(PNSLR_Internal_ChannelData, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), nil);
    if (!channelData) return false;

    if (!PNSLR_Internal_ConnectToChannelOs(name, channelData))
    {
        PNSLR_Delete(channelData, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), nil);
        return false;
    }

    // Verify header
    PNSLR_Internal_Header* header = channelData->header;
    if (header->magic != PNSLR_INTERNAL_CHANNEL_MAGIC ||
        header->version != PNSLR_INTERNAL_PROTOCOL_VERSION)
    {
        PNSLR_Internal_ReleaseChannelOs(channelData);
        PNSLR_Delete(channelData, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), nil);
        return false;
    }

    writer->handle = (u64) channelData;
    return true;
}

b8 PNSLR_PrepareSharedMemoryChannelMessage(PNSLR_SharedMemoryChannelWriter* writer, i64 bytes, PNSLR_SharedMemoryReservedMessage* reservedMessage)
{
    if (!writer || !reservedMessage) return false;

    PNSLR_Internal_ChannelData* channelData = PNSLR_Internal_GetChannelData(writer->handle);
    if (!channelData || !channelData->header) return false;

    i64 totalBytes = bytes + PNSLR_INTERNAL_PER_MESSAGE_OVERHEAD_SIZE;
    PNSLR_Internal_Header* header = channelData->header;
    PNSLR_Internal_MessageQueueHeader* mqh = PNSLR_Internal_GetMessageQueueHeader(header);

    i64 arenaStart = header->offsetToMessageData;
    i64 arenaEnd = arenaStart + header->dataSizeInBytes;

    i64 writeCursor = mqh->writeCursor;
    i64 readCursor = mqh->readCursor;
    i64 targetPosition = 0;
    i64 writeWrappingSentinelAt = 0;

    if (writeCursor >= readCursor)
    {
        // Test against end of buffer
        i64 remaining = arenaEnd - writeCursor;
        if (remaining >= totalBytes)
        {
            targetPosition = writeCursor;
        }
        else
        {
            // Need to wrap
            if (arenaEnd - writeCursor >= PNSLR_INTERNAL_PER_MESSAGE_OVERHEAD_SIZE)
            {
                writeWrappingSentinelAt = writeCursor;
            }
            writeCursor = header->offsetToMessageData;
            if (totalBytes >= header->dataSizeInBytes - 1) return false;
        }
    }

    if (!targetPosition)
    {
        // Test for space from beginning to read cursor
        i64 remaining = readCursor - writeCursor - 1;
        if (remaining >= totalBytes)
        {
            targetPosition = header->offsetToMessageData;
        }
        else
        {
            return false; // No space
        }
    }

    if (writeWrappingSentinelAt)
    {
        u8* dest = ((u8*)header) + writeWrappingSentinelAt;
        *((u64*)dest) = PNSLR_INTERNAL_WRAPPING_SENTINEL;
    }

    u8* dataStart = (u8*)header;
    reservedMessage->data = dataStart + writeCursor + PNSLR_INTERNAL_PER_MESSAGE_OVERHEAD_SIZE;
    reservedMessage->size = bytes;
    reservedMessage->internal = writer->handle | ((u64)writeCursor << 32);

    return true;
}

b8 PNSLR_CommitSharedMemoryChannelMessage(PNSLR_SharedMemoryChannelWriter* writer, PNSLR_SharedMemoryReservedMessage* reservedMessage)
{
    if (!writer || !reservedMessage) return false;

    PNSLR_Internal_ChannelData* channelData = PNSLR_Internal_GetChannelData(writer->handle);
    if (!channelData || !channelData->header) return false;

    i64 offset = (i64)(reservedMessage->internal >> 32);
    i64 totalBytes = reservedMessage->size + PNSLR_INTERNAL_PER_MESSAGE_OVERHEAD_SIZE;

    u8* data = (u8*)channelData->header;
    u64* magicPtr = (u64*)(data + offset);
    i64* lengthPtr = (i64*)(data + offset + 8);

    *magicPtr = PNSLR_INTERNAL_MESSAGE_MAGIC;
    *lengthPtr = totalBytes;

    PNSLR_Internal_MessageQueueHeader* mqh = PNSLR_Internal_GetMessageQueueHeader(channelData->header);
    mqh->writeCursor = offset + totalBytes;

    return true;
}

b8 PNSLR_DisconnectSharedMemoryChannelWriter(PNSLR_SharedMemoryChannelWriter* writer)
{
    if (!writer) return false;

    PNSLR_Internal_ChannelData* channelData = PNSLR_Internal_GetChannelData(writer->handle);
    if (!channelData) return false;

    PNSLR_Internal_ReleaseChannelOs(channelData);
    PNSLR_Delete(channelData, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), nil);

    writer->handle = 0;
    return true;
}

// Platform-specific implementations

#if PNSLR_WINDOWS

static b8 PNSLR_Internal_CreateChannelOs(utf8str name, i64 totalBytes, PNSLR_Internal_ChannelData* channelData)
{
    PNSLR_ArraySlice(u16) wName = PNSLR_UTF16FromUTF8WindowsOnly(name, PNSLR_GetAllocator_DefaultHeap());

    HANDLE mapping = CreateFileMappingW(INVALID_HANDLE_VALUE, nil, PAGE_READWRITE,
                                       (DWORD)(totalBytes >> 32), (DWORD)(totalBytes & 0xFFFFFFFF),
                                       wName.data);

    PNSLR_FreeSlice(&wName, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), nil);

    if (!mapping) return false;

    rawptr pointer = MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, (SIZE_T)totalBytes);
    if (!pointer)
    {
        CloseHandle(mapping);
        return false;
    }

    channelData->header = (PNSLR_Internal_Header*)pointer;
    channelData->osHandle = mapping;
    channelData->mappedMemory = pointer;
    channelData->mappedSize = totalBytes;

    return true;
}

static b8 PNSLR_Internal_ConnectToChannelOs(utf8str name, PNSLR_Internal_ChannelData* channelData)
{
    PNSLR_ArraySlice(u16) wName = PNSLR_UTF16FromUTF8WindowsOnly(name, PNSLR_GetAllocator_DefaultHeap());

    HANDLE mapping = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, wName.data);

    PNSLR_FreeSlice(&wName, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), nil);

    if (!mapping) return false;

    // First map just the header to get size
    rawptr headerPointer = MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, PNSLR_INTERNAL_OFFSET_TO_MESSAGE_DATA);
    if (!headerPointer)
    {
        CloseHandle(mapping);
        return false;
    }

    PNSLR_Internal_Header* header = (PNSLR_Internal_Header*)headerPointer;
    i64 actualBytes = header->fullMemoryRegionSizeInBytes;

    UnmapViewOfFile(headerPointer);

    // Now map the full size
    headerPointer = MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, (SIZE_T)actualBytes);
    if (!headerPointer)
    {
        CloseHandle(mapping);
        return false;
    }

    channelData->header = (PNSLR_Internal_Header*)headerPointer;
    channelData->osHandle = mapping;
    channelData->mappedMemory = headerPointer;
    channelData->mappedSize = actualBytes;

    return true;
}

static void PNSLR_Internal_ReleaseChannelOs(PNSLR_Internal_ChannelData* channelData)
{
    if (channelData->mappedMemory)
    {
        UnmapViewOfFile(channelData->mappedMemory);
    }
    if (channelData->osHandle)
    {
        CloseHandle((HANDLE) channelData->osHandle);
    }
}

#elif PNSLR_LINUX || PNSLR_OSX

static b8 PNSLR_Internal_CreateChannelOs(utf8str name, i64 totalBytes, PNSLR_Internal_ChannelData* channelData)
{
    cstring cName = PNSLR_CStringFromString(name, PNSLR_GetAllocator_DefaultHeap());
    i32 shm = shm_open(cName, O_CREAT | O_RDWR, 0777);
    PNSLR_FreeCString(cName, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), nil);
    if (shm == -1) return false;

    // Handle macOS ftruncate limitation
    b8 doTruncate = true;
    #if PNSLR_OSX
    struct stat memStat;
    if (fstat(shm, &memStat) == 0)
    {
        if (memStat.st_size != 0)
        {
            if (memStat.st_size >= totalBytes)
            {
                doTruncate = false;
            }
            else
            {
                close(shm);
                shm_unlink(cName);
                shm = shm_open(cName, O_CREAT | O_RDWR, 0777);
                if (shm == -1) return false;
            }
        }
    }
    #endif

    if (doTruncate)
    {
        if (ftruncate(shm, totalBytes) == -1)
        {
            close(shm);
            return false;
        }
    }

    i32 flags = MAP_SHARED;
    #if PNSLR_LINUX
    flags |= MAP_POPULATE;
    #endif

    rawptr pointer = mmap(nil, (size_t)totalBytes, PROT_READ | PROT_WRITE, flags, shm, 0);
    if (pointer == MAP_FAILED)
    {
        close(shm);
        return false;
    }

    channelData->header = (PNSLR_Internal_Header*)pointer;
    channelData->osHandle = (rawptr) (i64) shm;
    channelData->mappedMemory = pointer;
    channelData->mappedSize = totalBytes;

    return true;
}

static b8 PNSLR_Internal_ConnectToChannelOs(utf8str name, PNSLR_Internal_ChannelData* channelData)
{
    cstring cName = PNSLR_CStringFromString(name, PNSLR_GetAllocator_DefaultHeap());
    i32 shm = shm_open(cName, O_RDWR, 0);
    PNSLR_FreeCString(cName, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), nil);
    if (shm == -1) return false;

    // First map just the header
    rawptr headerPointer = mmap(nil, sizeof(PNSLR_Internal_Header), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
    if (headerPointer == MAP_FAILED)
    {
        close(shm);
        return false;
    }

    PNSLR_Internal_Header* header = (PNSLR_Internal_Header*)headerPointer;
    i64 actualBytes = header->fullMemoryRegionSizeInBytes;

    munmap(headerPointer, sizeof(PNSLR_Internal_Header));

    // Map full size
    headerPointer = mmap(nil, (size_t)actualBytes, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
    if (headerPointer == MAP_FAILED)
    {
        close(shm);
        return false;
    }

    channelData->header = (PNSLR_Internal_Header*)headerPointer;
    channelData->osHandle = (rawptr) (i64) shm;
    channelData->mappedMemory = headerPointer;
    channelData->mappedSize = actualBytes;

    return true;
}

static void PNSLR_Internal_ReleaseChannelOs(PNSLR_Internal_ChannelData* channelData)
{
    if (channelData->mappedMemory)
    {
        munmap(channelData->mappedMemory, (size_t) channelData->mappedSize);
    }
    if (channelData->osHandle)
    {
        close((i32) (i64) channelData->osHandle);
    }
}

#else // stub implementations

static b8 PNSLR_Internal_CreateChannelOs(utf8str name, i64 totalBytes, PNSLR_Internal_ChannelData* channelData) { return false; }

static b8 PNSLR_Internal_ConnectToChannelOs(utf8str name, PNSLR_Internal_ChannelData* channelData)
{
    return false;
}

static void PNSLR_Internal_ReleaseChannelOs(PNSLR_Internal_ChannelData* channelData) { }

#endif
