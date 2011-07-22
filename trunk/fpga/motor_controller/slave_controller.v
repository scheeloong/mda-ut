// This is the Avalon slave for the motor controller
// Registers 0-5 store the direction and on/off switch for each motor
// Registers 8-13 store the respective duty cycle
// The output should be fed to GPIO pins in the SOPC configuration

module slave_controller(input clk, input chipselect, input write, input [3:0]addr, input [31:0] writedata, output [23:0] GPIO_out);
  reg [11:0] in;
  reg [23:0] duty_cycle;
  
  always @(posedge clk)
    if (chipselect & write)
    casex (addr)
      4'b0000:
        in[1:0] <= writedata[1:0];
      4'b0001:
        in[3:2] <= writedata[1:0];
      4'b0010:
        in[5:4] <= writedata[1:0];
      4'b0011:
        in[7:6] <= writedata[1:0];
      4'b0100:
        in[9:8] <= writedata[1:0];
      4'b0101:
        in[11:10] <= writedata[1:0];
      4'b1000:
        duty_cycle[3:0] <= writedata[3:0];
      4'b1001:
        duty_cycle[7:4] <= writedata[3:0];
      4'b1010:
        duty_cycle[11:8] <= writedata[3:0];
      4'b1011:
        duty_cycle[15:12] <= writedata[3:0];
      4'b1100:
        duty_cycle[19:16] <= writedata[3:0];
      4'b1101:
        duty_cycle[23:20] <= writedata[3:0];
      default:
        ; // do nothing
    endcase

  generate
  genvar i;
    for (i=4; i<6; i=i+1)
      begin : motor_control_loop
        motor_controller mc(clk, in[i*2 + 1], in[i*2], duty_cycle[i*4+3:i*4], GPIO_out[i*4+3:i*4]);
      end
  endgenerate
endmodule
