#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <3ds.h>
#include <3ds/ipc.h>
#include <3ds/result.h>
#include <3ds/services/gspgpu.h>

#include "stuffPresentInSteelDiver.h"

void *memset(void * ptr, int value, size_t num)
{
    u8 *p = ptr;
    while (num) {
        *p++ = value;
        num--;
    }
    return ptr;
}

int _memcmp(void *ptr1, void *ptr2, size_t num)
{
    for(; num--; ptr1++, ptr2++)
        if(*(u8*)ptr1 != *(u8*)ptr2)
            return *(u8*)ptr1-*(u8*)ptr2;
            
    return 0;
}

void *memcpy(void *destination, const void *source, size_t num)
{
    u8 *dest = destination;
    u8 *src  = (u8*)source;
    while (num) {
        *dest++ = *src++;
        num--;
    }
    return destination;
}

int _strlen(const char* str)
{
    int l=0;
    while(*(str++))l++;
    return l;
}

char *strcpy(char *dest, const char *src)
{
   char *save = dest;
   while((*dest++ = *src++));
   return save;
}

Result gspwn(void* dst, void* src, u32 size)
{
    u32 gxCommand[]=
    {
        0x00000004, //command header (SetTextureCopy)
        (u32)src, //source address
        (u32)dst, //destination address
        size, //size
        0xFFFFFFFF, // dim in
        0xFFFFFFFF, // dim out
        0x00000008, // flags
        0x00000000, //unused
    };

    return _GSPGPU_GxTryEnqueue(sharedGspCmdBuf, gxCommand);
}

Result _GSPGPU_SetBufferSwap(Handle handle, u32 screenid, GSPGPU_FramebufferInfo framebufinfo)
{
    Result ret=0;
    u32 *cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = 0x00050200;
    cmdbuf[1] = screenid;
    memcpy(&cmdbuf[2], &framebufinfo, sizeof(GSPGPU_FramebufferInfo));

    if((ret=svcSendSyncRequest(handle)))return ret;

    return cmdbuf[1];
}

FS_Path _fsMakePath(FS_PathType type, const void* path)
{
    FS_Path p = { type, 0, path };
    switch (type)
    {
        case PATH_ASCII:
            p.size = _strlen((const char*)path)+1;
            break;
        case PATH_UTF16:
        {
            const u16* str = (const u16*)path;
            while (*str++) p.size++;
            p.size = (p.size+1)*2;
            break;
        }
        case PATH_EMPTY:
            p.size = 1;
            p.data = "";
        default:
            break;
    }
    return p;
}

Result _FSUSER_OpenArchive(Handle *handle, FS_Archive *archive)
{
    if(!archive)
        return -2;

    u32 *cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = 0x080C00C2;
    cmdbuf[1] = archive->id;
    cmdbuf[2] = archive->lowPath.type;
    cmdbuf[3] = archive->lowPath.size;
    cmdbuf[4] = (archive->lowPath.size << 14) | 0x2;
    cmdbuf[5] = (u32)archive->lowPath.data;

    Result ret = 0;
    if((ret = svcSendSyncRequest(*handle)))
        return ret;

    archive->handle  = cmdbuf[2] | ((u64)cmdbuf[3] << 32);

    return cmdbuf[1];
}

Result _FSFILE_Close(Handle handle)
{
    u32* cmdbuf=getThreadCommandBuffer();

    cmdbuf[0]=0x08080000;

    Result ret=0;
    if((ret=svcSendSyncRequest(handle)))return ret;

    return cmdbuf[1];
}

Result _FSFILE_Read(Handle handle, u32 *bytesRead, u64 offset, u32 *buffer, u32 size)
{
    u32 *cmdbuf=getThreadCommandBuffer();

    cmdbuf[0]=0x080200C2;
    cmdbuf[1]=(u32)offset;
    cmdbuf[2]=(u32)(offset>>32);
    cmdbuf[3]=size;
    cmdbuf[4]=(size<<4)|12;
    cmdbuf[5]=(u32)buffer;

    Result ret=0;
    if((ret=svcSendSyncRequest(handle)))return ret;

    if(bytesRead)*bytesRead=cmdbuf[2];

    return cmdbuf[1];
}

Result _FSFILE_GetSize(Handle handle, u64 *size)
{
    u32 *cmdbuf=getThreadCommandBuffer();

    cmdbuf[0] = 0x08040000;

    Result ret=0;
    if((ret=svcSendSyncRequest(handle)))return ret;

    if(size)*size = *((u64*)&cmdbuf[2]);

    return cmdbuf[1];
}

Result _FSFILE_Write(Handle handle, u32 *bytesWritten, u64 offset, u32 *data, u32 size, u32 flushFlags)
{
    u32 *cmdbuf=getThreadCommandBuffer();

    cmdbuf[0]=0x08030102;
    cmdbuf[1]=(u32)offset;
    cmdbuf[2]=(u32)(offset>>32);
    cmdbuf[3]=size;
    cmdbuf[4]=flushFlags;
    cmdbuf[5]=(size<<4)|10;
    cmdbuf[6]=(u32)data;

    Result ret=0;
    if((ret=svcSendSyncRequest(handle)))return ret;

    if(bytesWritten)*bytesWritten=cmdbuf[2];

    return cmdbuf[1];
}

Result _FSUSER_DeleteFile(Handle     *handle,
                  FS_Archive archive,
                  FS_Path    fileLowPath)
{
    u32 *cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = 0x08040142;
    cmdbuf[1] = 0;
    cmdbuf[2]=(u32) archive.handle;
    cmdbuf[3]=(u32) (archive.handle >> 32);
    cmdbuf[4] = fileLowPath.type;
    cmdbuf[5] = fileLowPath.size;
    cmdbuf[6] = (fileLowPath.size << 14) | 0x2;
    cmdbuf[7] = (u32)fileLowPath.data;

    Result ret = 0;
    if((ret = svcSendSyncRequest(*handle)))
        return ret;

    return cmdbuf[1];
}

Result _FSUSER_ControlArchive(Handle *handle, FS_Archive archive, FS_ArchiveAction action, void* input, u32 inputSize, void* output, u32 outputSize)
{
    u32* cmdbuf=getThreadCommandBuffer();

    cmdbuf[0]=0x080d0144;
    cmdbuf[1]=(u32) archive.handle;
    cmdbuf[2]=(u32) (archive.handle >> 32);
    cmdbuf[3]=action;
    cmdbuf[4]=inputSize;
    cmdbuf[5]=outputSize;
    cmdbuf[6]=(inputSize << 4) | 0x8 | 0x2;
    cmdbuf[7]=(u32)input;
    cmdbuf[8]=(outputSize << 4) | 0x8 | 0x4;
    cmdbuf[9]=(u32)output;

    Result ret=0;
    if((ret=svcSendSyncRequest(*handle)))return ret;

    return cmdbuf[1];
}

Result _FSUSER_FormatSaveData(Handle* handle, FS_ArchiveID archiveId, FS_Path path, u32 blocks, u32 directories, u32 files, u32 directoryBuckets, u32 fileBuckets, bool duplicateData)
{
    u32 *cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = 0x084c0242;
    cmdbuf[1] = archiveId;
    cmdbuf[2] = path.type;
    cmdbuf[3] = path.size;
    cmdbuf[4] = blocks;
    cmdbuf[5] = directories;
    cmdbuf[6] = files;
    cmdbuf[7] = directoryBuckets;
    cmdbuf[8] = fileBuckets;
    cmdbuf[9] = duplicateData;
    cmdbuf[10] = (path.size << 14) | 0x2;
    cmdbuf[11] = (u32) path.data;

    Result ret=0;
    if((ret=svcSendSyncRequest(*handle)))return ret;

    return cmdbuf[1];
}

Result _FSUSER_CloseArchive(Handle* handle, FS_Archive* archive)
{
    if(!archive)return -2;
    u32* cmdbuf=getThreadCommandBuffer();

    cmdbuf[0]=0x080E0080;
    cmdbuf[1]=(u32) archive->handle;
    cmdbuf[2]=(u32) (archive->handle >> 32);

    Result ret=0;
    if((ret=svcSendSyncRequest(*handle)))return ret;

    return cmdbuf[1];
}