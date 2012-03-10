/*  This file takes the 3 axis 10 bit data as input,
and outputs a signed value on three axis of the degree
as a sign extened 16 bit value of the degree*16.  The 
extra 4 bits are mean't as a degree of accuracy. */


module raw_data_to_degree(
   clk,           //clk for circuit, not much to say.  No reset.
	reset_n,
	tune_offset,   //When high, sends circuit into tunning mode for axis offset values.
	update_output, //When high, update outputs as soon as possible. 
	raw_x,
	raw_y,
	raw_z,
	no_external_force,
	output_x,
	output_y,
	output_z,
	output_x_degree,
	output_y_degree,
	output_z_degree,
	debug_0,
	debug_1,
	debug_2,
	debug_3
);

input         clk;
input         reset_n;
input         tune_offset;
input         update_output;
input [9:0]   raw_x;
input [9:0]   raw_y;
input [9:0]   raw_z;

output        no_external_force;

output reg [15:0] output_x;
output reg [15:0] output_y;
output reg [15:0] output_z;

output  reg [31:0] output_x_degree;
output  reg [31:0] output_y_degree;
output  reg [31:0] output_z_degree;

output [31:0]  debug_0;
output [31:0]  debug_1;
output [31:0]  debug_2;
output [31:0]  debug_3;

reg    [31:0] tuning_counter;
reg    [31:0] acc_magnitude_squared;

reg    [3:0]  state;
reg    [3:0]  next_state;
reg           state_done;

reg    [19:0] divider_denom;
reg    [19:0] divider_numer;

reg    [31:0] fsm_counter;

wire          is_upright;
wire   [9:0]  abs_x;
wire   [9:0]  abs_y;
wire   [9:0]  abs_z;
wire          x_is_neg;
wire          y_is_neg;
wire          z_is_neg;
wire   [15:0] output_x_degree_wire;
wire   [15:0] output_y_degree_wire;
wire   [15:0] output_z_degree_wire;

wire   [19:0] divider_quotient;

parameter INITIAL = 4'd0;
parameter TUNING = 4'd1;
parameter TUNING_DONE = 4'd2;
parameter OUTPUT_CALCULATION_0 = 4'd3;
parameter OUTPUT_CALCULATION_1 = 4'd4;
parameter OUTPUT_CALCULATION_2 = 4'd5;
parameter OUTPUT_CALCULATION_3 = 4'd6;
parameter OUTPUT_CALCULATION_4 = 4'd7;
parameter OUTPUT_CALCULATION_DONE = 4'd10;

assign is_upright = !raw_z[9];

always @ ( posedge clk )
begin
   state <= next_state;
end

always @ (state)
begin : FSM_STATES_0
   next_state = INITIAL;
   case(state)
      INITIAL: if (tune_offset) 
		   begin
            next_state <= TUNING;
         end 
		   else if (update_output) 
		   begin
            next_state <= OUTPUT_CALCULATION_0;
         end
			else begin
			   next_state <= INITIAL;
			end
		TUNING: if (tuning_counter == 32'h00000001)
		   begin
			   next_state <= TUNING_DONE;
			end
			else begin
			   next_state <= TUNING;
		   end
		TUNING_DONE: if (!tune_offset)
		   begin
			   next_state <= INITIAL;
			end
			else begin
			   next_state <= TUNING_DONE;
			end
		OUTPUT_CALCULATION_0: if (fsm_counter == 32'd100)
			begin
			   next_state <= OUTPUT_CALCULATION_1;
			end
			else begin
			   next_state <= OUTPUT_CALCULATION_0;
			end
		OUTPUT_CALCULATION_1: if (fsm_counter == 32'd100)
			begin
			   next_state <= OUTPUT_CALCULATION_2;
			end
			else begin
			   next_state <= OUTPUT_CALCULATION_1;
			end
		OUTPUT_CALCULATION_2: if (fsm_counter == 32'd100)
			begin
			   next_state <= OUTPUT_CALCULATION_3;
			end
			else begin
			   next_state <= OUTPUT_CALCULATION_2;
			end
		OUTPUT_CALCULATION_3: if (fsm_counter == 32'd100)
			begin
			   next_state <= OUTPUT_CALCULATION_4;
			end
			else begin
			   next_state <= OUTPUT_CALCULATION_3;
			end
		OUTPUT_CALCULATION_4: if (fsm_counter == 32'd100)
			begin
			   next_state <= OUTPUT_CALCULATION_DONE;
			end
			else begin
			   next_state <= OUTPUT_CALCULATION_4;
			end
		OUTPUT_CALCULATION_DONE: if (1'b1)
		   begin
			   next_state <= INITIAL;
			end
			else begin
			   next_state <= OUTPUT_CALCULATION_DONE;
			end
      default : next_state = INITIAL;
   endcase
end

assign abs_y = raw_x[9] ? ({!raw_x[9],!raw_x[8],!raw_x[7],!raw_x[6],!raw_x[5],!raw_x[4],!raw_x[3],!raw_x[2],!raw_x[1],!raw_x[0]}+1'b1):(raw_x[9:0]);
assign abs_x = raw_y[9] ? ({!raw_y[9],!raw_y[8],!raw_y[7],!raw_y[6],!raw_y[5],!raw_y[4],!raw_y[3],!raw_y[2],!raw_y[1],!raw_y[0]}+1'b1):(raw_y[9:0]);
assign abs_z = raw_z[9] ? ({!raw_z[9],!raw_z[8],!raw_z[7],!raw_z[6],!raw_z[5],!raw_z[4],!raw_z[3],!raw_z[2],!raw_z[1],!raw_z[0]}+1'b1):(raw_z[9:0]);
assign y_is_neg = raw_x[9];
assign x_is_neg = raw_y[9];
assign z_is_neg = raw_z[9];


always @ (posedge clk)
begin : FSM_STATES_2
fsm_counter <= 32'd0;
if ( fsm_counter == 32'd100 )
begin
   acc_magnitude_squared <= 255*255;
   fsm_counter <= 32'd0;
end
else if ( !reset_n )
begin
   state_done <= 1'b1;
end
else begin
   case(state)
      INITIAL : 
      begin
		   tuning_counter <= 32'd0;
      end
      TUNING : 
      begin
		   acc_magnitude_squared <= abs_x*abs_x + abs_y*abs_y + abs_z*abs_z;
		   tuning_counter <= tuning_counter + 1'b1;
      end
      TUNING_DONE : 
	   begin
		   tuning_counter <= 32'd0;
      end
		OUTPUT_CALCULATION_0 : 
	   begin
			if ( fsm_counter == 32'd0 )
			begin
			   fsm_counter <= fsm_counter + 1'b1;
				divider_denom <= 20'd256;
				divider_numer <= abs_x*20'd1000;
			end
			else if ( fsm_counter >= 32'd2 )
			begin
			   fsm_counter <= 32'd100;
				output_x <= divider_quotient[15:0];
			end
			else
			   fsm_counter <= fsm_counter + 1'b1;
      end
		OUTPUT_CALCULATION_1 : 
	   begin
		   if ( fsm_counter == 32'd0 )
			begin
			   fsm_counter <= fsm_counter + 1'b1;
				divider_denom <= 20'd256;
				divider_numer <= abs_y*20'd1000;
			end
			else if ( fsm_counter >= 32'd2 )
			begin
			   fsm_counter <= 32'd100;
				output_y <= divider_quotient[15:0];
			end
			else
			   fsm_counter <= fsm_counter + 1'b1;
      end
		OUTPUT_CALCULATION_2 : 
	   begin
		   if ( fsm_counter == 32'd0 )
			begin
			   fsm_counter <= fsm_counter + 1'b1;
				divider_denom <= 20'd256;
				divider_numer <= abs_z*20'd1000;
			end
			else if ( fsm_counter >= 32'd2 )
			begin
			   fsm_counter <= 32'd100;
				output_z <= divider_quotient[15:0];
			end
			else
			   fsm_counter <= fsm_counter + 1'b1;
      end
		OUTPUT_CALCULATION_3 : 
	   begin
		   fsm_counter <= 32'd100;
      end
		OUTPUT_CALCULATION_4 : 
	   begin
		   fsm_counter <= 32'd100;
      end
		OUTPUT_CALCULATION_DONE : 
	   begin
      end
      default : 
	   begin
			state_done <= 1'b0;
      end
   endcase
end
end

assign no_external_force = (((acc_magnitude_squared+acc_magnitude_squared[31:2])>(abs_x*abs_x + abs_y*abs_y + abs_z*abs_z))
&& ((acc_magnitude_squared-acc_magnitude_squared[31:2])<(abs_x*abs_x + abs_y*abs_y + abs_z*abs_z))) ? (1'b1): (1'b0);

divider_20bits divider_20bits(
	.denom(divider_denom),
	.numer(divider_numer),
	.quotient(divider_quotient),
	.remain());
	
arcsine_lookup_table x_axis_arcsin(
   .input_num(output_x[9:0]),
	.output_num(output_x_degree_wire)
);

arcsine_lookup_table y_axis_arcsin(
   .input_num(output_y[9:0]),
	.output_num(output_y_degree_wire)
);

arcsine_lookup_table z_axis_arcsin(
   .input_num(output_z[9:0]),
	.output_num(output_z_degree_wire)
);

always @ (*)
begin
   if ( no_external_force)
	begin
      if ( is_upright )
	   begin
	      output_x_degree <= (x_is_neg) ? ({16'hffff,~output_x_degree_wire}+1'b1): ({16'd0,output_x_degree_wire});
         output_y_degree <= (y_is_neg) ? ({16'hffff,~output_y_degree_wire}+1'b1): ({16'd0,output_y_degree_wire});
         output_z_degree <= (z_is_neg) ? ({16'hffff,~output_z_degree_wire}+1'b1): ({16'd0,output_z_degree_wire});   
	   end
	   else
	   begin
		   output_x_degree <= (!x_is_neg) ? ({16'hffff,~output_x_degree_wire}+1'b1): ({16'd0,output_x_degree_wire});//({16'h0,output_x_degree_wire}-32'd1440): (32'd1440-{16'd0,output_x_degree_wire});
         output_y_degree <= (y_is_neg) ? ({16'h0,output_y_degree_wire}-32'd1440): (32'd1440-{16'd0,output_y_degree_wire});
         output_z_degree <= (z_is_neg) ? ({16'h0,output_z_degree_wire}-32'd1440): (32'd1440-{16'd0,output_z_degree_wire});  
	   end
	 end else
	   begin
	      output_x_degree <= output_x_degree;
		   output_y_degree <= output_y_degree;
		   output_z_degree <= output_z_degree;
	end
end	
	
endmodule