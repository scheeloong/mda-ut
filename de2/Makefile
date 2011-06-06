QUARTUS_DIR=./quartus
BSP_DIR=./bsp
APP_DIR=./app

SOPC_FILE=mda_cpu.sopc
SOPCINFO_FILE=$(SOPC_FILE)info
BSP_SETTINGS_FILE=mda_de2.bsp

ELF_NAME=main.elf

GDB_TCP_PORT=2345



all:
	(cd $(QUARTUS_DIR) && $(MAKE))
	$(MAKE) $(BSP_DIR)
	nios2-app-generate-makefile --bsp-dir $(BSP_DIR) --app-dir $(APP_DIR) --inc-rdir $(APP_DIR)/inc --src-rdir $(APP_DIR)/src --elf-name $(ELF_NAME)
	(cd $(APP_DIR) && $(MAKE))

# This next command just creates the Board Support Pacakge from the sopc file
# Pretty basic command, it is actually pretty cool
$(BSP_DIR):$(QUARTUS_DIR)/$(SOPCINFO_FILE)
	nios2-bsp hal $(BSP_DIR) $(QUARTUS_DIR)/$(SOPCINFO_FILE) 


clean:
	(cd $(APP_DIR) && $(MAKE) $@)
	rm -rf $(BSP_DIR)
	(cd $(QUARTUS_DIR) && $(MAKE) $@)

# Alright the lines bellow help you get your code on the device
#
# You first need to configure the SOF file (I'm guessing this means software)
# make configure_sof
#
# Then you need to download your elf file
# You can use either 
#
# make download_elf_stop
# or
# make download_elf_go 
#
# I set the default to be stop because I think you will be debugging your 
# program with gdb at first
#
# After you do that, you need to start the gdb server
# you can do that with
# make gdb_server
#
# After you do that, you can do a 
# make gdb_client
#
# Which will start gdb
# I already put the line
# target remote localhost:2345
#
# in the .gdbinit file which is shared on svn, you need to do that to connect to the client
#
# Finally, I think if you want to interact with the terminal, you need to
# open a nios2-terminal
# you can open that with make terminal
configure_sof:
	nios2-configure-sof --directory $(QUARTUS_DIR)

download_elf:download_elf_stop

download_elf_stop:
	nios2-download --directory $(APP_DIR) -r --stop

download_elf_go:
	nios2-download --directory $(APP_DIR) -r --go

gdb_server:
	(cd $(APP_DIR) && nios2-gdb-server --tcpport $(GDB_TCP_PORT))

gdb_client:
	(cd $(APP_DIR) && nios2-elf-gdb $(ELF_NAME))

terminal:
	nios2-terminal

.PHONY:configure_sof download_elf download_elf_stop download_elf_go gdb_server gdb_client terminal
