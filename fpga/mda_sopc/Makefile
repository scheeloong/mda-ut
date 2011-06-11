BSP_DIR=./bsp
APP_DIR=./app
ELF_NAME=$(APP_DIR)/main.elf
SOPC_FILE=mda.sopc
SOPCINFO_FILE=$(SOPC_FILE)info

all: $(ELF_NAME)

$(SOPCINFO_FILE):
	-sopc_builder --no_splash --generate -s mda.sopc
	rm -f mda_inst.v

bsp/settings.bsp: mda.sopcinfo
	nios2-bsp hal $(BSP_DIR) $(SOPCINFO_FILE) --default_stdio jtag_uart_0

$(ELF_NAME): bsp/settings.bsp $(APP_DIR)/src/main.c
	nios2-app-generate-makefile --bsp-dir $(BSP_DIR) --app-dir $(APP_DIR) --inc-rdir $(APP_DIR)/inc --src-rdir $(APP_DIR)/src
	make -C $(APP_DIR)

clean:
	rm -rf db mda_sim *.v *.ptf* *.mif *.html *.sdc *.bsf *.qip *.txt *.hex *.sopcinfo mda_generation_script .sopc_builder *.h *.ocp $(BSP_DIR) $(ELF_NAME) $(APP_DIR)/*.objdump $(APP_DIR)/*.map $(APP_DIR)/Makefile $(APP_DIR)/obj $(BSP_DIR)

.PHONY: all clean
