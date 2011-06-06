QUARTUS_DIR=./quartus
BSP_DIR=./bsp
APP_DIR=./app

SOPC_FILE=mda_cpu.sopc
SOPCINFO_FILE=$(SOPC_FILE)info
BSP_SETTINGS_FILE=mda_de2.bsp



all:
	(cd $(QUARTUS_DIR) && $(MAKE))
	$(MAKE) $(BSP_DIR)
	nios2-app-generate-makefile --bsp-dir $(BSP_DIR) --app-dir $(APP_DIR) --inc-rdir $(APP_DIR)/inc --src-rdir $(APP_DIR)/src
	(cd $(APP_DIR) && $(MAKE))

# This next command just creates the Board Support Pacakge from the sopc file
# Pretty basic command, it is actually pretty cool
$(BSP_DIR):$(QUARTUS_DIR)/$(SOPCINFO_FILE)
	nios2-bsp hal $(BSP_DIR) $(QUARTUS_DIR)/$(SOPCINFO_FILE) 

clean:
	(cd $(APP_DIR) && $(MAKE) $@)
	rm -rf $(BSP_DIR)
	(cd $(QUARTUS_DIR) && $(MAKE) $@)

