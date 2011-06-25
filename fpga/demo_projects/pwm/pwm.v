module pwm(input CLOCK_50, output [7:0]LED);

  reg [27:0] counter = 28'd0;

  always @(posedge CLOCK_50)
    counter = counter + 28'd1;

  motor_controller mc0(CLOCK_50, 1'b1, 1'b1, counter[27:23], LED[3:0]);
  motor_controller mc1(CLOCK_50, 1'b0, 1'b1, counter[27:23] + 5'd10000, LED[7:4]);
endmodule
