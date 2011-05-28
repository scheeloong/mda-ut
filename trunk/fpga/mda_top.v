/*
 * This is the top-level module for the entire Quartus project.
 * This instantiates processor-independent modules and the
 * SOPC module, which contains NIOS and its Avalon slaves.
 */

module mda_top (input [3:0]SW, input CLOCK_50, input [0:0]KEY, output [3:0]LED);
  // instantiate Switch to LED connection
  proc_independent_switch_led proc_ind(SW, LED);

  // instantiate SOPC module
  mda mda_inst (CLOCK_50, KEY[0]);

endmodule

