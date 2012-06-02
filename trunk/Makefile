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
SIM_DIR=./sim
VISION_DIR=./vision
MOTORS_DIR=./motors

all: fpga vision motors sim

fpga:
	$(MAKE) -C $(FPGA_DIR)

vision:
	$(MAKE) -C $(VISION_DIR)

sim:
	$(MAKE) -C $(SIM_DIR)

motors:
	$(MAKE) -C $(MOTORS_DIR)

clean:
	$(MAKE) -C $(FPGA_DIR) $@
	$(MAKE) -C $(SIM_DIR) $@
	$(MAKE) -C $(VISION_DIR) $@
	$(MAKE) -C $(MISSION_DIR) $@

.PHONY: all clean fpga vision sim motors
