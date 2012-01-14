module magnetometer_i2c_interface(
   reset_n,
   clk,
   sda_magnetometer,
   scl_magnetometer,
   magnetometer_data_x,
   magnetometer_data_y,
   magnetometer_data_z,
   debug
);

input reset_n;
input clk;
inout sda_magnetometer;
output reg scl_magnetometer;

output [31:0] magnetometer_data_x;
output [31:0] magnetometer_data_y;
output [31:0] magnetometer_data_z;

output [7:0] debug;

reg       sda_no_high_z;
reg       sda_data;
assign sda_magnetometer = sda_no_high_z ? sda_data : 1'bz;
reg [7:0] debug_reg;

// assuming a 2 Mhz clk, turn this into 200 kHz due to device tolerances.
reg clk_100;
reg [31:0] clk_counter;

always @ ( posedge clk )
begin
   if ( clk_counter == 32'd2000000 )
   begin
      clk_100 <= ~clk_100;
      clk_counter <= 32'd0;
   end
   else
   begin
      clk_counter <= clk_counter + 1'b1;
   end
end

////////////////Start of i2c interface////////////////////

parameter WAITING = 8'd0;
parameter START = 8'd1;
parameter WRITE = 8'd2;
parameter READ = 8'd3;

assign debug[1:0] = state[1:0];
assign debug[7] = sda_magnetometer;
assign debug[6] = scl_magnetometer;
assign debug[5:2] = cycle_counter[3:0];

reg [6:0] address; //Address to begin I2c transactions
reg [7:0] command; 
//command[1:0] == 0 
//command[1:0] == 1 read 
//command[1:0] == 2 write

reg [7:0] data_0;  //Will be read at transaction time if write, will write to if not.
reg [7:0] data_1; 
reg       completed; // The instruction is completed or not.
reg [7:0] cycle_counter;

reg [7:0] state;
reg [7:0] next_state;

always @ ( posedge clk_100 )
begin
   command[1:0] <= 2'b1;
   address <= {6'h1e,1'b1}; // last bit 1 == read
end

always @ ( posedge clk_100 )
begin
   state <= next_state;
end

always @ (*)
begin : FSM_STATES_0
   if ( !reset_n )
   begin
      next_state <= WAITING;
   end
   else
   begin
   case(state)
      WAITING: if ( (command == 8'd2 || command == 8'd1) && sda_magnetometer && scl_magnetometer && 1'b0) // A command is issued and the I2c channel is open
         begin
            next_state <= START;
         end
         else
         begin
            next_state <= WAITING;
         end
      START:
         begin
            // This states transmits the address, and whether to write or read.
            if ( cycle_counter > 8'd17 && !sda_magnetometer && scl_magnetometer)
            begin
               if (command[1:0] == 2'd2) //write
                  next_state <= WRITE;
               else if (command[1:0] == 2'd1) //read
                  next_state <= READ;
               else
                  next_state <= START;
            end
            else begin
               next_state <= START;
            end
         end
      WRITE:
         begin
            next_state <= WRITE;
         end
      READ:
         begin
            next_state <= READ;
         end   
      default : next_state <= WAITING;
   endcase
   end
end


always @ (posedge clk_100 or negedge reset_n)
begin : FSM_STATES_2
   if ( !reset_n )
   begin
      cycle_counter <= 8'd0;
      sda_no_high_z <= 1'b0;
      sda_data <= 1'b0;
      scl_magnetometer <= 1'b1;
   end
   else begin
      case(state)
         WAITING:
         begin
            cycle_counter <= 8'd0;
            sda_no_high_z <= 1'b0;
            sda_data <= 1'b0;
            scl_magnetometer <= 1'b1;
         end
         START: 
         begin
            if ( cycle_counter > 8'd16 )
            begin   
               sda_no_high_z <= 1'b0;
               sda_data <= 1'b0;
            end      
            if ( cycle_counter == 8'd0 )
            begin
               sda_no_high_z <= 1'b1;
               sda_data <= 1'b0;
               scl_magnetometer <= 1'b1;
            end
            if ( cycle_counter == 8'd0 )
            begin
               sda_no_high_z <= 1'b1;
               sda_data <= 1'b0;
               scl_magnetometer <= 1'b0;
            end
            if ( cycle_counter > 1 && !cycle_counter[0] )
            begin
               scl_magnetometer <= 1'b1;
            end
            if ( cycle_counter[0] )
            begin
               scl_magnetometer <= 1'b0;
            end

            if ( cycle_counter == 8'd2 )
            begin
               sda_no_high_z <= 1'b1;
               sda_data <= address[6];
            end
              else if ( cycle_counter == 8'd4 )
            begin
               sda_no_high_z <= 1'b1;
               sda_data <= address[5];
            end
            else if ( cycle_counter == 8'd6 )
            begin
               sda_no_high_z <= 1'b1;
               sda_data <= address[4];
            end
            else if ( cycle_counter == 8'd8 )
            begin
               sda_no_high_z <= 1'b1;
               sda_data <= address[3];
            end
            else if ( cycle_counter == 8'd10 )
            begin
               sda_no_high_z <= 1'b1;
               sda_data <= address[2];
            end
            else if ( cycle_counter == 8'd12 )
            begin
               sda_no_high_z <= 1'b1;
               sda_data <= address[1];
            end
            else if ( cycle_counter == 8'd14 )
            begin
               sda_no_high_z <= 1'b1;
               sda_data <= address[0];
            end
            else if ( cycle_counter == 8'd16 )
            begin
               if (command[1:0] == 2'd2) //write
               begin
                  sda_no_high_z <= 1'b1;
                  sda_data <= 1'b0;
               end
               else if (command[1:0] == 2'd1) //read
               begin
                  sda_no_high_z <= 1'b1;
                  sda_data <= 1'b1;
               end
            end
            cycle_counter <= cycle_counter + 1'b1;            
         end
         WRITE:
         begin
         end
         READ:
         begin
            cycle_counter <= cycle_counter + 1'b1;      
            if ( cycle_counter[0] == 1'b0 )
            begin
               scl_magnetometer <= 1'b1;
            end
            if ( cycle_counter[0] == 1'b1 )
            begin
               scl_magnetometer <= 1'b0;
            end
            
            if ( cycle_counter == 8'd18 )
               debug_reg[7] <= sda_magnetometer;
              else if ( cycle_counter == 8'd20 )
               debug_reg[6] <= sda_magnetometer;
            else if ( cycle_counter == 8'd22 )
               debug_reg[5] <= sda_magnetometer;
            else if ( cycle_counter == 8'd24 )
               debug_reg[4] <= sda_magnetometer;
            else if ( cycle_counter == 8'd26 )
               debug_reg[3] <= sda_magnetometer;
            else if ( cycle_counter == 8'd28 )
               debug_reg[2] <= sda_magnetometer;
            else if ( cycle_counter == 8'd30 )
               debug_reg[1] <= sda_magnetometer;
            else if ( cycle_counter == 8'd32 )
               debug_reg[0] <= sda_magnetometer;      
         end
      endcase
   end
end


endmodule
