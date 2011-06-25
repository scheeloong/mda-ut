#
# MDA Top level makefile
#
# Created by: Mark Harfouche
#
# This makefile can compile everything given that you have everything
# as explained to you on the wiki
#
# You should only have to change the name of the directories listed at the top 
# to change what you want to build
#
FPGA_DIR=./de0nano
BSP_DIR=./bsp
NAV_DIR=./nav

VISION_DIR=./vision
JOYSTICK_DIR=./joystick


ELF_NAME=main.elf

GDB_TCP_PORT=2345



all:nav vision joystick

# This next command just creates the Board Support Pacakge from the sopc file
# Pretty basic command, it is actually pretty cool
bsp:fpga
	nios2-bsp hal $(BSP_DIR)  $(FPGA_DIR)

nav:bsp
	nios2-app-generate-makefile --bsp-dir $(BSP_DIR) --app-dir $(NAV_DIR) --inc-rdir $(NAV_DIR)/inc --src-rdir $(NAV_DIR)/src --elf-name $(ELF_NAME)
	$(MAKE) -C $(NAV_DIR)

fpga:
	$(MAKE) -C $(FPGA_DIR)

vision:

joystick:


clean:
	-$(MAKE) -k -C $(NAV_DIR) $@
	rm -f $(NAV_DIR)/Makfile
	rm -rf $(BSP_DIR)
	$(MAKE) -C $(FPGA_DIR) $@

.PHONY: all clean bsp nav fpga vision joystick

# Alright the lines below help you get your code on the device
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
	nios2-configure-sof --directory $(FPGA_DIR) || nios2-configure-sof --directory $(FPGA_DIR)

download_elf:download_elf_stop

download_elf_stop:
	nios2-download --accept-bad-sysid -r --stop --directory $(NAV_DIR) $(ELF_NAME)

download_elf_go:
	nios2-download --accept-bad-sysid -r --go   --directory $(NAV_DIR) $(ELF_NAME)

gdb_server:
	(cd $(NAV_DIR) && nios2-gdb-server --tcpport $(GDB_TCP_PORT))

gdb_client:
	(cd $(NAV_DIR) && nios2-elf-gdb $(ELF_NAME))

terminal:
	nios2-terminal

.PHONY:configure_sof download_elf download_elf_stop download_elf_go gdb_server gdb_client terminal
