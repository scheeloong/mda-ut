project_new mda_top -overwrite

# Target: DE0 Board
set_global_assignment -name FAMILY CycloneIV
set_global_assignment -name DEVICE AUTO
set_global_assignment -name TOP_LEVEL_ENTITY mda_top

source config/source_files.tcl

# Create timing assignments
create_base_clock -fmax "100 MHz" -target clk CLOCK_50

project_close
