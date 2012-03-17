// Power management Avalon slave

module power_management_slave (
  input clk,
  input chipselect,
  input write,
  input read,
  input [31:0] writedata,
  input data,
  output reg [31:0] readdata,
  output kill_sw,
  output [2:0] mux,
  output error
);

  reg start = 1'b0;

  always @(posedge clk)
    if (chipselect)
      if (write)
      begin
        start = writedata[0];
      end
      else if (read)
      begin
        readdata[2:0] = mux;
      end

power_management pm_inst (
  .kill_sw(kill_sw),
  .sel(mux),
  .data(data),
  .start(start),
  .clk(clk),
  .error(error)
);

endmodule
