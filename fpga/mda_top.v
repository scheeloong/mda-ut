/*
 * This is the top-level module for the entire Quartus project.
 * This instantiates processor-independent modules and the
 * SOPC module, which contains NIOS and its Avalon slaves.
 */

module mda_top (input [3:0]SW, output [3:0]LED);
  // instantiate Switch to LED connection
  proc_independent_switch_led proc_ind(SW, LED);

  // instantiate SOPC module

endmodule

