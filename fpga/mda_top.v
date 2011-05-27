/*
 * This is the top-level module for the entire Quartus project.
 * This instantiates processor-independent modules and the
 * SOPC module, which contains NIOS and its Avalon slaves.
 */

module mda_top (input [7:0]SW, output [7:0]LEDR);
  // instantiate SW to LEDR connection
  proc_independent_switch_led proc_ind(SW, LEDR);

  // instantiate SOPC module

endmodule

