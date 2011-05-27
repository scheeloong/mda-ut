#!/bin/sh

rm -f config/source_files.tcl

for file in $( find . -name '*.v' ); do
  echo set_global_assignment -name VERILOG_FILE $file >> config/source_files.tcl
done
