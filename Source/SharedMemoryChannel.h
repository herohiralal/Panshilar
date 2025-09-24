#ifndef PNSLR_SHARED_MEMORY_CHANNEL_H // ===========================================
#define PNSLR_SHARED_MEMORY_CHANNEL_H
#include "__Prelude.h"
EXTERN_C_BEGIN

/**
 * Represents a shared memory channel reader that creates and owns the shared memory segment.
 */
typedef struct PNSLR_SharedMemoryChannelReader
{
    u64 handle;
} PNSLR_SharedMemoryChannelReader;

/**
 * Represents a shared memory channel writer that connects to an existing shared memory segment.
 */
typedef struct PNSLR_SharedMemoryChannelWriter
{
    u64 handle;
} PNSLR_SharedMemoryChannelWriter;

/**
 * Represents a message that has been read from a shared memory channel.
 */
typedef struct PNSLR_SharedMemoryMessage
{
    rawptr data;
    i64    size;
    u64    internal;
} PNSLR_SharedMemoryMessage;

/**
 * Represents a reserved message slot for writing to a shared memory channel.
 */
typedef struct PNSLR_SharedMemoryReservedMessage
{
    rawptr data;
    i64    size;
    u64    internal;
} PNSLR_SharedMemoryReservedMessage;

/**
 * Creates a shared memory channel reader with the specified name and size.
 * The reader owns the shared memory segment and other processes can connect as writers.
 */
b8 PNSLR_CreateSharedMemoryChannelReader(utf8str name, i64 bytes, PNSLR_SharedMemoryChannelReader* reader);

/**
 * Polls for a message from the shared memory channel.
 * Returns true if a message was found, false otherwise.
 * Sets fatalError to true if an unrecoverable error occurred.
 */
b8 PNSLR_ReadSharedMemoryChannelMessage(PNSLR_SharedMemoryChannelReader* reader, PNSLR_SharedMemoryMessage* message, b8* fatalError);

/**
 * Acknowledges that a message has been processed and advances the read cursor.
 */
b8 PNSLR_AcknowledgeSharedMemoryChannelMessage(PNSLR_SharedMemoryMessage* message);

/**
 * Destroys a shared memory channel reader and releases all associated resources.
 */
b8 PNSLR_DestroySharedMemoryChannelReader(PNSLR_SharedMemoryChannelReader* reader);

/**
 * Attempts to connect to an existing shared memory channel as a writer.
 * Returns true if successful, false if the channel doesn't exist or connection failed.
 */
b8 PNSLR_TryConnectSharedMemoryChannelWriter(utf8str name, PNSLR_SharedMemoryChannelWriter* writer);

/**
 * Reserves space for a message in the shared memory channel.
 * Returns true if space was available, false otherwise.
 */
b8 PNSLR_PrepareSharedMemoryChannelMessage(PNSLR_SharedMemoryChannelWriter* writer, i64 bytes, PNSLR_SharedMemoryReservedMessage* reservedMessage);

/**
 * Commits a previously reserved message to the shared memory channel.
 */
b8 PNSLR_CommitSharedMemoryChannelMessage(PNSLR_SharedMemoryChannelWriter* writer, PNSLR_SharedMemoryReservedMessage* reservedMessage);

/**
 * Disconnects from a shared memory channel and releases writer resources.
 */
b8 PNSLR_DisconnectSharedMemoryChannelWriter(PNSLR_SharedMemoryChannelWriter* writer);

EXTERN_C_END
#endif // PNSLR_SHARED_MEMORY_CHANNEL_H ============================================
