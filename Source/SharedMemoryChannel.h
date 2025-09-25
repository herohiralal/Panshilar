#ifndef PNSLR_SHARED_MEMORY_CHANNEL_H // ===========================================
#define PNSLR_SHARED_MEMORY_CHANNEL_H
#include "__Prelude.h"
EXTERN_C_BEGIN

// Types ===========================================================================

/**
 * Opaque handle for a shared memory channel.
 */
typedef struct PNSLR_SharedMemoryChannelHandle { i64 handle; } PNSLR_SharedMemoryChannelHandle;

/**
 * Platform-specific header for a shared memory channel.
 */
typedef struct PNSLR_SharedMemoryChannelPlatformHeader { u32 magicNum; } PNSLR_SharedMemoryChannelPlatformHeader;

/**
 * Represents the status of a shared memory channel endpoint (reader or writer).
 */
ENUM_START(PNSLR_SharedMemoryChannelStatus, u8)
    #define PNSLR_SharedMemoryChannelStatus_Disconnected ((PNSLR_SharedMemoryChannelStatus) 0)
    #define PNSLR_SharedMemoryChannelStatus_Paused       ((PNSLR_SharedMemoryChannelStatus) 1)
    #define PNSLR_SharedMemoryChannelStatus_Active       ((PNSLR_SharedMemoryChannelStatus) 2)
ENUM_END

/**
 * Header for a shared memory channel, containing metadata about the channel.
 */
typedef struct PNSLR_SharedMemoryChannelHeader
{
    u32                             magicNum;
    u32                             version;
    PNSLR_SharedMemoryChannelStatus readerStatus;
    PNSLR_SharedMemoryChannelStatus writerStatus;
    u32                             offsetToOsSpecificHeader; // offsets from start of header
    u32                             offsetToMsgQueueHeader;   // offsets from start of header
    u32                             offsetToMsgData;          // offsets from start of header
    i64                             fullMemRegionSize;        // size of full memory region, from start of headaer to end of contiguous mem
    i64                             dataSize;                 // size of data area (sum of all msg sizes that can be queued at once)
} PNSLR_SharedMemoryChannelHeader;

/**
 * Header for the message queue within a shared memory channel.
 */
typedef struct PNSLR_SharedMemoryChannelMessageQueueHeader
{
    i64 readCursor;  // offset of next data to read from start of main header
    u8  padding[56]; // pad to 1 cacheline
    i64 writeCursor; // offset of next data to write from start of main header
} PNSLR_SharedMemoryChannelMessageQueueHeader;

/**
 * Represents a reader endpoint for a shared memory channel.
 */
typedef struct PNSLR_SharedMemoryChannelReader
{
    PNSLR_SharedMemoryChannelHeader* header;
    PNSLR_SharedMemoryChannelHandle  handle;
} PNSLR_SharedMemoryChannelReader;

/**
 * Represents a writer endpoint for a shared memory channel.
 */
typedef struct PNSLR_SharedMemoryChannelWriter
{
    PNSLR_SharedMemoryChannelHeader* header;
    PNSLR_SharedMemoryChannelHandle  handle;
} PNSLR_SharedMemoryChannelWriter;

/**
 * Represents a reserved message slot for writing to a shared memory channel.
 */
typedef struct PNSLR_SharedMemoryChannelReservedMessage
{
    PNSLR_SharedMemoryChannelWriter* channel;
    i64                              offset;
    i64                              size;         // includes message overhead
    u8*                              writePtr; // write here
} PNSLR_SharedMemoryChannelReservedMessage;

/**
 * Represents a message that has been read from a shared memory channel.
 */
typedef struct PNSLR_SharedMemoryChannelMessage
{
    PNSLR_SharedMemoryChannelReader* channel;
    i64                              offset;
    i64                              size;    // includes message overhead
    u8*                              readPtr; // read from here
    i64                              readSize;
} PNSLR_SharedMemoryChannelMessage;

// Reader Interface ================================================================

/**
 * Creates a shared memory channel reader with the specified name and size.
 * The reader owns the shared memory segment and other processes can connect as writers.
 */
b8 PNSLR_CreateSharedMemoryChannelReader(
    utf8str                          name,
    i64                              size,
    PNSLR_SharedMemoryChannelReader* reader
);

/**
 * Polls for a message from the shared memory channel.
 * Returns true if a message was found, false otherwise.
 * Sets fatalError to true if an unrecoverable error occurred.
 */
b8 PNSLR_ReadSharedMemoryChannelMessage(
    PNSLR_SharedMemoryChannelReader*  reader,
    PNSLR_SharedMemoryChannelMessage* message,
    b8*                               fatalError OPT_ARG
);

/**
 * Acknowledges that a message has been processed and advances the read cursor.
 */
b8 PNSLR_AcknowledgeSharedMemoryChannelMessage(
    PNSLR_SharedMemoryChannelMessage* message
);

/**
 * Destroys a shared memory channel reader and releases all associated resources.
 */
b8 PNSLR_DestroySharedMemoryChannelReader(
    PNSLR_SharedMemoryChannelReader* reader
);

// Writer Interface ================================================================

/**
 * Attempts to connect to an existing shared memory channel as a writer.
 * Returns true if successful, false if the channel doesn't exist or connection failed.
 */
b8 PNSLR_TryConnectSharedMemoryChannelWriter(
    utf8str                          name,
    PNSLR_SharedMemoryChannelWriter* writer
);

/**
 * Reserves space for a message in the shared memory channel.
 * Returns true if space was available, false otherwise.
 */
b8 PNSLR_PrepareSharedMemoryChannelMessage(
    PNSLR_SharedMemoryChannelWriter*          writer,
    i64                                       size,
    PNSLR_SharedMemoryChannelReservedMessage* reservedMessage
);

/**
 * Commits a previously reserved message to the shared memory channel.
 */
b8 PNSLR_CommitSharedMemoryChannelMessage(
    PNSLR_SharedMemoryChannelWriter*          writer,
    PNSLR_SharedMemoryChannelReservedMessage  reservedMessage
);

/**
 * Disconnects from a shared memory channel and releases writer resources.
 */
b8 PNSLR_DisconnectSharedMemoryChannelWriter(
    PNSLR_SharedMemoryChannelWriter* writer
);

EXTERN_C_END
#endif // PNSLR_SHARED_MEMORY_CHANNEL_H ============================================
