// Power management
//
// If any of the shutdown inputs goes high, all GPIO outputs will be set to 0

module power_management #(
  parameter NUM_IN = 1,
  parameter NUM_IOS = 1
) (
  input clk,
  input [NUM_IN-1:0] shutdown,
  input [NUM_IOS-1:0] gpio_in,
  output reg [NUM_IOS-1:0] gpio_out
);

  always @(posedge clk)
    if (|shutdown)
      // Set all GPIO outputs to low
      gpio_out = {NUM_IOS{1'b0}};
    else
      // Set all GPIO outputs to the input
      gpio_out = gpio_in;

endmodule
