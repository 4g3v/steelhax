#include <3ds.h>
#include "../../constants/constants.h"

Result gspwn(void* dst, void* src, u32 size);
Result _GSPGPU_SetBufferSwap(Handle handle, u32 screenid, GSPGPU_FramebufferInfo framebufinfo);
Result _FSUSER_OpenArchive(Handle* handle, FS_Archive* archive);
//Result _FSUSER_OpenFile(Handle* handle, Handle* out, FS_Archive archive, FS_Path fileLowPath, u32 openflags, u32 attributes);
//Result _FSUSER_OpenFileDirectly(Handle* handle, Handle* out, FS_Archive archive, FS_Path fileLowPath, u32 openflags, u32 attributes);
Result _FSUSER_DeleteFile(Handle *handle, FS_Archive archive, FS_Path fileLowPath);
Result _FSFILE_Read(Handle handle, u32 *bytesRead, u64 offset, u32 *buffer, u32 size);
Result _FSFILE_GetSize(Handle handle, u64 *size);
Result _FSFILE_Write(Handle handle, u32 *bytesWritten, u64 offset, u32 *data, u32 size, u32 flushFlags);
Result _FSFILE_Close(Handle handle);
Result _FSUSER_OpenArchive(Handle *handle, FS_Archive *archive);
Result _FSUSER_CloseArchive(Handle* handle, FS_Archive* archive);
Result _FSUSER_ControlArchive(Handle *handle, FS_Archive archive, FS_ArchiveAction action, void* input, u32 inputSize, void* output, u32 outputSize);
Result _FSUSER_FormatSaveData(Handle* handle, FS_ArchiveID archiveId, FS_Path path, u32 blocks, u32 directories, u32 files, u32 directoryBuckets, u32 fileBuckets, bool duplicateData);
FS_Path _fsMakePath(FS_PathType type, const void* path);

int _strlen(const char* str);
void *memset(void * ptr, int value, size_t num);
int _memcmp(void *ptr1, void *ptr2, size_t num);