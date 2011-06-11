# Run 
# quartus_sh --help=makefiles
# to generate this file
###################################################################
# Project Configuration: 
# 
# Specify the name of the design (project), the Quartus II Settings
# File (.qsf), and the list of source files used.
###################################################################

PROJECT=mda_de2
SOPC_FILE=mda_cpu.sopc
SOPCINFO_FILE=$(SOPC_FILE)info
SOPCINFO_FILE_DEPENDS=$(SOPC_FILE)
SOURCE_FILES = mda_de2.v sdram_pll.v $(SOPCINFO_FILE)
ASSIGNMENT_FILES = $(PROJECT).qpf $(PROJECT).qsf 

###################################################################
# Main Targets
#
# all: build everything
# clean: remove output files and database
###################################################################

all: smart.log $(PROJECT).asm.rpt $(PROJECT).sta.rpt 

clean: clean_sopc
	rm -rf *.rpt *.chg smart.log *.htm *.eqn *.pin *.sof *.pof db incremental_db *.map.smsg *.jdi *.map.summary *.fit.summary *.sta.summary

clean_sopc:
	rm -rf ledg.v mda_cpu.bsf cpu_oci_test_bench.v cpu_rf_ram_a.mif cpu_rf_ram_b.mif ledr.v key.v mda_cpu.qip cpu_ic_tag_ram.mif cpu.ocp mda_cpu_inst.v sdram.v cpu_jtag_debug_module_wrapper.v mda_cpu.html mda_cpu.v cpu_jtag_debug_module_tck.v mda_cpu_sim jtag_uart.v mda_cpu.sopcinfo mda_cpu_log.txt cpu_test_bench.v cpu_mult_cell.v cpu.v hex03.v mda_cpu.ptf.bak sw.v cpu_bht_ram.mif cpu.sdc mda_cpu.ptf cpu_dc_tag_ram.mif hex47.v timer_0.v cpu_jtag_debug_module_sysclk.v mda_cpu.ptf.8.0 timer_1.v timer_2.v timer_3.v lcd.v mda_cpu_generation_script mda_cpu.ptf.pre_generation_ptf cpu_ociram_default_contents.mif sopc_builder_log.txt .sopc_builder

.PHONY: clean all

map: smart.log $(PROJECT).map.rpt
fit: smart.log $(PROJECT).fit.rpt
asm: smart.log $(PROJECT).asm.rpt
sta: smart.log $(PROJECT).sta.rpt
smart: smart.log

###################################################################
# Executable Configuration
###################################################################

MAP_ARGS = --family=CycloneII
FIT_ARGS = --part=EP2C35F672C6
ASM_ARGS =
STA_ARGS =

###################################################################
# Target implementations
###################################################################

STAMP = echo done >

$(SOPCINFO_FILE): $(SOPCINFO_FILE_DEPENDS)
	- sopc_builder --generate $(SOPC_FILE)

$(PROJECT).map.rpt: map.chg $(SOURCE_FILES) 
	quartus_map $(MAP_ARGS) $(PROJECT)
	$(STAMP) fit.chg

$(PROJECT).fit.rpt: fit.chg $(PROJECT).map.rpt
	quartus_fit $(FIT_ARGS) $(PROJECT)
	$(STAMP) asm.chg
	$(STAMP) sta.chg

$(PROJECT).asm.rpt: asm.chg $(PROJECT).fit.rpt
	quartus_asm $(ASM_ARGS) $(PROJECT)

$(PROJECT).sta.rpt: sta.chg $(PROJECT).fit.rpt
	quartus_sta $(STA_ARGS) $(PROJECT) 

smart.log: $(ASSIGNMENT_FILES)
	quartus_sh --determine_smart_action $(PROJECT) > smart.log

###################################################################
# Project initialization
###################################################################

$(ASSIGNMENT_FILES):
	quartus_sh --prepare $(PROJECT)

map.chg:
	$(STAMP) map.chg
fit.chg:
	$(STAMP) fit.chg
sta.chg:
	$(STAMP) sta.chg
asm.chg:
	$(STAMP) asm.chg


