// This is the Avalon slave for the IMU
// Registers 0-2 store the gyroscope
// Registers 4-6 stores the magnetometer

module imu_controller(
   input clk, 
	input chipselect, 
	input [3:0]addr, 
	input read, 
	input write,
	output reg [31:0] readdata, 
	input spi_clk, 
	input sys_clk, 
	input ADC_SDAT, 
	output ADC_CS_N, 
	output ADC_SADDR, 
	output ADC_SCLK,
	input [31:0] writedata);
	
  wire [31:0] gyr_x;
  wire [31:0] gyr_y;
  wire [31:0] gyr_z;
  wire [31:0] mag_x;
  wire [31:0] mag_y;
  wire [31:0] mag_z;

  reg [31:0] raw_acclerometer_x;//signed 10 bit number
  reg [31:0] raw_acclerometer_y;
  reg [31:0] raw_acclerometer_z; 
  
  wire [31:0] output_x_degree;
  wire [31:0] output_y_degree;
  wire [31:0] output_z_degree;
 

  imu imu(
     1'b1, 
	  spi_clk, 
	  sys_clk, 
	  1'b0, 
	  gyr_x, 
	  gyr_y, 
	  gyr_z, 
	  mag_x, 
	  mag_y, 
	  mag_z, 
	  ADC_SDAT, 
	  ADC_CS_N, 
	  ADC_SADDR, 
	  ADC_SCLK);

raw_data_to_degree raw_data_to_degree(
     .clk(sys_clk),           //clk for circuit, not much to say.  No reset.
	  .reset_n(1'b1),
	  .tune_offset(1'b0),  
	  .update_output(1'b1),
	  .raw_x(raw_acclerometer_x),
	  .raw_y(raw_acclerometer_y),
	  .raw_z(raw_acclerometer_z),
	  .no_external_force(),
	  .output_x(),
	  .output_y(),
	  .output_z(),
	  .output_x_degree(output_x_degree),
	  .output_y_degree(output_y_degree),
	  .output_z_degree(output_z_degree)
);	  
	  
	  
  always @(posedge clk)
    if (chipselect & read)
    casex (addr)
      4'b0000:
        readdata <= gyr_x;
      4'b0001:
        readdata <= gyr_y;
      4'b0010:
        readdata <= gyr_z;
      4'b0100:
        readdata <= mag_x;
      4'b0101:
        readdata <= mag_y;
      4'b0110:
        readdata <= mag_z;
		4'b0111:
        readdata <= output_x_degree;
		4'b1000:
        readdata <= output_y_degree;
		4'b1001:
        readdata <= output_z_degree; 
		4'b1010:
        readdata <= 32'd0;
		4'b1011:
        readdata <= 32'd0; 
		4'b1100:
        readdata <= 32'd0;   
      default:
        readdata = 32'd0;
    endcase
	 	 
	 always @(posedge clk)
    if (chipselect & write)
    casex (addr)
	   4'b0000:
        raw_acclerometer_x <= writedata;
      4'b0001:
        raw_acclerometer_y <= writedata;
      4'b0010:
        raw_acclerometer_z <= writedata; 
      default:
		begin
		   // ???
		end
    endcase
endmodule

