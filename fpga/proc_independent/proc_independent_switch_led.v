/*
 * Connect Switches to red LED's as a simple test program.
 */

module proc_independent_switch_led(SW, LEDR);

  parameter WIDTH = 8;

  input [WIDTH-1:0] SW;
  output [WIDTH-1:0] LEDR;

  assign LEDR = SW;

endmodule
