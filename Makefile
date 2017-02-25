all: save

setup:
	@mkdir -p build constants
	@python scripts/makeHeaders.py constants/constants FIRM_VERSION=OLD steel_ropdb/v1_ropdb.txt	

rop: setup
	@armips steel_save/steel_initial_rop.s
	@armips steel_save/steel_rop.s

save: rop
	@armips steel_save/steel_save.s
	@./SteelSaveTool.exe -rop build/initial_rop.bin save
	@./SteelSaveTool.exe -checksum save

sendall: save
	curl -T save ftp://192.168.0.213:5000/JKSV/Saves/Steel_Diver__Sub_Wars/1/save
	curl -T build/rop.bin ftp://192.168.0.213:5000/steelhax/rop.bin

sendrop: rop
	curl -T build/rop.bin ftp://192.168.0.213:5000/steelhax/rop.bin	

clean:
	@rm -r build constants