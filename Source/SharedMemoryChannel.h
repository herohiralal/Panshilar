#ifndef PNSLR_SHARED_MEMORY_CHANNEL_H // ===========================================
#define PNSLR_SHARED_MEMORY_CHANNEL_H
#include "__Prelude.h"
EXTERN_C_BEGIN

typedef struct PNSLR_SharedMemoryChannelReader
{
    u64 handle;
} PNSLR_SharedMemoryChannelReader;

typedef struct PNSLR_SharedMemoryChannelWriter
{
    u64 handle;
} PNSLR_SharedMemoryChannelWriter;

typedef struct PNSLR_SharedMemoryMessage
{
    rawptr data;
    i64    size;
    u64    internal;
} PNSLR_SharedMemoryMessage;

typedef struct PNSLR_SharedMemoryReservedMessage
{
    rawptr data;
    i64    size;
    u64    internal;
} PNSLR_SharedMemoryReservedMessage;

b8 PNSLR_CreateSharedMemoryChannelReader(utf8str name, i64 bytes, PNSLR_SharedMemoryChannelReader* reader);
b8 PNSLR_ReadSharedMemoryChannelMessage(PNSLR_SharedMemoryChannelReader* reader, PNSLR_SharedMemoryMessage* message, b8* fatalError);
b8 PNSLR_AcknowledgeSharedMemoryChannelMessage(PNSLR_SharedMemoryMessage* message);
b8 PNSLR_DestroySharedMemoryChannelReader(PNSLR_SharedMemoryChannelReader* reader);

b8 PNSLR_TryConnectSharedMemoryChannelWriter(utf8str name, PNSLR_SharedMemoryChannelWriter* writer);
b8 PNSLR_PrepareSharedMemoryChannelMessage(PNSLR_SharedMemoryChannelWriter* writer, i64 bytes, PNSLR_SharedMemoryReservedMessage* reservedMessage);
b8 PNSLR_CommitSharedMemoryChannelMessage(PNSLR_SharedMemoryChannelWriter* writer, PNSLR_SharedMemoryReservedMessage* reservedMessage);
b8 PNSLR_DisconnectSharedMemoryChannelWriter(PNSLR_SharedMemoryChannelWriter* writer);

EXTERN_C_END
#endif // PNSLR_SHARED_MEMORY_CHANNEL_H ============================================
