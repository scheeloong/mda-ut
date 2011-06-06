module mda_de2(
	CLOCK_50, 
	SW, KEY, 
	LEDG, LEDR, 
	HEX0, HEX1, HEX2, HEX3, HEX4, HEX5, HEX6, HEX7,
	LCD_EN, LCD_RS, LCD_RW, LCD_DATA,
	DRAM_ADDR, DRAM_BA_0, DRAM_BA_1, DRAM_CAS_N, DRAM_CKE, 
	DRAM_CLK, DRAM_CS_N, DRAM_DQ, 
	DRAM_LDQM, DRAM_UDQM, DRAM_RAS_N, DRAM_WE_N
	);
input CLOCK_50;
input [17:0] SW;
input [3:0] KEY;
output [17:0] LEDR;
output [7:0] LEDG;
output[6:0] HEX0;
output[6:0] HEX1;
output[6:0] HEX2;
output[6:0] HEX3;
output[6:0] HEX4;
output[6:0] HEX5;
output[6:0] HEX6;
output[6:0] HEX7;

output LCD_EN;
output LCD_RS;
output LCD_RW;
inout [7:0] LCD_DATA;

output [11:0] DRAM_ADDR;
output DRAM_BA_0, DRAM_BA_1;
output DRAM_CAS_N;
output DRAM_CKE;
output DRAM_CS_N;
inout  [15:0] DRAM_DQ;
output DRAM_LDQM, DRAM_UDQM;
output DRAM_RAS_N;
output DRAM_WE_N;
output DRAM_CLK;

wire [31:0] HEX03;
wire [31:0] HEX47;

assign HEX0[6:0] = HEX03[ 6: 0];
assign HEX1[6:0] = HEX03[14: 8];
assign HEX2[6:0] = HEX03[22:16];
assign HEX3[6:0] = HEX03[30:24];

assign HEX4[6:0] = HEX47[ 6: 0];
assign HEX5[6:0] = HEX47[14: 8];
assign HEX6[6:0] = HEX47[22:16];
assign HEX7[6:0] = HEX47[30:24];

sdram_pll neg_3ns(CLOCK_50, DRAM_CLK);
mda_cpu nios2(
	.clk(CLOCK_50),
	.reset_n(KEY[0]),
	.out_port_from_the_hex03(HEX03),
	.out_port_from_the_hex47(HEX47),
	.in_port_to_the_key(KEY),
	.out_port_from_the_ledg(LEDG),
	.out_port_from_the_ledr(LEDR),
	.in_port_to_the_sw(SW),
	.LCD_E_from_the_lcd(LCD_EN),
	.LCD_RS_from_the_lcd(LCD_RS),
	.LCD_RW_from_the_lcd(LCD_RW),
	.LCD_data_to_and_from_the_lcd(LCD_DATA),
	.zs_addr_from_the_sdram(DRAM_ADDR),
	.zs_ba_from_the_sdram({DRAM_BA_1, DRAM_BA_0}),
	.zs_cas_n_from_the_sdram(DRAM_CAS_N),
	.zs_cke_from_the_sdram(DRAM_CKE),
	.zs_cs_n_from_the_sdram(DRAM_CS_N),
	.zs_dq_to_and_from_the_sdram(DRAM_DQ),
	.zs_dqm_from_the_sdram({DRAM_UDQM, DRAM_LDQM}),
	.zs_ras_n_from_the_sdram(DRAM_RAS_N),
	.zs_we_n_from_the_sdram(DRAM_WE_N) 
	);
               
endmodule