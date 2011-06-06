BSP_DIR=./bsp
QUARTUS_DIR=./quartus
SOPC_FILE=mda_cpu.sopc
SOPCINFO_FILE=$(SOPC_FILE)info
BSP_SETTINGS_FILE=mda_de2.bsp



all:
	(cd $(QUARTUS_DIR) && $(MAKE))
	nios2-bsp hal $(BSP_DIR) $(QUARTUS_DIR)/$(SOPCINFO_FILE) 
