// This is the Avalon slave for the IMU
//
// Registers 0-2 store the gyroscope
// Register 3 stores the depth sensor
// Registers 4-6 store the magnetometer
// Registers 7-9 store the accelerometer

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

  wire [31:0] raw_depth;
  wire [31:0] output_x_degree;
  wire [31:0] output_y_degree;
  wire [31:0] output_z_degree;
 

  imu imu(
    .reset_n(1'b1),
    .spi_clk(spi_clk),
    .sys_clk(sys_clk),
    .no_external_force(1'b0),
    .sda(),
    .scl(),
    .gyroscope_degree_x(gyr_x),
    .gyroscope_degree_y(gyr_y),
    .gyroscope_degree_z(gyr_z),
    .raw_depth(raw_depth),
    .magnetometer_data_x(mag_x),
    .magnetometer_data_y(mag_y),
    .magnetometer_data_z(mag_z),
    .ADC_SDAT(ADC_SDAT),
    .ADC_CS_N(ADC_CS_N),
    .ADC_SADDR(ADC_SADDR),
    .ADC_SCLK(ADC_SCLK)
);

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
      4'b0011:
        readdata <= raw_depth;
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
      default:
        readdata <= 32'd0;
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

