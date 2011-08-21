// This is the motor_controller for each H-bridge circuit
// Direction and on/off can be specified, as well as duty cycle

`include "defines.v"

`define MAX_DC (1024*`MAX_DUTY_CYCLE_FRAC)

module motor_controller (input clk, input dir, input on, input [`DUTY_CYCLE_SIZE-1:0] duty_cycle, output reg [3:0] out);

  reg [3:0] out_reg;
  reg [`FREQ_NEG_POW-1:0] duty_counter = 0;
  reg [8:0] dead_time_counter = `DEAD_TIME;
  reg [1:0] prev_in = 2'b00;

  always @(posedge clk)
  begin
    if ({dir, on} != prev_in)
      dead_time_counter <= 9'd0;
    if (dead_time_counter != `DEAD_TIME)
      dead_time_counter <= dead_time_counter + 9'd1;
    casex ({dir, on})
      2'bx0: out_reg <= 4'b0000;
      2'b11: out_reg <= 4'b1001;
      2'b01: out_reg <= 4'b0110;
    endcase
    prev_in <= {dir, on};
    duty_counter <= duty_counter + `DUTY_CYCLE_SIZE'd1;
    out <= ((duty_counter[`FREQ_NEG_POW-1:`FREQ_NEG_POW-`DUTY_CYCLE_SIZE] < duty_cycle) && (duty_counter[`FREQ_NEG_POW-1:`FREQ_NEG_POW-`DUTY_CYCLE_SIZE] < `MAX_DC) && (dead_time_counter == `DEAD_TIME)) ? out_reg : 4'd0;
  end

endmodule

