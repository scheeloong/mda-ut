all: proj compile

proj:
	./config/generate_source_files.sh
	quartus_sh -t config/setup_proj.tcl

compile:
	quartus_sh --flow compile mda_top

nios_all:
	make -C nios

nios_clean:
	make all -C nios

clean: nios_clean
	rm -rf db incremental_db *.summary *.rpt *.done *.smsg *.qpf *.qsf config/source_files.tcl *.qdf *.sof *.pin
