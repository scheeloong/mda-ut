// This is the motor_controller for each H-bridge circuit
// Direction and on/off can be specified, as well as duty cycle
// Change the PWM frequency by modifying duty_counter's length in bits (and the padding in the comparison)
// There is also a dead time to ensure that the H-bridge doesn't toggle between two on states in 10 microseconds

module motor_controller (input clk, input dir, input on, input [4:0] duty_cycle, output reg [3:0] out);
  parameter dead_time = 9'd500; // 500 cycles = 10 microseconds

  reg [3:0] out_reg;
  reg [9:0] duty_counter = 10'd0;
  reg [8:0] dead_time_counter = dead_time;
  reg [1:0] prev_in = 2'b00;

  always @(posedge clk)
  begin
    if ({dir, on} != prev_in)
      dead_time_counter <= 9'd0;
    if (dead_time_counter != dead_time)
      dead_time_counter <= dead_time_counter + 9'd1;
    casex ({dir, on})
      2'bx0: out_reg <= 4'b0000;
      2'b11: out_reg <= 4'b1001;
      2'b01: out_reg <= 4'b0110;
    endcase
    prev_in <= {dir, on};
    duty_counter <= duty_counter + 4'd1;
    out <= ((duty_counter < {5'd0, duty_cycle}) && (dead_time_counter == dead_time)) ? out_reg : 4'd0;
  end

endmodule

