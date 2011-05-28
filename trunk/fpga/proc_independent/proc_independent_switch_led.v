/*
 * Connect Switches to lower 4 Green LED's as a simple test program.
 */

module proc_independent_switch_led(SW, LED);

  parameter WIDTH = 4;

  input [WIDTH-1:0] SW;
  output [WIDTH-1:0] LED;

  assign LED = SW;

endmodule
