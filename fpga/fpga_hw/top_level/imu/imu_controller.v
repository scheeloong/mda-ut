// This is the Avalon slave for the IMU
//
// Register 3 stores the depth sensor
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

  wire [31:0] raw_depth;
 

  imu imu(
    .reset_n(1'b1),
    .spi_clk(spi_clk),
    .sys_clk(sys_clk),
    .sda(),
    .scl(),
    .raw_depth(raw_depth),
    .ADC_SDAT(ADC_SDAT),
    .ADC_CS_N(ADC_CS_N),
    .ADC_SADDR(ADC_SADDR),
    .ADC_SCLK(ADC_SCLK)
);

  always @(posedge clk)
    if (chipselect & read)
    casex (addr)
      4'b0011:
        readdata <= raw_depth;
      default:
        readdata <= 32'd0;
    endcase
endmodule

