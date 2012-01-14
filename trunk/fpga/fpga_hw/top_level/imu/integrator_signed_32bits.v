module integrator_signed_32bits(
   clk,
   reset_n,
   update_period,
   input_32,
   output_32
);

input         clk;
input         reset_n;
input [31:0]  update_period;
input [31:0]  input_32;
output [31:0] output_32;

reg [31:0] accumulator;
reg [31:0] counter_32_0;

assign output_32 = accumulator;

always @ ( posedge clk )
begin
   if ( !reset_n )
   begin
      accumulator <= 32'd0;
      counter_32_0 <= 32'd0;
   end
   else
   begin
      if ( counter_32_0 >= update_period )
      begin
         counter_32_0 <= 32'd0;
         accumulator <= accumulator + input_32;
      end
      else
      begin
         counter_32_0 <= counter_32_0 + 1'b1;
      end
   end
end

endmodule
