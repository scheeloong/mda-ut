// This is the Avalon slave for the IMU
// Registers 0-2 store the gyroscope
// Registers 4-6 stores the magnetometer

module imu_controller(input clk, input chipselect, input [2:0]addr, input read, output reg [31:0] readdata, input spi_clk, input sys_clk);
  wire [31:0] gyr_x;
  wire [31:0] gyr_y;
  wire [31:0] gyr_z;
  wire [31:0] mag_x;
  wire [31:0] mag_y;
  wire [31:0] mag_z;

  imu imu(1'b1, spi_clk, sys_clk, 1'b0, gyr_x, gyr_y, gyr_z, mag_x, mag_y, mag_z);

  always @(posedge clk)
    if (chipselect & read)
    casex (addr)
      3'b000:
        readdata <= gyr_x;
      3'b001:
        readdata <= gyr_y;
      3'b010:
        readdata <= gyr_z;
      3'b100:
        readdata <= mag_x;
      3'b101:
        readdata <= mag_y;
      3'b110:
        readdata <= mag_z;
      default:
        readdata = 32'd0;
    endcase
endmodule

