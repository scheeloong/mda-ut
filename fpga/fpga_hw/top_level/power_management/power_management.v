// Power management
//
// Allows the FPGA to turn on and off power, as well as monitor voltage levels

module power_management (
  output reg kill_sw,
  output reg [2:0] sel,
  input data,
  input start, /* Signal to turn on power */
  input clk /* 50 MHz */
);

  reg [9:0] wait_cnt = 10'd0; /* Overflow at ~49 KHz */
  reg state = 1'd0;

  always @(posedge clk)
  // Wait state (until start is high)
  if (state == 1'd0)
  begin
    kill_sw <= 1'b0;
    sel <= 3'b000;
    if (start)
    begin
      state <= 1'd1;
    end
  end
  else
  begin
    wait_cnt <= wait_cnt + 10'd1;
    if (wait_cnt == 10'd0)
    begin
      if (sel == 3'd6)
      begin
        kill_sw = 1'b1;
        sel <= 3'd000;
      end
      else
        sel <= sel + 3'b001;
      // Check input after waiting a ~49 KHz cycle
      // Power off if data is Low on an Even check or High on an Odd check
      if (!start || (wait_cnt == 10'd1023) &&
          ((data == 1'b0 && sel[0] == 1'b0)
          || data == 1'b1 && sel[0] == 1'b1))
      begin
        state = 1'd0;
      end
    end
  end

endmodule
