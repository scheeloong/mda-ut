module pwm(input CLOCK_50, output [7:0]LED, output [7:0]GPIO_0);

  reg [27:0] counter = 28'd0;

  always @(posedge CLOCK_50)
    counter = counter + 28'd1;

  motor_controller mc0(CLOCK_50, 1'b1, 1'b1, counter[27:18], LED[3:0]);
  motor_controller mc1(CLOCK_50, 1'b0, 1'b1, counter[27:18] + 10'd512, LED[7:4]);
  motor_controller mc2(CLOCK_50, 1'b1, 1'b1, 10'd512, {GPIO_0[7], GPIO_0[5], GPIO_0[3], GPIO_0[1]});
endmodule
