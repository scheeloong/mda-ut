// Power management
//
// Allows the FPGA to turn on and off power, as well as monitor voltage levels

module power_management (
  output reg kill_sw,
  output reg [2:0] sel,
  output error,
  input data,
  input start, /* Signal to turn on power */
  input clk /* 50 MHz */
);

  reg [9:0] wait_cnt; /* Overflow at ~49 KHz */
  reg error_reg;

  always @(posedge clk)
  // Wait state (until start is high)
  if (start == 1'd0)
  begin
    kill_sw <= 1'b0;
    sel <= 3'b111;
    wait_cnt = 10'd0;
    error_reg = 1'b0;
  end
  // Monitor voltage levels continuously
  else
  begin
    if (!error_reg)
      wait_cnt <= wait_cnt + 10'd1;
    if (!error_reg && wait_cnt == 10'd0)
    begin
      if (sel == 3'd6)
      begin
        // Power on when all voltage tests are successful
        kill_sw = 1'b1;
        sel <= 3'b000;
      end
      else
        sel <= sel + 3'b001;
    end
    // Check input after waiting a ~49 KHz cycle (ignore when sel is 111)
    // Power off if data is Low on an Even check or High on an Odd check
    if (&wait_cnt && !(&sel)
        && ((data == 1'b0 && sel[0] == 1'b0)
        || (data == 1'b1 && sel[0] == 1'b1)))
    begin
      error_reg <= 1'd1;
      kill_sw = 1'b0;
    end
  end

  assign error = error_reg;

endmodule
