// If all switches are on, then rotate through 0-7 for the output.
// Otherwise, the output is controlled by the switches.
module power_management(input CLOCK_50, input [3:0]SW, output [3:0]LED, inout [33:0]GPIO_1);
  wire [2:0] voltage_mux;
  reg [28:0] counter = 29'd0;

  // kill switch
  assign GPIO_1[33] = SW[0];

  // voltage muxes
  assign {GPIO_1[29], GPIO_1[31], GPIO_1[25]} = voltage_mux;
  assign voltage_mux = (&SW[3:1]) ? counter[28:26] : SW[3:1];

  // LEDs
  assign LED[3:1] = voltage_mux;
  assign LED[0] = GPIO_1[27];

  always @(posedge CLOCK_50)
    counter = counter + 29'd1;
endmodule
