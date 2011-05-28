all: sopc_all proj compile nios_all

sopc_all:
	make -C mda_sopc

sopc_clean:
	make clean -C mda_sopc

proj:
	./config/generate_source_files.sh
	quartus_sh -t config/setup_proj.tcl

compile:
	quartus_sh --flow compile mda_top

nios_all:
	make -C nios

nios_clean:
	make clean -C nios

clean: nios_clean sopc_clean
	rm -rf db incremental_db *.summary *.rpt *.done *.smsg *.qpf *.qsf config/source_files.tcl *.qdf *.sof *.pin *.jdi
