module gyro_lookup_table(input_num,output_num);
input  [31:0] input_num;
output [31:0] output_num;


divider_signed_32 divider_signed_32(
	.denom(32'd133),
	.numer(input_num),
	.quotient(output_num),
	.remain());
endmodule
