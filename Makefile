#
# MDA Top level makefile
#
# This makefile can compile everything given that you have everything
# as explained to you on the wiki
#
# You should only have to change the name of the directories listed at the top 
# to change what you want to build
#
FPGA_DIR=./fpga
SOFTWARE_DIR=./software

all: vision sim fpga motors

fpga:
	$(MAKE) -C $(FPGA_DIR)

software:
	$(MAKE) -C $(SOFTWARE_DIR)

clean:
	$(MAKE) -C $(FPGA_DIR) $@
	$(MAKE) -C $(SOFTWARE_DIR) $@

.PHONY: all clean fpga vision sim motors
