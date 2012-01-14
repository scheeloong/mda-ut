module gyroscope_data_to_degrees (
   clk,
   reset_n,
   no_external_force,
   gyroscope_data_x,
   gyroscope_data_y,
   gyroscope_data_z,
   gyroscope_degree_x,
   gyroscope_degree_y,
   gyroscope_degree_z
);

// Expect input: raw gyropscope adc data
// This module will tune for the initial adc value condition

input          clk;
input          reset_n;
input          no_external_force;
input [31:0]   gyroscope_data_x;
input [31:0]   gyroscope_data_y;
input [31:0]   gyroscope_data_z;

output reg [31:0]   gyroscope_degree_x;
output reg [31:0]   gyroscope_degree_y;
output reg [31:0]   gyroscope_degree_z;

wire [31:0] integrator_input_x;
wire [31:0] integrator_input_y;
wire [31:0] integrator_input_z;
wire [31:0] integrator_output_x;
wire [31:0] integrator_output_y;
wire [31:0] integrator_output_z;
wire [31:0] lookup_output;

reg signed[31:0]   gyroscope_degree_x_temp;
reg signed[31:0]   gyroscope_degree_y_temp;
reg signed[31:0]   gyroscope_degree_z_temp;
reg         tuned;
reg  [31:0] state;
reg  [31:0] next_state;
reg  [31:0] tuning_counter;
reg  [31:0] x_axis_offset;
reg  [31:0] y_axis_offset;
reg  [31:0] z_axis_offset;
reg  [63:0] accumulator_filter_x;
reg  [63:0] accumulator_filter_y;
reg  [63:0] accumulator_filter_z;
reg         is_negative;
reg  [31:0] lookup_input;
reg  [31:0] counter;
reg  [1:0]  lookup_select;

assign   integrator_input_x = (tuned) ? (gyroscope_data_x - x_axis_offset) : (32'd0);
assign   integrator_input_y = (tuned) ? (gyroscope_data_y - y_axis_offset) : (32'd0);
assign   integrator_input_z = (tuned) ? (gyroscope_data_z - z_axis_offset) : (32'd0);

parameter INITIAL = 32'd0;
parameter TUNED = 32'd1;

/*

Upon reset, this system will take a snapshot of the gyroscope data to use as offset after 10 million clk cycles.

*/

always @ ( posedge clk )
begin
   state <= next_state;
end

always @ (state)
begin : FSM_STATES_0
   next_state = INITIAL;
   case(state)
      INITIAL: if (reset_n) 
         begin
            next_state <= TUNED;
         end 
         else begin
            next_state <= INITIAL;
         end
      TUNED: if ( reset_n )
         begin
            next_state <= TUNED;
         end
         else begin
            next_state <= INITIAL;
         end
      default : next_state = INITIAL;
   endcase
end


always @ (posedge clk)
begin : FSM_STATES_2
if ( !reset_n )
begin
end
else begin
   case(state)
      INITIAL:
      begin
         tuned <= 1'b0;
         tuning_counter <= 32'd0;
         x_axis_offset <= 32'd0;
         y_axis_offset <= 32'd0;
         z_axis_offset <= 32'd0;
         accumulator_filter_x <= 64'd0;
         accumulator_filter_y <= 64'd0;
         accumulator_filter_z <= 64'd0;
      end
      TUNED: 
      begin
         if ( tuning_counter < (32'd50000000) )
         begin
            tuning_counter <= tuning_counter + 1'b1;
         end
         else if ( tuning_counter < (32'd16777216 + 32'd50000000) )
         begin
            tuning_counter <= tuning_counter + 1'b1;
            accumulator_filter_x <= accumulator_filter_x + gyroscope_data_x;
            accumulator_filter_y <= accumulator_filter_y + gyroscope_data_y;
            accumulator_filter_z <= accumulator_filter_z + gyroscope_data_z;
         end
         else
         begin
            if ( tuning_counter == (32'd16777216 + 32'd50000000))
            begin
               tuned <= 1'b1;
               x_axis_offset <= accumulator_filter_x[55:24];
               y_axis_offset <= accumulator_filter_y[55:24];
               z_axis_offset <= accumulator_filter_z[55:24];
               // the offsets will not be touched until next offset
               tuning_counter <= tuning_counter + 1'b1;
            end
         end         
      end
   endcase
end
end

integrator_signed_32bits integrator_x_axis_angular_velocity(
   .clk(clk),
   .reset_n(reset_n && !no_external_force),
   .update_period(32'd196078),
   .input_32(integrator_input_z),
   .output_32(integrator_output_x)
);
integrator_signed_32bits integrator_y_axis_angular_velocity(
   .clk(clk),
   .reset_n(reset_n && !no_external_force),
   .update_period(32'd196078),
   .input_32(integrator_input_y),
   .output_32(integrator_output_y)
);
integrator_signed_32bits integrator_z_axis_angular_velocity(
   .clk(clk),
   .reset_n(reset_n && !no_external_force),
   .update_period(32'd196078),
   .input_32(integrator_input_x),
   .output_32(integrator_output_z)
);

always @ ( posedge clk )
begin
   if ( counter == 32'd1)
   begin
      counter <= counter + 1'b1;
      if (lookup_select == 2'd0)
      begin
         if ( integrator_output_x[31] == 1'b1 )
         begin
            is_negative <= 1'b0;
            lookup_input <= (~integrator_output_x) + 1'b1;
         end
         else
         begin
            is_negative <= 1'b1;
            lookup_input <= integrator_output_x;
         end
      end
      else if (lookup_select == 2'd1)
      begin
         if ( integrator_output_y[31] == 1'b1 )
         begin
            is_negative <= 1'b0;
            lookup_input <= (~integrator_output_y) + 1'b1;
         end
         else
         begin
            is_negative <= 1'b1;
            lookup_input <= integrator_output_y;
         end
      end
      else if (lookup_select == 2'd2)
      begin
         if ( integrator_output_z[31] == 1'b1 )
         begin
            is_negative <= 1'b0;
            lookup_input <= (~integrator_output_z) + 1'b1;
         end
         else
         begin
            is_negative <= 1'b1;
            lookup_input <= integrator_output_z;
         end
      end   
   end
   else if ( counter == 32'd5 )
   begin
      counter <= counter + 1'b1;
      if (lookup_select == 2'd0)
      begin
         if ( is_negative )
         begin
            gyroscope_degree_x_temp <= (~lookup_output) + 1'b1;
         end
         else
         begin
            gyroscope_degree_x_temp <= lookup_output;         
         end
      end
      else if (lookup_select == 2'd1)
      begin
         if ( is_negative )
         begin
            gyroscope_degree_y_temp <= (~lookup_output) + 1'b1;
         end
         else
         begin
            gyroscope_degree_y_temp <= lookup_output;         
         end
      end
      else if (lookup_select == 2'd2)
      begin
         if ( is_negative )
         begin
            gyroscope_degree_z_temp <= (~lookup_output) + 1'b1;
         end
         else
         begin
            gyroscope_degree_z_temp <= lookup_output;         
         end
      end
   end
   else if ( counter == 32'd6 )
   begin
      counter <= counter + 1'b1;
      if ( ( gyroscope_degree_x_temp > 32'd1440 ) && !gyroscope_degree_x_temp[31])
         gyroscope_degree_x_temp <= gyroscope_degree_x_temp - 32'd2880;
      else if ( (gyroscope_degree_x_temp + 32'd2880) < 32'd1440) 
         gyroscope_degree_x_temp <= gyroscope_degree_x_temp + 32'd2880;
      if ( ( gyroscope_degree_y_temp > 32'd1440 ) && !gyroscope_degree_y_temp[31])
         gyroscope_degree_y_temp <= gyroscope_degree_y_temp - 32'd2880;
      else if ( (gyroscope_degree_y_temp + 32'd2880) < 32'd1440) 
         gyroscope_degree_y_temp <= gyroscope_degree_y_temp + 32'd2880;
      if ( ( gyroscope_degree_z_temp > 32'd1440 ) && !gyroscope_degree_z_temp[31])
         gyroscope_degree_z_temp <= gyroscope_degree_z_temp - 32'd2880;
      else if ( (gyroscope_degree_z_temp + 32'd2880) < 32'd1440) 
         gyroscope_degree_z_temp <= gyroscope_degree_z_temp + 32'd2880;
   end
   else if ( counter == 32'd7 )
   begin
      counter <= counter + 1'b1;
      if ( ( gyroscope_degree_x_temp > 32'd1440 ) && !gyroscope_degree_x_temp[31])
         gyroscope_degree_x_temp <= gyroscope_degree_x_temp - 32'd2880;
      else if ( (gyroscope_degree_x_temp + 32'd2880) < 32'd1440) 
         gyroscope_degree_x_temp <= gyroscope_degree_x_temp + 32'd2880;
      if ( ( gyroscope_degree_y_temp > 32'd1440 ) && !gyroscope_degree_y_temp[31])
         gyroscope_degree_y_temp <= gyroscope_degree_y_temp - 32'd2880;
      else if ( (gyroscope_degree_y_temp + 32'd2880) < 32'd1440) 
         gyroscope_degree_y_temp <= gyroscope_degree_y_temp + 32'd2880;
      if ( ( gyroscope_degree_z_temp > 32'd1440 ) && !gyroscope_degree_z_temp[31])
         gyroscope_degree_z_temp <= gyroscope_degree_z_temp - 32'd2880;
      else if ( (gyroscope_degree_z_temp + 32'd2880) < 32'd1440) 
         gyroscope_degree_z_temp <= gyroscope_degree_z_temp + 32'd2880;
   end
   else if ( counter == 32'd8 )
   begin
      counter <= counter + 1'b1;
      if ( ( gyroscope_degree_x_temp > 32'd1440 ) && !gyroscope_degree_x_temp[31])
         gyroscope_degree_x_temp <= gyroscope_degree_x_temp - 32'd2880;
      else if ( (gyroscope_degree_x_temp + 32'd2880) < 32'd1440) 
         gyroscope_degree_x_temp <= gyroscope_degree_x_temp + 32'd2880;
      if ( ( gyroscope_degree_y_temp > 32'd1440 ) && !gyroscope_degree_y_temp[31])
         gyroscope_degree_y_temp <= gyroscope_degree_y_temp - 32'd2880;
      else if ( (gyroscope_degree_y_temp + 32'd2880) < 32'd1440) 
         gyroscope_degree_y_temp <= gyroscope_degree_y_temp + 32'd2880;
      if ( ( gyroscope_degree_z_temp > 32'd1440 ) && !gyroscope_degree_z_temp[31])
         gyroscope_degree_z_temp <= gyroscope_degree_z_temp - 32'd2880;
      else if ( (gyroscope_degree_z_temp + 32'd2880) < 32'd1440) 
         gyroscope_degree_z_temp <= gyroscope_degree_z_temp + 32'd2880;
   end
   else if ( counter == 32'd9 )
   begin
      counter <= counter + 1'b1;
      if ( ( gyroscope_degree_x_temp > 32'd1440 ) && !gyroscope_degree_x_temp[31])
         gyroscope_degree_x_temp <= gyroscope_degree_x_temp - 32'd2880;
      else if ( (gyroscope_degree_x_temp + 32'd2880) < 32'd1440) 
         gyroscope_degree_x_temp <= gyroscope_degree_x_temp + 32'd2880;
      if ( ( gyroscope_degree_y_temp > 32'd1440 ) && !gyroscope_degree_y_temp[31])
         gyroscope_degree_y_temp <= gyroscope_degree_y_temp - 32'd2880;
      else if ( (gyroscope_degree_y_temp + 32'd2880) < 32'd1440) 
         gyroscope_degree_y_temp <= gyroscope_degree_y_temp + 32'd2880;
      if ( ( gyroscope_degree_z_temp > 32'd1440 ) && !gyroscope_degree_z_temp[31])
         gyroscope_degree_z_temp <= gyroscope_degree_z_temp - 32'd2880;
      else if ( (gyroscope_degree_z_temp + 32'd2880) < 32'd1440) 
         gyroscope_degree_z_temp <= gyroscope_degree_z_temp + 32'd2880;
   end
   else if ( counter >= 32'd10 )
   begin
      if (lookup_select == 2'd0)
      begin
         gyroscope_degree_x <= gyroscope_degree_x_temp;         
      end
      else if (lookup_select == 2'd1)
      begin
         gyroscope_degree_y <= gyroscope_degree_y_temp;         
      end
      else if (lookup_select == 2'd2)
      begin
         gyroscope_degree_z <= gyroscope_degree_z_temp;         
      end
      lookup_select <= lookup_select + 1'b1;   
      counter <= 32'd0;
   end
   else begin
      counter <= counter + 1'b1;
   end
end

gyro_lookup_table gyro_lookup_table(
   .input_num(lookup_input),
   .output_num(lookup_output)
);

endmodule
