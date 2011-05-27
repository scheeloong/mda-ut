all: proj compile

proj:
	./config/generate_source_files.sh
	quartus_sh -t config/setup_proj.tcl

compile:
	quartus_sh --flow compile mda_top

clean:
	rm -rf db incremental_db *.summary *.rpt *.done *.smsg *.qpf *.qsf config/source_files.tcl
