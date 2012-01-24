// Motion in perpendicular plane will cause variations on the specific axis, note that this represent motion AROUND a specific axis
// z-axis = side to side (short)
// y-axis = side to side (long)
// x-axis = vertical

/*

---------------------------------|----------------------------------
---------------------------------|- Y - Axis -----------------------
---------------------------------|----------------------------------
---------------------------------|----------------------------------
---------------------=========================----------------------
---------------------||                     ||----------------------
---------------------||                     ||______________________
---------------------||        DE0-Nano     ||--- X - Axis ---------
---------------------||                     ||----------------------
---------------------||                     ||----------------------
---------------------=========================----------------------
---------------------------------|----------------------------------
---------------------------------|----------------------------------
---------------------------------|----------------------------------
---------------------------------|----------------------------------

*/

module imu(
   input reset_n,
   input dly_rst,
   input spi_clk,
   input sys_clk,
   input no_external_force,
   output [31:0] gyroscope_degree_x,
   output [31:0] gyroscope_degree_y,
   output [31:0] gyroscope_degree_z,
   output [31:0]  magnetometer_data_x,
   output [31:0]  magnetometer_data_y,
   output [31:0]  magnetometer_data_z,

   //////////// ADC //////////
   input                                  ADC_SDAT,
   output                                 ADC_CS_N,
   output                                 ADC_SADDR,
   output                                 ADC_SCLK
);

///=======================================================
//  REG/WIRE declarations
//=======================================================
wire  [11:0]  ADC_12_bit_channel_0;
wire  [11:0]  ADC_12_bit_channel_1;
wire  [11:0]  ADC_12_bit_channel_2;
wire  [11:0]  ADC_12_bit_channel_3;
wire  [11:0]  ADC_12_bit_channel_4;
wire  [11:0]  ADC_12_bit_channel_5;
wire  [11:0]  ADC_12_bit_channel_6;
wire  [11:0]  ADC_12_bit_channel_7;
 
/* The GPIO 2 pin layout

1     0
3     2
5     4
7     6
9     8
11    10     
ADC5  12
ADC7  ADC7
ADC2  ADC3
ADC0  ADC4
GND   ADC1

*/

gyroscope_data_to_degrees gyroscope_data_to_degrees(
   .clk(sys_clk),
   .reset_n(reset_n),
   .no_external_force(no_external_force),
   .gyroscope_data_x({20'd0,ADC_12_bit_channel_3}),
   .gyroscope_data_y({20'd0,ADC_12_bit_channel_4}),
   .gyroscope_data_z({20'd0,ADC_12_bit_channel_7}),
   .gyroscope_degree_x(gyroscope_degree_x),
   .gyroscope_degree_y(gyroscope_degree_y),
   .gyroscope_degree_z(gyroscope_degree_z)
);

// ADC connections, on chip

ADC_CTRL adc_controller_8_channels (
   .iRST(!reset_n),
   .iCLK(spi_clk),
   .iCLK_n(!spi_clk),
   .iGO(1'b1),
						
   .oDIN(ADC_SADDR),
   .oCS_n(ADC_CS_N),
   .oSCLK(ADC_SCLK),
   .iDOUT(ADC_SDAT),
						
   .oADC_12_bit_channel_0(ADC_12_bit_channel_0),
   .oADC_12_bit_channel_1(ADC_12_bit_channel_1),
   .oADC_12_bit_channel_2(ADC_12_bit_channel_2),
   .oADC_12_bit_channel_3(ADC_12_bit_channel_3),
   .oADC_12_bit_channel_4(ADC_12_bit_channel_4),
   .oADC_12_bit_channel_5(ADC_12_bit_channel_5),
   .oADC_12_bit_channel_6(ADC_12_bit_channel_6),
   .oADC_12_bit_channel_7(ADC_12_bit_channel_7)
);

magnetometer_i2c_interface magnetometer_i2c_interface( 
   .reset_n(reset_n),
   .clk(spi_clk),
   .magnetometer_data_x(magnetometer_data_x),
   .magnetometer_data_y(magnetometer_data_y),
   .magnetometer_data_z(magnetometer_data_z)
);	
	
endmodule