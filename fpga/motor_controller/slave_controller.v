// This is the Avalon slave for the motor controller
// Register 0 stores the direction and on/off switch for all 6 motors (12 bits)
// Register 1 stores the duty cycle
// The output should be fed to GPIO pins in the SOPC configuration

module slave_controller(input clk, input write, input addr, input [31:0] writedata, output [23:0] GPIO_out);
  reg [11:0] in;
  reg [4:0]  duty_cycle;

  always @(posedge clk)
    if (write & ~addr)
      in <= writedata[11:0];
    else if (write & addr)
      duty_cycle <= writedata[4:0];

  generate
  genvar i;
    for (i=0; i<6; i=i+1)
      begin : motor_control_loop
        motor_controller mc(clk, in[i*2 + 1], in[i*2], duty_cycle, GPIO_out[i*4+3:i*4]);
      end
  endgenerate
endmodule

