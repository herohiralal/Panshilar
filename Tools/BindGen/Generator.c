#include "Generator.h"

void RunGenerator(GeneratorFn generator, PNSLR_Path tgtDir, utf8str subDirName, ParsedContent* content, PNSLR_Allocator allocator)
{
    PNSLR_ArenaAllocatorSnapshot snapshot = PNSLR_CaptureArenaAllocatorSnapshot(allocator);

    PNSLR_Path actualTgt = PNSLR_GetPathForSubdirectory(tgtDir, subDirName, allocator);
    if (PNSLR_PathExists(actualTgt, PNSLR_PathExistsCheckType_Directory)) { if (!PNSLR_DeletePath(actualTgt)) FORCE_DBG_TRAP; }
    if (!PNSLR_CreateDirectoryTree(actualTgt)) FORCE_DBG_TRAP;

    generator(actualTgt, content, allocator);

    PNSLR_ArenaSnapshotError err = PNSLR_RestoreArenaAllocatorSnapshot(&snapshot, PNSLR_GET_LOC());
    if (err != PNSLR_ArenaSnapshotError_None) FORCE_DBG_TRAP;
}
