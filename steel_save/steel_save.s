.nds

.loadtable "steel_save/unicode.tbl"

.create "save",0x0

; Save header
.orga 0x0 ; set output pointer
.word 0x00000000 ; checksum to fix

.word 0x0000001B
.word 0x00000027
.word 0x00000004 ; some values

.incbin "steel_save/keysAndValues.bin" ; some keys / values because I am too lazy to put them in here

.word 0xDEC0ADDE ; how often the loop iterates
.word 0xEFBEEFBE
.word 0xEFBEEFBE
.word 0xEFBEEFBE
.word 0xEFBEEFBE
.word 0xEFBEEFBE
.word 0xEFBEEFBE
.word 0x0FFFE5E8 ; stack location when the function clears

.incbin "build/initial_rop.bin" ; rop code, loop overrides pc after count being 0x07
.close