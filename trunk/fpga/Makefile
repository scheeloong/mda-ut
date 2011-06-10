all: sopc_all proj compile

sopc_all:
	make -C mda_sopc

proj: sopc_all
	./config/generate_source_files.sh
	quartus_sh -t config/setup_proj.tcl

compile:
	quartus_sh --flow compile mda_top

sopc_clean:
	make clean -C mda_sopc

clean: sopc_clean
	rm -rf db incremental_db *.summary *.rpt *.done *.smsg *.qpf *.qsf config/source_files.tcl *.qdf *.sof *.pin *.jdi

.PHONY: all sopc_all proj compile sopc_clean clean
