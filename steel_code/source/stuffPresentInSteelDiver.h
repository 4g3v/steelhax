#include <3ds.h>

#include "../../constants/constants.h"
#define LINEAR_BUFFER ((u8*)0x14100000)
#define PAYLOAD_VA (0x00101000)

// handles
//static Handle* const srvHandle = (Handle*)STEEL_SRV_HANDLE;
static Handle* const fsHandle = (Handle*)STEEL_FSUSER_HANDLE;
static Handle* const dspHandle = (Handle*)STEEL_DSPDSP_HANDLE;
static Handle* const gspHandle = (Handle*)STEEL_GSPGPU_HANDLE;

// buffers
static u32** const sharedGspCmdBuf = (u32**)(STEEL_GSPGPU_INTERRUPT_RECEIVER_STRUCT + 0x58);

// functions
static Result (* const _GSPGPU_FlushDataCache)(Handle* handle, Handle kprocess, u32* addr, u32 size) = (void*)STEEL_GSPGPU_FLUSHDATACACHE;
static Result (* const _GSPGPU_GxTryEnqueue)(u32** sharedGspCmdBuf, u32* cmdAdr) = (void*)STEEL_GSPGPU_GXTRYENQUEUE;
static Result (* const _FSUSER_OpenFileDirectly)(Handle* handle, Handle* fileHandle, u32 transaction, u32 archiveId, u32 archivePathType, char* archivePath, u32 archivePathLength, u32 filePathType, char* filePath, u32 filePathLength, u8 openflags, u32 attributes) = (void*)STEEL_FSUSER_OPENFILEDIRECTLY;
//static Result (* const _FSUSER_OpenFile)(Handle* handle, Handle* fileHandle, u32 transaction, int unk, u64 archiveHandle, u32 filePathType, char* filePath, u32 filePathLength, u8 openflags, u32 attribute) = (void*)STEEL_FSUSER_OPENFILE;
static Result (* const _FSUSER_ReadFile)(Handle* handle, u32* bytesRead, s64 offset, void *buffer, u32 size) = (void*)STEEL_FSFILE_READ;
static Result (* const _DSPDSP_UnloadComponent)(Handle* handle) = (void*)STEEL_DSPDSP_UNLOADCOMPONENT;
static Result (* const _DSPDSP_RegisterInterruptEvents)(Handle* handle, Handle event, u32 type, u32 port) = (void*)STEEL_DSPDSP_REGISTERINTERRUPTEVENTS;
//static Result (* const _SRV_GetServiceHandle)(Handle* out, const char* name) = (void*)STEEL_SRV_GETSERVICEHANDLE;
static void (* const APT_FinalizeClientThread)() = (void*)STEEL_APT_FINALIZECLIENTTHREAD;
static void (* const _svcBreak)(u32 r0) = (void*)STEEL_SVC_BREAK;