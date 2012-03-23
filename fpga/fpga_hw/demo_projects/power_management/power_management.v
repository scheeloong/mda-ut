module power_management(input [3:0]SW, output [3:0]LED, inout [33:0]GPIO_1);
  wire [2:0] voltage_mux;

  // kill switch
  assign GPIO_1[33] = SW[0];
  // voltage muxes
  assign {GPIO_1[29], GPIO_1[31], GPIO_1[25]} = voltage_mux;
  assign voltage_mux = SW[3:1];

  // LEDs
  assign LED[3:1] = voltage_mux;
  assign LED[0] = GPIO_1[27];
endmodule
