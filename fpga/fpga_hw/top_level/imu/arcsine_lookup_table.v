module arcsine_lookup_table(input_num,output_num);
input  [9:0]  input_num;
output reg [15:0] output_num;


always @ (input_num)
begin
   if ( input_num == 20'd0 )
   begin
      output_num = { 13'd0 , 3'd0 };
   end
   else if ( input_num == 20'd1 )
   begin
      output_num = { 13'd0 , 3'd1 };
   end
   else if ( input_num == 20'd2 )
   begin
      output_num = { 13'd0 , 3'd1 };
   end
   else if ( input_num == 20'd3 )
   begin
      output_num = { 13'd0 , 3'd2 };
   end
   else if ( input_num == 20'd4 )
   begin
      output_num = { 13'd0 , 3'd2 };
   end
   else if ( input_num == 20'd5 )
   begin
      output_num = { 13'd0 , 3'd3 };
   end
   else if ( input_num == 20'd6 )
   begin
      output_num = { 13'd0 , 3'd3 };
   end
   else if ( input_num == 20'd7 )
   begin
      output_num = { 13'd0 , 3'd4 };
   end
   else if ( input_num == 20'd8 )
   begin
      output_num = { 13'd0 , 3'd4 };
   end
   else if ( input_num == 20'd9 )
   begin
      output_num = { 13'd0 , 3'd5 };
   end
   else if ( input_num == 20'd10 )
   begin
      output_num = { 13'd0 , 3'd5 };
   end
   else if ( input_num == 20'd11 )
   begin
      output_num = { 13'd0 , 3'd6 };
   end
   else if ( input_num == 20'd12 )
   begin
      output_num = { 13'd0 , 3'd6 };
   end
   else if ( input_num == 20'd13 )
   begin
      output_num = { 13'd0 , 3'd6 };
   end
   else if ( input_num == 20'd14 )
   begin
      output_num = { 13'd0 , 3'd7 };
   end
   else if ( input_num == 20'd15 )
   begin
      output_num = { 13'd0 , 3'd7 };
   end
   else if ( input_num == 20'd16 )
   begin
      output_num = { 13'd1 , 3'd0 };
   end
   else if ( input_num == 20'd17 )
   begin
      output_num = { 13'd1 , 3'd0 };
   end
   else if ( input_num == 20'd18 )
   begin
      output_num = { 13'd1 , 3'd1 };
   end
   else if ( input_num == 20'd19 )
   begin
      output_num = { 13'd1 , 3'd1 };
   end
   else if ( input_num == 20'd20 )
   begin
      output_num = { 13'd1 , 3'd2 };
   end
   else if ( input_num == 20'd21 )
   begin
      output_num = { 13'd1 , 3'd2 };
   end
   else if ( input_num == 20'd22 )
   begin
      output_num = { 13'd1 , 3'd3 };
   end
   else if ( input_num == 20'd23 )
   begin
      output_num = { 13'd1 , 3'd3 };
   end
   else if ( input_num == 20'd24 )
   begin
      output_num = { 13'd1 , 3'd4 };
   end
   else if ( input_num == 20'd25 )
   begin
      output_num = { 13'd1 , 3'd4 };
   end
   else if ( input_num == 20'd26 )
   begin
      output_num = { 13'd1 , 3'd4 };
   end
   else if ( input_num == 20'd27 )
   begin
      output_num = { 13'd1 , 3'd5 };
   end
   else if ( input_num == 20'd28 )
   begin
      output_num = { 13'd1 , 3'd5 };
   end
   else if ( input_num == 20'd29 )
   begin
      output_num = { 13'd1 , 3'd6 };
   end
   else if ( input_num == 20'd30 )
   begin
      output_num = { 13'd1 , 3'd6 };
   end
   else if ( input_num == 20'd31 )
   begin
      output_num = { 13'd1 , 3'd7 };
   end
   else if ( input_num == 20'd32 )
   begin
      output_num = { 13'd1 , 3'd7 };
   end
   else if ( input_num == 20'd33 )
   begin
      output_num = { 13'd2 , 3'd0 };
   end
   else if ( input_num == 20'd34 )
   begin
      output_num = { 13'd2 , 3'd0 };
   end
   else if ( input_num == 20'd35 )
   begin
      output_num = { 13'd2 , 3'd1 };
   end
   else if ( input_num == 20'd36 )
   begin
      output_num = { 13'd2 , 3'd1 };
   end
   else if ( input_num == 20'd37 )
   begin
      output_num = { 13'd2 , 3'd1 };
   end
   else if ( input_num == 20'd38 )
   begin
      output_num = { 13'd2 , 3'd2 };
   end
   else if ( input_num == 20'd39 )
   begin
      output_num = { 13'd2 , 3'd2 };
   end
   else if ( input_num == 20'd40 )
   begin
      output_num = { 13'd2 , 3'd3 };
   end
   else if ( input_num == 20'd41 )
   begin
      output_num = { 13'd2 , 3'd3 };
   end
   else if ( input_num == 20'd42 )
   begin
      output_num = { 13'd2 , 3'd4 };
   end
   else if ( input_num == 20'd43 )
   begin
      output_num = { 13'd2 , 3'd4 };
   end
   else if ( input_num == 20'd44 )
   begin
      output_num = { 13'd2 , 3'd5 };
   end
   else if ( input_num == 20'd45 )
   begin
      output_num = { 13'd2 , 3'd5 };
   end
   else if ( input_num == 20'd46 )
   begin
      output_num = { 13'd2 , 3'd6 };
   end
   else if ( input_num == 20'd47 )
   begin
      output_num = { 13'd2 , 3'd6 };
   end
   else if ( input_num == 20'd48 )
   begin
      output_num = { 13'd2 , 3'd7 };
   end
   else if ( input_num == 20'd49 )
   begin
      output_num = { 13'd2 , 3'd7 };
   end
   else if ( input_num == 20'd50 )
   begin
      output_num = { 13'd2 , 3'd7 };
   end
   else if ( input_num == 20'd51 )
   begin
      output_num = { 13'd3 , 3'd0 };
   end
   else if ( input_num == 20'd52 )
   begin
      output_num = { 13'd3 , 3'd0 };
   end
   else if ( input_num == 20'd53 )
   begin
      output_num = { 13'd3 , 3'd1 };
   end
   else if ( input_num == 20'd54 )
   begin
      output_num = { 13'd3 , 3'd1 };
   end
   else if ( input_num == 20'd55 )
   begin
      output_num = { 13'd3 , 3'd2 };
   end
   else if ( input_num == 20'd56 )
   begin
      output_num = { 13'd3 , 3'd2 };
   end
   else if ( input_num == 20'd57 )
   begin
      output_num = { 13'd3 , 3'd3 };
   end
   else if ( input_num == 20'd58 )
   begin
      output_num = { 13'd3 , 3'd3 };
   end
   else if ( input_num == 20'd59 )
   begin
      output_num = { 13'd3 , 3'd4 };
   end
   else if ( input_num == 20'd60 )
   begin
      output_num = { 13'd3 , 3'd4 };
   end
   else if ( input_num == 20'd61 )
   begin
      output_num = { 13'd3 , 3'd4 };
   end
   else if ( input_num == 20'd62 )
   begin
      output_num = { 13'd3 , 3'd5 };
   end
   else if ( input_num == 20'd63 )
   begin
      output_num = { 13'd3 , 3'd5 };
   end
   else if ( input_num == 20'd64 )
   begin
      output_num = { 13'd3 , 3'd6 };
   end
   else if ( input_num == 20'd65 )
   begin
      output_num = { 13'd3 , 3'd6 };
   end
   else if ( input_num == 20'd66 )
   begin
      output_num = { 13'd3 , 3'd7 };
   end
   else if ( input_num == 20'd67 )
   begin
      output_num = { 13'd3 , 3'd7 };
   end
   else if ( input_num == 20'd68 )
   begin
      output_num = { 13'd4 , 3'd0 };
   end
   else if ( input_num == 20'd69 )
   begin
      output_num = { 13'd4 , 3'd0 };
   end
   else if ( input_num == 20'd70 )
   begin
      output_num = { 13'd4 , 3'd1 };
   end
   else if ( input_num == 20'd71 )
   begin
      output_num = { 13'd4 , 3'd1 };
   end
   else if ( input_num == 20'd72 )
   begin
      output_num = { 13'd4 , 3'd2 };
   end
   else if ( input_num == 20'd73 )
   begin
      output_num = { 13'd4 , 3'd2 };
   end
   else if ( input_num == 20'd74 )
   begin
      output_num = { 13'd4 , 3'd2 };
   end
   else if ( input_num == 20'd75 )
   begin
      output_num = { 13'd4 , 3'd3 };
   end
   else if ( input_num == 20'd76 )
   begin
      output_num = { 13'd4 , 3'd3 };
   end
   else if ( input_num == 20'd77 )
   begin
      output_num = { 13'd4 , 3'd4 };
   end
   else if ( input_num == 20'd78 )
   begin
      output_num = { 13'd4 , 3'd4 };
   end
   else if ( input_num == 20'd79 )
   begin
      output_num = { 13'd4 , 3'd5 };
   end
   else if ( input_num == 20'd80 )
   begin
      output_num = { 13'd4 , 3'd5 };
   end
   else if ( input_num == 20'd81 )
   begin
      output_num = { 13'd4 , 3'd6 };
   end
   else if ( input_num == 20'd82 )
   begin
      output_num = { 13'd4 , 3'd6 };
   end
   else if ( input_num == 20'd83 )
   begin
      output_num = { 13'd4 , 3'd7 };
   end
   else if ( input_num == 20'd84 )
   begin
      output_num = { 13'd4 , 3'd7 };
   end
   else if ( input_num == 20'd85 )
   begin
      output_num = { 13'd5 , 3'd0 };
   end
   else if ( input_num == 20'd86 )
   begin
      output_num = { 13'd5 , 3'd0 };
   end
   else if ( input_num == 20'd87 )
   begin
      output_num = { 13'd5 , 3'd0 };
   end
   else if ( input_num == 20'd88 )
   begin
      output_num = { 13'd5 , 3'd1 };
   end
   else if ( input_num == 20'd89 )
   begin
      output_num = { 13'd5 , 3'd1 };
   end
   else if ( input_num == 20'd90 )
   begin
      output_num = { 13'd5 , 3'd2 };
   end
   else if ( input_num == 20'd91 )
   begin
      output_num = { 13'd5 , 3'd2 };
   end
   else if ( input_num == 20'd92 )
   begin
      output_num = { 13'd5 , 3'd3 };
   end
   else if ( input_num == 20'd93 )
   begin
      output_num = { 13'd5 , 3'd3 };
   end
   else if ( input_num == 20'd94 )
   begin
      output_num = { 13'd5 , 3'd4 };
   end
   else if ( input_num == 20'd95 )
   begin
      output_num = { 13'd5 , 3'd4 };
   end
   else if ( input_num == 20'd96 )
   begin
      output_num = { 13'd5 , 3'd5 };
   end
   else if ( input_num == 20'd97 )
   begin
      output_num = { 13'd5 , 3'd5 };
   end
   else if ( input_num == 20'd98 )
   begin
      output_num = { 13'd5 , 3'd5 };
   end
   else if ( input_num == 20'd99 )
   begin
      output_num = { 13'd5 , 3'd6 };
   end
   else if ( input_num == 20'd100 )
   begin
      output_num = { 13'd5 , 3'd6 };
   end
   else if ( input_num == 20'd101 )
   begin
      output_num = { 13'd5 , 3'd7 };
   end
   else if ( input_num == 20'd102 )
   begin
      output_num = { 13'd5 , 3'd7 };
   end
   else if ( input_num == 20'd103 )
   begin
      output_num = { 13'd6 , 3'd0 };
   end
   else if ( input_num == 20'd104 )
   begin
      output_num = { 13'd6 , 3'd0 };
   end
   else if ( input_num == 20'd105 )
   begin
      output_num = { 13'd6 , 3'd1 };
   end
   else if ( input_num == 20'd106 )
   begin
      output_num = { 13'd6 , 3'd1 };
   end
   else if ( input_num == 20'd107 )
   begin
      output_num = { 13'd6 , 3'd2 };
   end
   else if ( input_num == 20'd108 )
   begin
      output_num = { 13'd6 , 3'd2 };
   end
   else if ( input_num == 20'd109 )
   begin
      output_num = { 13'd6 , 3'd3 };
   end
   else if ( input_num == 20'd110 )
   begin
      output_num = { 13'd6 , 3'd3 };
   end
   else if ( input_num == 20'd111 )
   begin
      output_num = { 13'd6 , 3'd3 };
   end
   else if ( input_num == 20'd112 )
   begin
      output_num = { 13'd6 , 3'd4 };
   end
   else if ( input_num == 20'd113 )
   begin
      output_num = { 13'd6 , 3'd4 };
   end
   else if ( input_num == 20'd114 )
   begin
      output_num = { 13'd6 , 3'd5 };
   end
   else if ( input_num == 20'd115 )
   begin
      output_num = { 13'd6 , 3'd5 };
   end
   else if ( input_num == 20'd116 )
   begin
      output_num = { 13'd6 , 3'd6 };
   end
   else if ( input_num == 20'd117 )
   begin
      output_num = { 13'd6 , 3'd6 };
   end
   else if ( input_num == 20'd118 )
   begin
      output_num = { 13'd6 , 3'd7 };
   end
   else if ( input_num == 20'd119 )
   begin
      output_num = { 13'd6 , 3'd7 };
   end
   else if ( input_num == 20'd120 )
   begin
      output_num = { 13'd7 , 3'd0 };
   end
   else if ( input_num == 20'd121 )
   begin
      output_num = { 13'd7 , 3'd0 };
   end
   else if ( input_num == 20'd122 )
   begin
      output_num = { 13'd7 , 3'd1 };
   end
   else if ( input_num == 20'd123 )
   begin
      output_num = { 13'd7 , 3'd1 };
   end
   else if ( input_num == 20'd124 )
   begin
      output_num = { 13'd7 , 3'd1 };
   end
   else if ( input_num == 20'd125 )
   begin
      output_num = { 13'd7 , 3'd2 };
   end
   else if ( input_num == 20'd126 )
   begin
      output_num = { 13'd7 , 3'd2 };
   end
   else if ( input_num == 20'd127 )
   begin
      output_num = { 13'd7 , 3'd3 };
   end
   else if ( input_num == 20'd128 )
   begin
      output_num = { 13'd7 , 3'd3 };
   end
   else if ( input_num == 20'd129 )
   begin
      output_num = { 13'd7 , 3'd4 };
   end
   else if ( input_num == 20'd130 )
   begin
      output_num = { 13'd7 , 3'd4 };
   end
   else if ( input_num == 20'd131 )
   begin
      output_num = { 13'd7 , 3'd5 };
   end
   else if ( input_num == 20'd132 )
   begin
      output_num = { 13'd7 , 3'd5 };
   end
   else if ( input_num == 20'd133 )
   begin
      output_num = { 13'd7 , 3'd6 };
   end
   else if ( input_num == 20'd134 )
   begin
      output_num = { 13'd7 , 3'd6 };
   end
   else if ( input_num == 20'd135 )
   begin
      output_num = { 13'd7 , 3'd7 };
   end
   else if ( input_num == 20'd136 )
   begin
      output_num = { 13'd7 , 3'd7 };
   end
   else if ( input_num == 20'd137 )
   begin
      output_num = { 13'd7 , 3'd7 };
   end
   else if ( input_num == 20'd138 )
   begin
      output_num = { 13'd8 , 3'd0 };
   end
   else if ( input_num == 20'd139 )
   begin
      output_num = { 13'd8 , 3'd0 };
   end
   else if ( input_num == 20'd140 )
   begin
      output_num = { 13'd8 , 3'd1 };
   end
   else if ( input_num == 20'd141 )
   begin
      output_num = { 13'd8 , 3'd1 };
   end
   else if ( input_num == 20'd142 )
   begin
      output_num = { 13'd8 , 3'd2 };
   end
   else if ( input_num == 20'd143 )
   begin
      output_num = { 13'd8 , 3'd2 };
   end
   else if ( input_num == 20'd144 )
   begin
      output_num = { 13'd8 , 3'd3 };
   end
   else if ( input_num == 20'd145 )
   begin
      output_num = { 13'd8 , 3'd3 };
   end
   else if ( input_num == 20'd146 )
   begin
      output_num = { 13'd8 , 3'd4 };
   end
   else if ( input_num == 20'd147 )
   begin
      output_num = { 13'd8 , 3'd4 };
   end
   else if ( input_num == 20'd148 )
   begin
      output_num = { 13'd8 , 3'd5 };
   end
   else if ( input_num == 20'd149 )
   begin
      output_num = { 13'd8 , 3'd5 };
   end
   else if ( input_num == 20'd150 )
   begin
      output_num = { 13'd8 , 3'd6 };
   end
   else if ( input_num == 20'd151 )
   begin
      output_num = { 13'd8 , 3'd6 };
   end
   else if ( input_num == 20'd152 )
   begin
      output_num = { 13'd8 , 3'd6 };
   end
   else if ( input_num == 20'd153 )
   begin
      output_num = { 13'd8 , 3'd7 };
   end
   else if ( input_num == 20'd154 )
   begin
      output_num = { 13'd8 , 3'd7 };
   end
   else if ( input_num == 20'd155 )
   begin
      output_num = { 13'd9 , 3'd0 };
   end
   else if ( input_num == 20'd156 )
   begin
      output_num = { 13'd9 , 3'd0 };
   end
   else if ( input_num == 20'd157 )
   begin
      output_num = { 13'd9 , 3'd1 };
   end
   else if ( input_num == 20'd158 )
   begin
      output_num = { 13'd9 , 3'd1 };
   end
   else if ( input_num == 20'd159 )
   begin
      output_num = { 13'd9 , 3'd2 };
   end
   else if ( input_num == 20'd160 )
   begin
      output_num = { 13'd9 , 3'd2 };
   end
   else if ( input_num == 20'd161 )
   begin
      output_num = { 13'd9 , 3'd3 };
   end
   else if ( input_num == 20'd162 )
   begin
      output_num = { 13'd9 , 3'd3 };
   end
   else if ( input_num == 20'd163 )
   begin
      output_num = { 13'd9 , 3'd4 };
   end
   else if ( input_num == 20'd164 )
   begin
      output_num = { 13'd9 , 3'd4 };
   end
   else if ( input_num == 20'd165 )
   begin
      output_num = { 13'd9 , 3'd4 };
   end
   else if ( input_num == 20'd166 )
   begin
      output_num = { 13'd9 , 3'd5 };
   end
   else if ( input_num == 20'd167 )
   begin
      output_num = { 13'd9 , 3'd5 };
   end
   else if ( input_num == 20'd168 )
   begin
      output_num = { 13'd9 , 3'd6 };
   end
   else if ( input_num == 20'd169 )
   begin
      output_num = { 13'd9 , 3'd6 };
   end
   else if ( input_num == 20'd170 )
   begin
      output_num = { 13'd9 , 3'd7 };
   end
   else if ( input_num == 20'd171 )
   begin
      output_num = { 13'd9 , 3'd7 };
   end
   else if ( input_num == 20'd172 )
   begin
      output_num = { 13'd10 , 3'd0 };
   end
   else if ( input_num == 20'd173 )
   begin
      output_num = { 13'd10 , 3'd0 };
   end
   else if ( input_num == 20'd174 )
   begin
      output_num = { 13'd10 , 3'd1 };
   end
   else if ( input_num == 20'd175 )
   begin
      output_num = { 13'd10 , 3'd1 };
   end
   else if ( input_num == 20'd176 )
   begin
      output_num = { 13'd10 , 3'd2 };
   end
   else if ( input_num == 20'd177 )
   begin
      output_num = { 13'd10 , 3'd2 };
   end
   else if ( input_num == 20'd178 )
   begin
      output_num = { 13'd10 , 3'd3 };
   end
   else if ( input_num == 20'd179 )
   begin
      output_num = { 13'd10 , 3'd3 };
   end
   else if ( input_num == 20'd180 )
   begin
      output_num = { 13'd10 , 3'd3 };
   end
   else if ( input_num == 20'd181 )
   begin
      output_num = { 13'd10 , 3'd4 };
   end
   else if ( input_num == 20'd182 )
   begin
      output_num = { 13'd10 , 3'd4 };
   end
   else if ( input_num == 20'd183 )
   begin
      output_num = { 13'd10 , 3'd5 };
   end
   else if ( input_num == 20'd184 )
   begin
      output_num = { 13'd10 , 3'd5 };
   end
   else if ( input_num == 20'd185 )
   begin
      output_num = { 13'd10 , 3'd6 };
   end
   else if ( input_num == 20'd186 )
   begin
      output_num = { 13'd10 , 3'd6 };
   end
   else if ( input_num == 20'd187 )
   begin
      output_num = { 13'd10 , 3'd7 };
   end
   else if ( input_num == 20'd188 )
   begin
      output_num = { 13'd10 , 3'd7 };
   end
   else if ( input_num == 20'd189 )
   begin
      output_num = { 13'd11 , 3'd0 };
   end
   else if ( input_num == 20'd190 )
   begin
      output_num = { 13'd11 , 3'd0 };
   end
   else if ( input_num == 20'd191 )
   begin
      output_num = { 13'd11 , 3'd1 };
   end
   else if ( input_num == 20'd192 )
   begin
      output_num = { 13'd11 , 3'd1 };
   end
   else if ( input_num == 20'd193 )
   begin
      output_num = { 13'd11 , 3'd2 };
   end
   else if ( input_num == 20'd194 )
   begin
      output_num = { 13'd11 , 3'd2 };
   end
   else if ( input_num == 20'd195 )
   begin
      output_num = { 13'd11 , 3'd2 };
   end
   else if ( input_num == 20'd196 )
   begin
      output_num = { 13'd11 , 3'd3 };
   end
   else if ( input_num == 20'd197 )
   begin
      output_num = { 13'd11 , 3'd3 };
   end
   else if ( input_num == 20'd198 )
   begin
      output_num = { 13'd11 , 3'd4 };
   end
   else if ( input_num == 20'd199 )
   begin
      output_num = { 13'd11 , 3'd4 };
   end
   else if ( input_num == 20'd200 )
   begin
      output_num = { 13'd11 , 3'd5 };
   end
   else if ( input_num == 20'd201 )
   begin
      output_num = { 13'd11 , 3'd5 };
   end
   else if ( input_num == 20'd202 )
   begin
      output_num = { 13'd11 , 3'd6 };
   end
   else if ( input_num == 20'd203 )
   begin
      output_num = { 13'd11 , 3'd6 };
   end
   else if ( input_num == 20'd204 )
   begin
      output_num = { 13'd11 , 3'd7 };
   end
   else if ( input_num == 20'd205 )
   begin
      output_num = { 13'd11 , 3'd7 };
   end
   else if ( input_num == 20'd206 )
   begin
      output_num = { 13'd12 , 3'd0 };
   end
   else if ( input_num == 20'd207 )
   begin
      output_num = { 13'd12 , 3'd0 };
   end
   else if ( input_num == 20'd208 )
   begin
      output_num = { 13'd12 , 3'd1 };
   end
   else if ( input_num == 20'd209 )
   begin
      output_num = { 13'd12 , 3'd1 };
   end
   else if ( input_num == 20'd210 )
   begin
      output_num = { 13'd12 , 3'd1 };
   end
   else if ( input_num == 20'd211 )
   begin
      output_num = { 13'd12 , 3'd2 };
   end
   else if ( input_num == 20'd212 )
   begin
      output_num = { 13'd12 , 3'd2 };
   end
   else if ( input_num == 20'd213 )
   begin
      output_num = { 13'd12 , 3'd3 };
   end
   else if ( input_num == 20'd214 )
   begin
      output_num = { 13'd12 , 3'd3 };
   end
   else if ( input_num == 20'd215 )
   begin
      output_num = { 13'd12 , 3'd4 };
   end
   else if ( input_num == 20'd216 )
   begin
      output_num = { 13'd12 , 3'd4 };
   end
   else if ( input_num == 20'd217 )
   begin
      output_num = { 13'd12 , 3'd5 };
   end
   else if ( input_num == 20'd218 )
   begin
      output_num = { 13'd12 , 3'd5 };
   end
   else if ( input_num == 20'd219 )
   begin
      output_num = { 13'd12 , 3'd6 };
   end
   else if ( input_num == 20'd220 )
   begin
      output_num = { 13'd12 , 3'd6 };
   end
   else if ( input_num == 20'd221 )
   begin
      output_num = { 13'd12 , 3'd7 };
   end
   else if ( input_num == 20'd222 )
   begin
      output_num = { 13'd12 , 3'd7 };
   end
   else if ( input_num == 20'd223 )
   begin
      output_num = { 13'd13 , 3'd0 };
   end
   else if ( input_num == 20'd224 )
   begin
      output_num = { 13'd13 , 3'd0 };
   end
   else if ( input_num == 20'd225 )
   begin
      output_num = { 13'd13 , 3'd1 };
   end
   else if ( input_num == 20'd226 )
   begin
      output_num = { 13'd13 , 3'd1 };
   end
   else if ( input_num == 20'd227 )
   begin
      output_num = { 13'd13 , 3'd1 };
   end
   else if ( input_num == 20'd228 )
   begin
      output_num = { 13'd13 , 3'd2 };
   end
   else if ( input_num == 20'd229 )
   begin
      output_num = { 13'd13 , 3'd2 };
   end
   else if ( input_num == 20'd230 )
   begin
      output_num = { 13'd13 , 3'd3 };
   end
   else if ( input_num == 20'd231 )
   begin
      output_num = { 13'd13 , 3'd3 };
   end
   else if ( input_num == 20'd232 )
   begin
      output_num = { 13'd13 , 3'd4 };
   end
   else if ( input_num == 20'd233 )
   begin
      output_num = { 13'd13 , 3'd4 };
   end
   else if ( input_num == 20'd234 )
   begin
      output_num = { 13'd13 , 3'd5 };
   end
   else if ( input_num == 20'd235 )
   begin
      output_num = { 13'd13 , 3'd5 };
   end
   else if ( input_num == 20'd236 )
   begin
      output_num = { 13'd13 , 3'd6 };
   end
   else if ( input_num == 20'd237 )
   begin
      output_num = { 13'd13 , 3'd6 };
   end
   else if ( input_num == 20'd238 )
   begin
      output_num = { 13'd13 , 3'd7 };
   end
   else if ( input_num == 20'd239 )
   begin
      output_num = { 13'd13 , 3'd7 };
   end
   else if ( input_num == 20'd240 )
   begin
      output_num = { 13'd14 , 3'd0 };
   end
   else if ( input_num == 20'd241 )
   begin
      output_num = { 13'd14 , 3'd0 };
   end
   else if ( input_num == 20'd242 )
   begin
      output_num = { 13'd14 , 3'd1 };
   end
   else if ( input_num == 20'd243 )
   begin
      output_num = { 13'd14 , 3'd1 };
   end
   else if ( input_num == 20'd244 )
   begin
      output_num = { 13'd14 , 3'd1 };
   end
   else if ( input_num == 20'd245 )
   begin
      output_num = { 13'd14 , 3'd2 };
   end
   else if ( input_num == 20'd246 )
   begin
      output_num = { 13'd14 , 3'd2 };
   end
   else if ( input_num == 20'd247 )
   begin
      output_num = { 13'd14 , 3'd3 };
   end
   else if ( input_num == 20'd248 )
   begin
      output_num = { 13'd14 , 3'd3 };
   end
   else if ( input_num == 20'd249 )
   begin
      output_num = { 13'd14 , 3'd4 };
   end
   else if ( input_num == 20'd250 )
   begin
      output_num = { 13'd14 , 3'd4 };
   end
   else if ( input_num == 20'd251 )
   begin
      output_num = { 13'd14 , 3'd5 };
   end
   else if ( input_num == 20'd252 )
   begin
      output_num = { 13'd14 , 3'd5 };
   end
   else if ( input_num == 20'd253 )
   begin
      output_num = { 13'd14 , 3'd6 };
   end
   else if ( input_num == 20'd254 )
   begin
      output_num = { 13'd14 , 3'd6 };
   end
   else if ( input_num == 20'd255 )
   begin
      output_num = { 13'd14 , 3'd7 };
   end
   else if ( input_num == 20'd256 )
   begin
      output_num = { 13'd14 , 3'd7 };
   end
   else if ( input_num == 20'd257 )
   begin
      output_num = { 13'd15 , 3'd0 };
   end
   else if ( input_num == 20'd258 )
   begin
      output_num = { 13'd15 , 3'd0 };
   end
   else if ( input_num == 20'd259 )
   begin
      output_num = { 13'd15 , 3'd1 };
   end
   else if ( input_num == 20'd260 )
   begin
      output_num = { 13'd15 , 3'd1 };
   end
   else if ( input_num == 20'd261 )
   begin
      output_num = { 13'd15 , 3'd2 };
   end
   else if ( input_num == 20'd262 )
   begin
      output_num = { 13'd15 , 3'd2 };
   end
   else if ( input_num == 20'd263 )
   begin
      output_num = { 13'd15 , 3'd2 };
   end
   else if ( input_num == 20'd264 )
   begin
      output_num = { 13'd15 , 3'd3 };
   end
   else if ( input_num == 20'd265 )
   begin
      output_num = { 13'd15 , 3'd3 };
   end
   else if ( input_num == 20'd266 )
   begin
      output_num = { 13'd15 , 3'd4 };
   end
   else if ( input_num == 20'd267 )
   begin
      output_num = { 13'd15 , 3'd4 };
   end
   else if ( input_num == 20'd268 )
   begin
      output_num = { 13'd15 , 3'd5 };
   end
   else if ( input_num == 20'd269 )
   begin
      output_num = { 13'd15 , 3'd5 };
   end
   else if ( input_num == 20'd270 )
   begin
      output_num = { 13'd15 , 3'd6 };
   end
   else if ( input_num == 20'd271 )
   begin
      output_num = { 13'd15 , 3'd6 };
   end
   else if ( input_num == 20'd272 )
   begin
      output_num = { 13'd15 , 3'd7 };
   end
   else if ( input_num == 20'd273 )
   begin
      output_num = { 13'd15 , 3'd7 };
   end
   else if ( input_num == 20'd274 )
   begin
      output_num = { 13'd16 , 3'd0 };
   end
   else if ( input_num == 20'd275 )
   begin
      output_num = { 13'd16 , 3'd0 };
   end
   else if ( input_num == 20'd276 )
   begin
      output_num = { 13'd16 , 3'd1 };
   end
   else if ( input_num == 20'd277 )
   begin
      output_num = { 13'd16 , 3'd1 };
   end
   else if ( input_num == 20'd278 )
   begin
      output_num = { 13'd16 , 3'd2 };
   end
   else if ( input_num == 20'd279 )
   begin
      output_num = { 13'd16 , 3'd2 };
   end
   else if ( input_num == 20'd280 )
   begin
      output_num = { 13'd16 , 3'd3 };
   end
   else if ( input_num == 20'd281 )
   begin
      output_num = { 13'd16 , 3'd3 };
   end
   else if ( input_num == 20'd282 )
   begin
      output_num = { 13'd16 , 3'd4 };
   end
   else if ( input_num == 20'd283 )
   begin
      output_num = { 13'd16 , 3'd4 };
   end
   else if ( input_num == 20'd284 )
   begin
      output_num = { 13'd16 , 3'd4 };
   end
   else if ( input_num == 20'd285 )
   begin
      output_num = { 13'd16 , 3'd5 };
   end
   else if ( input_num == 20'd286 )
   begin
      output_num = { 13'd16 , 3'd5 };
   end
   else if ( input_num == 20'd287 )
   begin
      output_num = { 13'd16 , 3'd6 };
   end
   else if ( input_num == 20'd288 )
   begin
      output_num = { 13'd16 , 3'd6 };
   end
   else if ( input_num == 20'd289 )
   begin
      output_num = { 13'd16 , 3'd7 };
   end
   else if ( input_num == 20'd290 )
   begin
      output_num = { 13'd16 , 3'd7 };
   end
   else if ( input_num == 20'd291 )
   begin
      output_num = { 13'd17 , 3'd0 };
   end
   else if ( input_num == 20'd292 )
   begin
      output_num = { 13'd17 , 3'd0 };
   end
   else if ( input_num == 20'd293 )
   begin
      output_num = { 13'd17 , 3'd1 };
   end
   else if ( input_num == 20'd294 )
   begin
      output_num = { 13'd17 , 3'd1 };
   end
   else if ( input_num == 20'd295 )
   begin
      output_num = { 13'd17 , 3'd2 };
   end
   else if ( input_num == 20'd296 )
   begin
      output_num = { 13'd17 , 3'd2 };
   end
   else if ( input_num == 20'd297 )
   begin
      output_num = { 13'd17 , 3'd3 };
   end
   else if ( input_num == 20'd298 )
   begin
      output_num = { 13'd17 , 3'd3 };
   end
   else if ( input_num == 20'd299 )
   begin
      output_num = { 13'd17 , 3'd4 };
   end
   else if ( input_num == 20'd300 )
   begin
      output_num = { 13'd17 , 3'd4 };
   end
   else if ( input_num == 20'd301 )
   begin
      output_num = { 13'd17 , 3'd5 };
   end
   else if ( input_num == 20'd302 )
   begin
      output_num = { 13'd17 , 3'd5 };
   end
   else if ( input_num == 20'd303 )
   begin
      output_num = { 13'd17 , 3'd6 };
   end
   else if ( input_num == 20'd304 )
   begin
      output_num = { 13'd17 , 3'd6 };
   end
   else if ( input_num == 20'd305 )
   begin
      output_num = { 13'd17 , 3'd7 };
   end
   else if ( input_num == 20'd306 )
   begin
      output_num = { 13'd17 , 3'd7 };
   end
   else if ( input_num == 20'd307 )
   begin
      output_num = { 13'd18 , 3'd0 };
   end
   else if ( input_num == 20'd308 )
   begin
      output_num = { 13'd18 , 3'd0 };
   end
   else if ( input_num == 20'd309 )
   begin
      output_num = { 13'd18 , 3'd0 };
   end
   else if ( input_num == 20'd310 )
   begin
      output_num = { 13'd18 , 3'd1 };
   end
   else if ( input_num == 20'd311 )
   begin
      output_num = { 13'd18 , 3'd1 };
   end
   else if ( input_num == 20'd312 )
   begin
      output_num = { 13'd18 , 3'd2 };
   end
   else if ( input_num == 20'd313 )
   begin
      output_num = { 13'd18 , 3'd2 };
   end
   else if ( input_num == 20'd314 )
   begin
      output_num = { 13'd18 , 3'd3 };
   end
   else if ( input_num == 20'd315 )
   begin
      output_num = { 13'd18 , 3'd3 };
   end
   else if ( input_num == 20'd316 )
   begin
      output_num = { 13'd18 , 3'd4 };
   end
   else if ( input_num == 20'd317 )
   begin
      output_num = { 13'd18 , 3'd4 };
   end
   else if ( input_num == 20'd318 )
   begin
      output_num = { 13'd18 , 3'd5 };
   end
   else if ( input_num == 20'd319 )
   begin
      output_num = { 13'd18 , 3'd5 };
   end
   else if ( input_num == 20'd320 )
   begin
      output_num = { 13'd18 , 3'd6 };
   end
   else if ( input_num == 20'd321 )
   begin
      output_num = { 13'd18 , 3'd6 };
   end
   else if ( input_num == 20'd322 )
   begin
      output_num = { 13'd18 , 3'd7 };
   end
   else if ( input_num == 20'd323 )
   begin
      output_num = { 13'd18 , 3'd7 };
   end
   else if ( input_num == 20'd324 )
   begin
      output_num = { 13'd19 , 3'd0 };
   end
   else if ( input_num == 20'd325 )
   begin
      output_num = { 13'd19 , 3'd0 };
   end
   else if ( input_num == 20'd326 )
   begin
      output_num = { 13'd19 , 3'd1 };
   end
   else if ( input_num == 20'd327 )
   begin
      output_num = { 13'd19 , 3'd1 };
   end
   else if ( input_num == 20'd328 )
   begin
      output_num = { 13'd19 , 3'd2 };
   end
   else if ( input_num == 20'd329 )
   begin
      output_num = { 13'd19 , 3'd2 };
   end
   else if ( input_num == 20'd330 )
   begin
      output_num = { 13'd19 , 3'd3 };
   end
   else if ( input_num == 20'd331 )
   begin
      output_num = { 13'd19 , 3'd3 };
   end
   else if ( input_num == 20'd332 )
   begin
      output_num = { 13'd19 , 3'd4 };
   end
   else if ( input_num == 20'd333 )
   begin
      output_num = { 13'd19 , 3'd4 };
   end
   else if ( input_num == 20'd334 )
   begin
      output_num = { 13'd19 , 3'd5 };
   end
   else if ( input_num == 20'd335 )
   begin
      output_num = { 13'd19 , 3'd5 };
   end
   else if ( input_num == 20'd336 )
   begin
      output_num = { 13'd19 , 3'd6 };
   end
   else if ( input_num == 20'd337 )
   begin
      output_num = { 13'd19 , 3'd6 };
   end
   else if ( input_num == 20'd338 )
   begin
      output_num = { 13'd19 , 3'd7 };
   end
   else if ( input_num == 20'd339 )
   begin
      output_num = { 13'd19 , 3'd7 };
   end
   else if ( input_num == 20'd340 )
   begin
      output_num = { 13'd20 , 3'd0 };
   end
   else if ( input_num == 20'd341 )
   begin
      output_num = { 13'd20 , 3'd0 };
   end
   else if ( input_num == 20'd342 )
   begin
      output_num = { 13'd20 , 3'd0 };
   end
   else if ( input_num == 20'd343 )
   begin
      output_num = { 13'd20 , 3'd1 };
   end
   else if ( input_num == 20'd344 )
   begin
      output_num = { 13'd20 , 3'd1 };
   end
   else if ( input_num == 20'd345 )
   begin
      output_num = { 13'd20 , 3'd2 };
   end
   else if ( input_num == 20'd346 )
   begin
      output_num = { 13'd20 , 3'd2 };
   end
   else if ( input_num == 20'd347 )
   begin
      output_num = { 13'd20 , 3'd3 };
   end
   else if ( input_num == 20'd348 )
   begin
      output_num = { 13'd20 , 3'd3 };
   end
   else if ( input_num == 20'd349 )
   begin
      output_num = { 13'd20 , 3'd4 };
   end
   else if ( input_num == 20'd350 )
   begin
      output_num = { 13'd20 , 3'd4 };
   end
   else if ( input_num == 20'd351 )
   begin
      output_num = { 13'd20 , 3'd5 };
   end
   else if ( input_num == 20'd352 )
   begin
      output_num = { 13'd20 , 3'd5 };
   end
   else if ( input_num == 20'd353 )
   begin
      output_num = { 13'd20 , 3'd6 };
   end
   else if ( input_num == 20'd354 )
   begin
      output_num = { 13'd20 , 3'd6 };
   end
   else if ( input_num == 20'd355 )
   begin
      output_num = { 13'd20 , 3'd7 };
   end
   else if ( input_num == 20'd356 )
   begin
      output_num = { 13'd20 , 3'd7 };
   end
   else if ( input_num == 20'd357 )
   begin
      output_num = { 13'd21 , 3'd0 };
   end
   else if ( input_num == 20'd358 )
   begin
      output_num = { 13'd21 , 3'd0 };
   end
   else if ( input_num == 20'd359 )
   begin
      output_num = { 13'd21 , 3'd1 };
   end
   else if ( input_num == 20'd360 )
   begin
      output_num = { 13'd21 , 3'd1 };
   end
   else if ( input_num == 20'd361 )
   begin
      output_num = { 13'd21 , 3'd2 };
   end
   else if ( input_num == 20'd362 )
   begin
      output_num = { 13'd21 , 3'd2 };
   end
   else if ( input_num == 20'd363 )
   begin
      output_num = { 13'd21 , 3'd3 };
   end
   else if ( input_num == 20'd364 )
   begin
      output_num = { 13'd21 , 3'd3 };
   end
   else if ( input_num == 20'd365 )
   begin
      output_num = { 13'd21 , 3'd4 };
   end
   else if ( input_num == 20'd366 )
   begin
      output_num = { 13'd21 , 3'd4 };
   end
   else if ( input_num == 20'd367 )
   begin
      output_num = { 13'd21 , 3'd5 };
   end
   else if ( input_num == 20'd368 )
   begin
      output_num = { 13'd21 , 3'd5 };
   end
   else if ( input_num == 20'd369 )
   begin
      output_num = { 13'd21 , 3'd6 };
   end
   else if ( input_num == 20'd370 )
   begin
      output_num = { 13'd21 , 3'd6 };
   end
   else if ( input_num == 20'd371 )
   begin
      output_num = { 13'd21 , 3'd7 };
   end
   else if ( input_num == 20'd372 )
   begin
      output_num = { 13'd21 , 3'd7 };
   end
   else if ( input_num == 20'd373 )
   begin
      output_num = { 13'd22 , 3'd0 };
   end
   else if ( input_num == 20'd374 )
   begin
      output_num = { 13'd22 , 3'd0 };
   end
   else if ( input_num == 20'd375 )
   begin
      output_num = { 13'd22 , 3'd1 };
   end
   else if ( input_num == 20'd376 )
   begin
      output_num = { 13'd22 , 3'd1 };
   end
   else if ( input_num == 20'd377 )
   begin
      output_num = { 13'd22 , 3'd2 };
   end
   else if ( input_num == 20'd378 )
   begin
      output_num = { 13'd22 , 3'd2 };
   end
   else if ( input_num == 20'd379 )
   begin
      output_num = { 13'd22 , 3'd3 };
   end
   else if ( input_num == 20'd380 )
   begin
      output_num = { 13'd22 , 3'd3 };
   end
   else if ( input_num == 20'd381 )
   begin
      output_num = { 13'd22 , 3'd4 };
   end
   else if ( input_num == 20'd382 )
   begin
      output_num = { 13'd22 , 3'd4 };
   end
   else if ( input_num == 20'd383 )
   begin
      output_num = { 13'd22 , 3'd5 };
   end
   else if ( input_num == 20'd384 )
   begin
      output_num = { 13'd22 , 3'd5 };
   end
   else if ( input_num == 20'd385 )
   begin
      output_num = { 13'd22 , 3'd6 };
   end
   else if ( input_num == 20'd386 )
   begin
      output_num = { 13'd22 , 3'd6 };
   end
   else if ( input_num == 20'd387 )
   begin
      output_num = { 13'd22 , 3'd7 };
   end
   else if ( input_num == 20'd388 )
   begin
      output_num = { 13'd22 , 3'd7 };
   end
   else if ( input_num == 20'd389 )
   begin
      output_num = { 13'd23 , 3'd0 };
   end
   else if ( input_num == 20'd390 )
   begin
      output_num = { 13'd23 , 3'd0 };
   end
   else if ( input_num == 20'd391 )
   begin
      output_num = { 13'd23 , 3'd1 };
   end
   else if ( input_num == 20'd392 )
   begin
      output_num = { 13'd23 , 3'd1 };
   end
   else if ( input_num == 20'd393 )
   begin
      output_num = { 13'd23 , 3'd2 };
   end
   else if ( input_num == 20'd394 )
   begin
      output_num = { 13'd23 , 3'd2 };
   end
   else if ( input_num == 20'd395 )
   begin
      output_num = { 13'd23 , 3'd3 };
   end
   else if ( input_num == 20'd396 )
   begin
      output_num = { 13'd23 , 3'd3 };
   end
   else if ( input_num == 20'd397 )
   begin
      output_num = { 13'd23 , 3'd4 };
   end
   else if ( input_num == 20'd398 )
   begin
      output_num = { 13'd23 , 3'd4 };
   end
   else if ( input_num == 20'd399 )
   begin
      output_num = { 13'd23 , 3'd5 };
   end
   else if ( input_num == 20'd400 )
   begin
      output_num = { 13'd23 , 3'd5 };
   end
   else if ( input_num == 20'd401 )
   begin
      output_num = { 13'd23 , 3'd6 };
   end
   else if ( input_num == 20'd402 )
   begin
      output_num = { 13'd23 , 3'd6 };
   end
   else if ( input_num == 20'd403 )
   begin
      output_num = { 13'd23 , 3'd7 };
   end
   else if ( input_num == 20'd404 )
   begin
      output_num = { 13'd23 , 3'd7 };
   end
   else if ( input_num == 20'd405 )
   begin
      output_num = { 13'd24 , 3'd0 };
   end
   else if ( input_num == 20'd406 )
   begin
      output_num = { 13'd24 , 3'd0 };
   end
   else if ( input_num == 20'd407 )
   begin
      output_num = { 13'd24 , 3'd1 };
   end
   else if ( input_num == 20'd408 )
   begin
      output_num = { 13'd24 , 3'd1 };
   end
   else if ( input_num == 20'd409 )
   begin
      output_num = { 13'd24 , 3'd2 };
   end
   else if ( input_num == 20'd410 )
   begin
      output_num = { 13'd24 , 3'd2 };
   end
   else if ( input_num == 20'd411 )
   begin
      output_num = { 13'd24 , 3'd3 };
   end
   else if ( input_num == 20'd412 )
   begin
      output_num = { 13'd24 , 3'd3 };
   end
   else if ( input_num == 20'd413 )
   begin
      output_num = { 13'd24 , 3'd4 };
   end
   else if ( input_num == 20'd414 )
   begin
      output_num = { 13'd24 , 3'd4 };
   end
   else if ( input_num == 20'd415 )
   begin
      output_num = { 13'd24 , 3'd5 };
   end
   else if ( input_num == 20'd416 )
   begin
      output_num = { 13'd24 , 3'd5 };
   end
   else if ( input_num == 20'd417 )
   begin
      output_num = { 13'd24 , 3'd6 };
   end
   else if ( input_num == 20'd418 )
   begin
      output_num = { 13'd24 , 3'd6 };
   end
   else if ( input_num == 20'd419 )
   begin
      output_num = { 13'd24 , 3'd7 };
   end
   else if ( input_num == 20'd420 )
   begin
      output_num = { 13'd24 , 3'd7 };
   end
   else if ( input_num == 20'd421 )
   begin
      output_num = { 13'd25 , 3'd0 };
   end
   else if ( input_num == 20'd422 )
   begin
      output_num = { 13'd25 , 3'd0 };
   end
   else if ( input_num == 20'd423 )
   begin
      output_num = { 13'd25 , 3'd1 };
   end
   else if ( input_num == 20'd424 )
   begin
      output_num = { 13'd25 , 3'd1 };
   end
   else if ( input_num == 20'd425 )
   begin
      output_num = { 13'd25 , 3'd2 };
   end
   else if ( input_num == 20'd426 )
   begin
      output_num = { 13'd25 , 3'd2 };
   end
   else if ( input_num == 20'd427 )
   begin
      output_num = { 13'd25 , 3'd3 };
   end
   else if ( input_num == 20'd428 )
   begin
      output_num = { 13'd25 , 3'd3 };
   end
   else if ( input_num == 20'd429 )
   begin
      output_num = { 13'd25 , 3'd4 };
   end
   else if ( input_num == 20'd430 )
   begin
      output_num = { 13'd25 , 3'd4 };
   end
   else if ( input_num == 20'd431 )
   begin
      output_num = { 13'd25 , 3'd5 };
   end
   else if ( input_num == 20'd432 )
   begin
      output_num = { 13'd25 , 3'd5 };
   end
   else if ( input_num == 20'd433 )
   begin
      output_num = { 13'd25 , 3'd6 };
   end
   else if ( input_num == 20'd434 )
   begin
      output_num = { 13'd25 , 3'd6 };
   end
   else if ( input_num == 20'd435 )
   begin
      output_num = { 13'd25 , 3'd7 };
   end
   else if ( input_num == 20'd436 )
   begin
      output_num = { 13'd25 , 3'd7 };
   end
   else if ( input_num == 20'd437 )
   begin
      output_num = { 13'd26 , 3'd0 };
   end
   else if ( input_num == 20'd438 )
   begin
      output_num = { 13'd26 , 3'd0 };
   end
   else if ( input_num == 20'd439 )
   begin
      output_num = { 13'd26 , 3'd1 };
   end
   else if ( input_num == 20'd440 )
   begin
      output_num = { 13'd26 , 3'd1 };
   end
   else if ( input_num == 20'd441 )
   begin
      output_num = { 13'd26 , 3'd2 };
   end
   else if ( input_num == 20'd442 )
   begin
      output_num = { 13'd26 , 3'd2 };
   end
   else if ( input_num == 20'd443 )
   begin
      output_num = { 13'd26 , 3'd3 };
   end
   else if ( input_num == 20'd444 )
   begin
      output_num = { 13'd26 , 3'd3 };
   end
   else if ( input_num == 20'd445 )
   begin
      output_num = { 13'd26 , 3'd4 };
   end
   else if ( input_num == 20'd446 )
   begin
      output_num = { 13'd26 , 3'd4 };
   end
   else if ( input_num == 20'd447 )
   begin
      output_num = { 13'd26 , 3'd5 };
   end
   else if ( input_num == 20'd448 )
   begin
      output_num = { 13'd26 , 3'd5 };
   end
   else if ( input_num == 20'd449 )
   begin
      output_num = { 13'd26 , 3'd6 };
   end
   else if ( input_num == 20'd450 )
   begin
      output_num = { 13'd26 , 3'd6 };
   end
   else if ( input_num == 20'd451 )
   begin
      output_num = { 13'd26 , 3'd7 };
   end
   else if ( input_num == 20'd452 )
   begin
      output_num = { 13'd26 , 3'd7 };
   end
   else if ( input_num == 20'd453 )
   begin
      output_num = { 13'd27 , 3'd0 };
   end
   else if ( input_num == 20'd454 )
   begin
      output_num = { 13'd27 , 3'd1 };
   end
   else if ( input_num == 20'd455 )
   begin
      output_num = { 13'd27 , 3'd1 };
   end
   else if ( input_num == 20'd456 )
   begin
      output_num = { 13'd27 , 3'd2 };
   end
   else if ( input_num == 20'd457 )
   begin
      output_num = { 13'd27 , 3'd2 };
   end
   else if ( input_num == 20'd458 )
   begin
      output_num = { 13'd27 , 3'd3 };
   end
   else if ( input_num == 20'd459 )
   begin
      output_num = { 13'd27 , 3'd3 };
   end
   else if ( input_num == 20'd460 )
   begin
      output_num = { 13'd27 , 3'd4 };
   end
   else if ( input_num == 20'd461 )
   begin
      output_num = { 13'd27 , 3'd4 };
   end
   else if ( input_num == 20'd462 )
   begin
      output_num = { 13'd27 , 3'd5 };
   end
   else if ( input_num == 20'd463 )
   begin
      output_num = { 13'd27 , 3'd5 };
   end
   else if ( input_num == 20'd464 )
   begin
      output_num = { 13'd27 , 3'd6 };
   end
   else if ( input_num == 20'd465 )
   begin
      output_num = { 13'd27 , 3'd6 };
   end
   else if ( input_num == 20'd466 )
   begin
      output_num = { 13'd27 , 3'd7 };
   end
   else if ( input_num == 20'd467 )
   begin
      output_num = { 13'd27 , 3'd7 };
   end
   else if ( input_num == 20'd468 )
   begin
      output_num = { 13'd28 , 3'd0 };
   end
   else if ( input_num == 20'd469 )
   begin
      output_num = { 13'd28 , 3'd0 };
   end
   else if ( input_num == 20'd470 )
   begin
      output_num = { 13'd28 , 3'd1 };
   end
   else if ( input_num == 20'd471 )
   begin
      output_num = { 13'd28 , 3'd1 };
   end
   else if ( input_num == 20'd472 )
   begin
      output_num = { 13'd28 , 3'd2 };
   end
   else if ( input_num == 20'd473 )
   begin
      output_num = { 13'd28 , 3'd2 };
   end
   else if ( input_num == 20'd474 )
   begin
      output_num = { 13'd28 , 3'd3 };
   end
   else if ( input_num == 20'd475 )
   begin
      output_num = { 13'd28 , 3'd3 };
   end
   else if ( input_num == 20'd476 )
   begin
      output_num = { 13'd28 , 3'd4 };
   end
   else if ( input_num == 20'd477 )
   begin
      output_num = { 13'd28 , 3'd4 };
   end
   else if ( input_num == 20'd478 )
   begin
      output_num = { 13'd28 , 3'd5 };
   end
   else if ( input_num == 20'd479 )
   begin
      output_num = { 13'd28 , 3'd5 };
   end
   else if ( input_num == 20'd480 )
   begin
      output_num = { 13'd28 , 3'd6 };
   end
   else if ( input_num == 20'd481 )
   begin
      output_num = { 13'd28 , 3'd7 };
   end
   else if ( input_num == 20'd482 )
   begin
      output_num = { 13'd28 , 3'd7 };
   end
   else if ( input_num == 20'd483 )
   begin
      output_num = { 13'd29 , 3'd0 };
   end
   else if ( input_num == 20'd484 )
   begin
      output_num = { 13'd29 , 3'd0 };
   end
   else if ( input_num == 20'd485 )
   begin
      output_num = { 13'd29 , 3'd1 };
   end
   else if ( input_num == 20'd486 )
   begin
      output_num = { 13'd29 , 3'd1 };
   end
   else if ( input_num == 20'd487 )
   begin
      output_num = { 13'd29 , 3'd2 };
   end
   else if ( input_num == 20'd488 )
   begin
      output_num = { 13'd29 , 3'd2 };
   end
   else if ( input_num == 20'd489 )
   begin
      output_num = { 13'd29 , 3'd3 };
   end
   else if ( input_num == 20'd490 )
   begin
      output_num = { 13'd29 , 3'd3 };
   end
   else if ( input_num == 20'd491 )
   begin
      output_num = { 13'd29 , 3'd4 };
   end
   else if ( input_num == 20'd492 )
   begin
      output_num = { 13'd29 , 3'd4 };
   end
   else if ( input_num == 20'd493 )
   begin
      output_num = { 13'd29 , 3'd5 };
   end
   else if ( input_num == 20'd494 )
   begin
      output_num = { 13'd29 , 3'd5 };
   end
   else if ( input_num == 20'd495 )
   begin
      output_num = { 13'd29 , 3'd6 };
   end
   else if ( input_num == 20'd496 )
   begin
      output_num = { 13'd29 , 3'd6 };
   end
   else if ( input_num == 20'd497 )
   begin
      output_num = { 13'd29 , 3'd7 };
   end
   else if ( input_num == 20'd498 )
   begin
      output_num = { 13'd29 , 3'd7 };
   end
   else if ( input_num == 20'd499 )
   begin
      output_num = { 13'd30 , 3'd0 };
   end
   else if ( input_num == 20'd500 )
   begin
      output_num = { 13'd30 , 3'd1 };
   end
   else if ( input_num == 20'd501 )
   begin
      output_num = { 13'd30 , 3'd1 };
   end
   else if ( input_num == 20'd502 )
   begin
      output_num = { 13'd30 , 3'd2 };
   end
   else if ( input_num == 20'd503 )
   begin
      output_num = { 13'd30 , 3'd2 };
   end
   else if ( input_num == 20'd504 )
   begin
      output_num = { 13'd30 , 3'd3 };
   end
   else if ( input_num == 20'd505 )
   begin
      output_num = { 13'd30 , 3'd3 };
   end
   else if ( input_num == 20'd506 )
   begin
      output_num = { 13'd30 , 3'd4 };
   end
   else if ( input_num == 20'd507 )
   begin
      output_num = { 13'd30 , 3'd4 };
   end
   else if ( input_num == 20'd508 )
   begin
      output_num = { 13'd30 , 3'd5 };
   end
   else if ( input_num == 20'd509 )
   begin
      output_num = { 13'd30 , 3'd5 };
   end
   else if ( input_num == 20'd510 )
   begin
      output_num = { 13'd30 , 3'd6 };
   end
   else if ( input_num == 20'd511 )
   begin
      output_num = { 13'd30 , 3'd6 };
   end
   else if ( input_num == 20'd512 )
   begin
      output_num = { 13'd30 , 3'd7 };
   end
   else if ( input_num == 20'd513 )
   begin
      output_num = { 13'd30 , 3'd7 };
   end
   else if ( input_num == 20'd514 )
   begin
      output_num = { 13'd31 , 3'd0 };
   end
   else if ( input_num == 20'd515 )
   begin
      output_num = { 13'd31 , 3'd0 };
   end
   else if ( input_num == 20'd516 )
   begin
      output_num = { 13'd31 , 3'd1 };
   end
   else if ( input_num == 20'd517 )
   begin
      output_num = { 13'd31 , 3'd2 };
   end
   else if ( input_num == 20'd518 )
   begin
      output_num = { 13'd31 , 3'd2 };
   end
   else if ( input_num == 20'd519 )
   begin
      output_num = { 13'd31 , 3'd3 };
   end
   else if ( input_num == 20'd520 )
   begin
      output_num = { 13'd31 , 3'd3 };
   end
   else if ( input_num == 20'd521 )
   begin
      output_num = { 13'd31 , 3'd4 };
   end
   else if ( input_num == 20'd522 )
   begin
      output_num = { 13'd31 , 3'd4 };
   end
   else if ( input_num == 20'd523 )
   begin
      output_num = { 13'd31 , 3'd5 };
   end
   else if ( input_num == 20'd524 )
   begin
      output_num = { 13'd31 , 3'd5 };
   end
   else if ( input_num == 20'd525 )
   begin
      output_num = { 13'd31 , 3'd6 };
   end
   else if ( input_num == 20'd526 )
   begin
      output_num = { 13'd31 , 3'd6 };
   end
   else if ( input_num == 20'd527 )
   begin
      output_num = { 13'd31 , 3'd7 };
   end
   else if ( input_num == 20'd528 )
   begin
      output_num = { 13'd31 , 3'd7 };
   end
   else if ( input_num == 20'd529 )
   begin
      output_num = { 13'd32 , 3'd0 };
   end
   else if ( input_num == 20'd530 )
   begin
      output_num = { 13'd32 , 3'd1 };
   end
   else if ( input_num == 20'd531 )
   begin
      output_num = { 13'd32 , 3'd1 };
   end
   else if ( input_num == 20'd532 )
   begin
      output_num = { 13'd32 , 3'd2 };
   end
   else if ( input_num == 20'd533 )
   begin
      output_num = { 13'd32 , 3'd2 };
   end
   else if ( input_num == 20'd534 )
   begin
      output_num = { 13'd32 , 3'd3 };
   end
   else if ( input_num == 20'd535 )
   begin
      output_num = { 13'd32 , 3'd3 };
   end
   else if ( input_num == 20'd536 )
   begin
      output_num = { 13'd32 , 3'd4 };
   end
   else if ( input_num == 20'd537 )
   begin
      output_num = { 13'd32 , 3'd4 };
   end
   else if ( input_num == 20'd538 )
   begin
      output_num = { 13'd32 , 3'd5 };
   end
   else if ( input_num == 20'd539 )
   begin
      output_num = { 13'd32 , 3'd5 };
   end
   else if ( input_num == 20'd540 )
   begin
      output_num = { 13'd32 , 3'd6 };
   end
   else if ( input_num == 20'd541 )
   begin
      output_num = { 13'd32 , 3'd7 };
   end
   else if ( input_num == 20'd542 )
   begin
      output_num = { 13'd32 , 3'd7 };
   end
   else if ( input_num == 20'd543 )
   begin
      output_num = { 13'd33 , 3'd0 };
   end
   else if ( input_num == 20'd544 )
   begin
      output_num = { 13'd33 , 3'd0 };
   end
   else if ( input_num == 20'd545 )
   begin
      output_num = { 13'd33 , 3'd1 };
   end
   else if ( input_num == 20'd546 )
   begin
      output_num = { 13'd33 , 3'd1 };
   end
   else if ( input_num == 20'd547 )
   begin
      output_num = { 13'd33 , 3'd2 };
   end
   else if ( input_num == 20'd548 )
   begin
      output_num = { 13'd33 , 3'd2 };
   end
   else if ( input_num == 20'd549 )
   begin
      output_num = { 13'd33 , 3'd3 };
   end
   else if ( input_num == 20'd550 )
   begin
      output_num = { 13'd33 , 3'd3 };
   end
   else if ( input_num == 20'd551 )
   begin
      output_num = { 13'd33 , 3'd4 };
   end
   else if ( input_num == 20'd552 )
   begin
      output_num = { 13'd33 , 3'd5 };
   end
   else if ( input_num == 20'd553 )
   begin
      output_num = { 13'd33 , 3'd5 };
   end
   else if ( input_num == 20'd554 )
   begin
      output_num = { 13'd33 , 3'd6 };
   end
   else if ( input_num == 20'd555 )
   begin
      output_num = { 13'd33 , 3'd6 };
   end
   else if ( input_num == 20'd556 )
   begin
      output_num = { 13'd33 , 3'd7 };
   end
   else if ( input_num == 20'd557 )
   begin
      output_num = { 13'd33 , 3'd7 };
   end
   else if ( input_num == 20'd558 )
   begin
      output_num = { 13'd34 , 3'd0 };
   end
   else if ( input_num == 20'd559 )
   begin
      output_num = { 13'd34 , 3'd0 };
   end
   else if ( input_num == 20'd560 )
   begin
      output_num = { 13'd34 , 3'd1 };
   end
   else if ( input_num == 20'd561 )
   begin
      output_num = { 13'd34 , 3'd1 };
   end
   else if ( input_num == 20'd562 )
   begin
      output_num = { 13'd34 , 3'd2 };
   end
   else if ( input_num == 20'd563 )
   begin
      output_num = { 13'd34 , 3'd3 };
   end
   else if ( input_num == 20'd564 )
   begin
      output_num = { 13'd34 , 3'd3 };
   end
   else if ( input_num == 20'd565 )
   begin
      output_num = { 13'd34 , 3'd4 };
   end
   else if ( input_num == 20'd566 )
   begin
      output_num = { 13'd34 , 3'd4 };
   end
   else if ( input_num == 20'd567 )
   begin
      output_num = { 13'd34 , 3'd5 };
   end
   else if ( input_num == 20'd568 )
   begin
      output_num = { 13'd34 , 3'd5 };
   end
   else if ( input_num == 20'd569 )
   begin
      output_num = { 13'd34 , 3'd6 };
   end
   else if ( input_num == 20'd570 )
   begin
      output_num = { 13'd34 , 3'd7 };
   end
   else if ( input_num == 20'd571 )
   begin
      output_num = { 13'd34 , 3'd7 };
   end
   else if ( input_num == 20'd572 )
   begin
      output_num = { 13'd35 , 3'd0 };
   end
   else if ( input_num == 20'd573 )
   begin
      output_num = { 13'd35 , 3'd0 };
   end
   else if ( input_num == 20'd574 )
   begin
      output_num = { 13'd35 , 3'd1 };
   end
   else if ( input_num == 20'd575 )
   begin
      output_num = { 13'd35 , 3'd1 };
   end
   else if ( input_num == 20'd576 )
   begin
      output_num = { 13'd35 , 3'd2 };
   end
   else if ( input_num == 20'd577 )
   begin
      output_num = { 13'd35 , 3'd2 };
   end
   else if ( input_num == 20'd578 )
   begin
      output_num = { 13'd35 , 3'd3 };
   end
   else if ( input_num == 20'd579 )
   begin
      output_num = { 13'd35 , 3'd4 };
   end
   else if ( input_num == 20'd580 )
   begin
      output_num = { 13'd35 , 3'd4 };
   end
   else if ( input_num == 20'd581 )
   begin
      output_num = { 13'd35 , 3'd5 };
   end
   else if ( input_num == 20'd582 )
   begin
      output_num = { 13'd35 , 3'd5 };
   end
   else if ( input_num == 20'd583 )
   begin
      output_num = { 13'd35 , 3'd6 };
   end
   else if ( input_num == 20'd584 )
   begin
      output_num = { 13'd35 , 3'd6 };
   end
   else if ( input_num == 20'd585 )
   begin
      output_num = { 13'd35 , 3'd7 };
   end
   else if ( input_num == 20'd586 )
   begin
      output_num = { 13'd35 , 3'd7 };
   end
   else if ( input_num == 20'd587 )
   begin
      output_num = { 13'd36 , 3'd0 };
   end
   else if ( input_num == 20'd588 )
   begin
      output_num = { 13'd36 , 3'd1 };
   end
   else if ( input_num == 20'd589 )
   begin
      output_num = { 13'd36 , 3'd1 };
   end
   else if ( input_num == 20'd590 )
   begin
      output_num = { 13'd36 , 3'd2 };
   end
   else if ( input_num == 20'd591 )
   begin
      output_num = { 13'd36 , 3'd2 };
   end
   else if ( input_num == 20'd592 )
   begin
      output_num = { 13'd36 , 3'd3 };
   end
   else if ( input_num == 20'd593 )
   begin
      output_num = { 13'd36 , 3'd3 };
   end
   else if ( input_num == 20'd594 )
   begin
      output_num = { 13'd36 , 3'd4 };
   end
   else if ( input_num == 20'd595 )
   begin
      output_num = { 13'd36 , 3'd5 };
   end
   else if ( input_num == 20'd596 )
   begin
      output_num = { 13'd36 , 3'd5 };
   end
   else if ( input_num == 20'd597 )
   begin
      output_num = { 13'd36 , 3'd6 };
   end
   else if ( input_num == 20'd598 )
   begin
      output_num = { 13'd36 , 3'd6 };
   end
   else if ( input_num == 20'd599 )
   begin
      output_num = { 13'd36 , 3'd7 };
   end
   else if ( input_num == 20'd600 )
   begin
      output_num = { 13'd36 , 3'd7 };
   end
   else if ( input_num == 20'd601 )
   begin
      output_num = { 13'd37 , 3'd0 };
   end
   else if ( input_num == 20'd602 )
   begin
      output_num = { 13'd37 , 3'd1 };
   end
   else if ( input_num == 20'd603 )
   begin
      output_num = { 13'd37 , 3'd1 };
   end
   else if ( input_num == 20'd604 )
   begin
      output_num = { 13'd37 , 3'd2 };
   end
   else if ( input_num == 20'd605 )
   begin
      output_num = { 13'd37 , 3'd2 };
   end
   else if ( input_num == 20'd606 )
   begin
      output_num = { 13'd37 , 3'd3 };
   end
   else if ( input_num == 20'd607 )
   begin
      output_num = { 13'd37 , 3'd3 };
   end
   else if ( input_num == 20'd608 )
   begin
      output_num = { 13'd37 , 3'd4 };
   end
   else if ( input_num == 20'd609 )
   begin
      output_num = { 13'd37 , 3'd5 };
   end
   else if ( input_num == 20'd610 )
   begin
      output_num = { 13'd37 , 3'd5 };
   end
   else if ( input_num == 20'd611 )
   begin
      output_num = { 13'd37 , 3'd6 };
   end
   else if ( input_num == 20'd612 )
   begin
      output_num = { 13'd37 , 3'd6 };
   end
   else if ( input_num == 20'd613 )
   begin
      output_num = { 13'd37 , 3'd7 };
   end
   else if ( input_num == 20'd614 )
   begin
      output_num = { 13'd38 , 3'd0 };
   end
   else if ( input_num == 20'd615 )
   begin
      output_num = { 13'd38 , 3'd0 };
   end
   else if ( input_num == 20'd616 )
   begin
      output_num = { 13'd38 , 3'd1 };
   end
   else if ( input_num == 20'd617 )
   begin
      output_num = { 13'd38 , 3'd1 };
   end
   else if ( input_num == 20'd618 )
   begin
      output_num = { 13'd38 , 3'd2 };
   end
   else if ( input_num == 20'd619 )
   begin
      output_num = { 13'd38 , 3'd2 };
   end
   else if ( input_num == 20'd620 )
   begin
      output_num = { 13'd38 , 3'd3 };
   end
   else if ( input_num == 20'd621 )
   begin
      output_num = { 13'd38 , 3'd4 };
   end
   else if ( input_num == 20'd622 )
   begin
      output_num = { 13'd38 , 3'd4 };
   end
   else if ( input_num == 20'd623 )
   begin
      output_num = { 13'd38 , 3'd5 };
   end
   else if ( input_num == 20'd624 )
   begin
      output_num = { 13'd38 , 3'd5 };
   end
   else if ( input_num == 20'd625 )
   begin
      output_num = { 13'd38 , 3'd6 };
   end
   else if ( input_num == 20'd626 )
   begin
      output_num = { 13'd38 , 3'd7 };
   end
   else if ( input_num == 20'd627 )
   begin
      output_num = { 13'd38 , 3'd7 };
   end
   else if ( input_num == 20'd628 )
   begin
      output_num = { 13'd39 , 3'd0 };
   end
   else if ( input_num == 20'd629 )
   begin
      output_num = { 13'd39 , 3'd0 };
   end
   else if ( input_num == 20'd630 )
   begin
      output_num = { 13'd39 , 3'd1 };
   end
   else if ( input_num == 20'd631 )
   begin
      output_num = { 13'd39 , 3'd1 };
   end
   else if ( input_num == 20'd632 )
   begin
      output_num = { 13'd39 , 3'd2 };
   end
   else if ( input_num == 20'd633 )
   begin
      output_num = { 13'd39 , 3'd3 };
   end
   else if ( input_num == 20'd634 )
   begin
      output_num = { 13'd39 , 3'd3 };
   end
   else if ( input_num == 20'd635 )
   begin
      output_num = { 13'd39 , 3'd4 };
   end
   else if ( input_num == 20'd636 )
   begin
      output_num = { 13'd39 , 3'd4 };
   end
   else if ( input_num == 20'd637 )
   begin
      output_num = { 13'd39 , 3'd5 };
   end
   else if ( input_num == 20'd638 )
   begin
      output_num = { 13'd39 , 3'd6 };
   end
   else if ( input_num == 20'd639 )
   begin
      output_num = { 13'd39 , 3'd6 };
   end
   else if ( input_num == 20'd640 )
   begin
      output_num = { 13'd39 , 3'd7 };
   end
   else if ( input_num == 20'd641 )
   begin
      output_num = { 13'd39 , 3'd7 };
   end
   else if ( input_num == 20'd642 )
   begin
      output_num = { 13'd40 , 3'd0 };
   end
   else if ( input_num == 20'd643 )
   begin
      output_num = { 13'd40 , 3'd1 };
   end
   else if ( input_num == 20'd644 )
   begin
      output_num = { 13'd40 , 3'd1 };
   end
   else if ( input_num == 20'd645 )
   begin
      output_num = { 13'd40 , 3'd2 };
   end
   else if ( input_num == 20'd646 )
   begin
      output_num = { 13'd40 , 3'd2 };
   end
   else if ( input_num == 20'd647 )
   begin
      output_num = { 13'd40 , 3'd3 };
   end
   else if ( input_num == 20'd648 )
   begin
      output_num = { 13'd40 , 3'd4 };
   end
   else if ( input_num == 20'd649 )
   begin
      output_num = { 13'd40 , 3'd4 };
   end
   else if ( input_num == 20'd650 )
   begin
      output_num = { 13'd40 , 3'd5 };
   end
   else if ( input_num == 20'd651 )
   begin
      output_num = { 13'd40 , 3'd5 };
   end
   else if ( input_num == 20'd652 )
   begin
      output_num = { 13'd40 , 3'd6 };
   end
   else if ( input_num == 20'd653 )
   begin
      output_num = { 13'd40 , 3'd7 };
   end
   else if ( input_num == 20'd654 )
   begin
      output_num = { 13'd40 , 3'd7 };
   end
   else if ( input_num == 20'd655 )
   begin
      output_num = { 13'd41 , 3'd0 };
   end
   else if ( input_num == 20'd656 )
   begin
      output_num = { 13'd41 , 3'd0 };
   end
   else if ( input_num == 20'd657 )
   begin
      output_num = { 13'd41 , 3'd1 };
   end
   else if ( input_num == 20'd658 )
   begin
      output_num = { 13'd41 , 3'd2 };
   end
   else if ( input_num == 20'd659 )
   begin
      output_num = { 13'd41 , 3'd2 };
   end
   else if ( input_num == 20'd660 )
   begin
      output_num = { 13'd41 , 3'd3 };
   end
   else if ( input_num == 20'd661 )
   begin
      output_num = { 13'd41 , 3'd4 };
   end
   else if ( input_num == 20'd662 )
   begin
      output_num = { 13'd41 , 3'd4 };
   end
   else if ( input_num == 20'd663 )
   begin
      output_num = { 13'd41 , 3'd5 };
   end
   else if ( input_num == 20'd664 )
   begin
      output_num = { 13'd41 , 3'd5 };
   end
   else if ( input_num == 20'd665 )
   begin
      output_num = { 13'd41 , 3'd6 };
   end
   else if ( input_num == 20'd666 )
   begin
      output_num = { 13'd41 , 3'd7 };
   end
   else if ( input_num == 20'd667 )
   begin
      output_num = { 13'd41 , 3'd7 };
   end
   else if ( input_num == 20'd668 )
   begin
      output_num = { 13'd42 , 3'd0 };
   end
   else if ( input_num == 20'd669 )
   begin
      output_num = { 13'd42 , 3'd0 };
   end
   else if ( input_num == 20'd670 )
   begin
      output_num = { 13'd42 , 3'd1 };
   end
   else if ( input_num == 20'd671 )
   begin
      output_num = { 13'd42 , 3'd2 };
   end
   else if ( input_num == 20'd672 )
   begin
      output_num = { 13'd42 , 3'd2 };
   end
   else if ( input_num == 20'd673 )
   begin
      output_num = { 13'd42 , 3'd3 };
   end
   else if ( input_num == 20'd674 )
   begin
      output_num = { 13'd42 , 3'd4 };
   end
   else if ( input_num == 20'd675 )
   begin
      output_num = { 13'd42 , 3'd4 };
   end
   else if ( input_num == 20'd676 )
   begin
      output_num = { 13'd42 , 3'd5 };
   end
   else if ( input_num == 20'd677 )
   begin
      output_num = { 13'd42 , 3'd5 };
   end
   else if ( input_num == 20'd678 )
   begin
      output_num = { 13'd42 , 3'd6 };
   end
   else if ( input_num == 20'd679 )
   begin
      output_num = { 13'd42 , 3'd7 };
   end
   else if ( input_num == 20'd680 )
   begin
      output_num = { 13'd42 , 3'd7 };
   end
   else if ( input_num == 20'd681 )
   begin
      output_num = { 13'd43 , 3'd0 };
   end
   else if ( input_num == 20'd682 )
   begin
      output_num = { 13'd43 , 3'd1 };
   end
   else if ( input_num == 20'd683 )
   begin
      output_num = { 13'd43 , 3'd1 };
   end
   else if ( input_num == 20'd684 )
   begin
      output_num = { 13'd43 , 3'd2 };
   end
   else if ( input_num == 20'd685 )
   begin
      output_num = { 13'd43 , 3'd2 };
   end
   else if ( input_num == 20'd686 )
   begin
      output_num = { 13'd43 , 3'd3 };
   end
   else if ( input_num == 20'd687 )
   begin
      output_num = { 13'd43 , 3'd4 };
   end
   else if ( input_num == 20'd688 )
   begin
      output_num = { 13'd43 , 3'd4 };
   end
   else if ( input_num == 20'd689 )
   begin
      output_num = { 13'd43 , 3'd5 };
   end
   else if ( input_num == 20'd690 )
   begin
      output_num = { 13'd43 , 3'd6 };
   end
   else if ( input_num == 20'd691 )
   begin
      output_num = { 13'd43 , 3'd6 };
   end
   else if ( input_num == 20'd692 )
   begin
      output_num = { 13'd43 , 3'd7 };
   end
   else if ( input_num == 20'd693 )
   begin
      output_num = { 13'd43 , 3'd7 };
   end
   else if ( input_num == 20'd694 )
   begin
      output_num = { 13'd44 , 3'd0 };
   end
   else if ( input_num == 20'd695 )
   begin
      output_num = { 13'd44 , 3'd1 };
   end
   else if ( input_num == 20'd696 )
   begin
      output_num = { 13'd44 , 3'd1 };
   end
   else if ( input_num == 20'd697 )
   begin
      output_num = { 13'd44 , 3'd2 };
   end
   else if ( input_num == 20'd698 )
   begin
      output_num = { 13'd44 , 3'd3 };
   end
   else if ( input_num == 20'd699 )
   begin
      output_num = { 13'd44 , 3'd3 };
   end
   else if ( input_num == 20'd700 )
   begin
      output_num = { 13'd44 , 3'd4 };
   end
   else if ( input_num == 20'd701 )
   begin
      output_num = { 13'd44 , 3'd5 };
   end
   else if ( input_num == 20'd702 )
   begin
      output_num = { 13'd44 , 3'd5 };
   end
   else if ( input_num == 20'd703 )
   begin
      output_num = { 13'd44 , 3'd6 };
   end
   else if ( input_num == 20'd704 )
   begin
      output_num = { 13'd44 , 3'd6 };
   end
   else if ( input_num == 20'd705 )
   begin
      output_num = { 13'd44 , 3'd7 };
   end
   else if ( input_num == 20'd706 )
   begin
      output_num = { 13'd45 , 3'd0 };
   end
   else if ( input_num == 20'd707 )
   begin
      output_num = { 13'd45 , 3'd0 };
   end
   else if ( input_num == 20'd708 )
   begin
      output_num = { 13'd45 , 3'd1 };
   end
   else if ( input_num == 20'd709 )
   begin
      output_num = { 13'd45 , 3'd2 };
   end
   else if ( input_num == 20'd710 )
   begin
      output_num = { 13'd45 , 3'd2 };
   end
   else if ( input_num == 20'd711 )
   begin
      output_num = { 13'd45 , 3'd3 };
   end
   else if ( input_num == 20'd712 )
   begin
      output_num = { 13'd45 , 3'd4 };
   end
   else if ( input_num == 20'd713 )
   begin
      output_num = { 13'd45 , 3'd4 };
   end
   else if ( input_num == 20'd714 )
   begin
      output_num = { 13'd45 , 3'd5 };
   end
   else if ( input_num == 20'd715 )
   begin
      output_num = { 13'd45 , 3'd6 };
   end
   else if ( input_num == 20'd716 )
   begin
      output_num = { 13'd45 , 3'd6 };
   end
   else if ( input_num == 20'd717 )
   begin
      output_num = { 13'd45 , 3'd7 };
   end
   else if ( input_num == 20'd718 )
   begin
      output_num = { 13'd46 , 3'd0 };
   end
   else if ( input_num == 20'd719 )
   begin
      output_num = { 13'd46 , 3'd0 };
   end
   else if ( input_num == 20'd720 )
   begin
      output_num = { 13'd46 , 3'd1 };
   end
   else if ( input_num == 20'd721 )
   begin
      output_num = { 13'd46 , 3'd2 };
   end
   else if ( input_num == 20'd722 )
   begin
      output_num = { 13'd46 , 3'd2 };
   end
   else if ( input_num == 20'd723 )
   begin
      output_num = { 13'd46 , 3'd3 };
   end
   else if ( input_num == 20'd724 )
   begin
      output_num = { 13'd46 , 3'd4 };
   end
   else if ( input_num == 20'd725 )
   begin
      output_num = { 13'd46 , 3'd4 };
   end
   else if ( input_num == 20'd726 )
   begin
      output_num = { 13'd46 , 3'd5 };
   end
   else if ( input_num == 20'd727 )
   begin
      output_num = { 13'd46 , 3'd6 };
   end
   else if ( input_num == 20'd728 )
   begin
      output_num = { 13'd46 , 3'd6 };
   end
   else if ( input_num == 20'd729 )
   begin
      output_num = { 13'd46 , 3'd7 };
   end
   else if ( input_num == 20'd730 )
   begin
      output_num = { 13'd47 , 3'd0 };
   end
   else if ( input_num == 20'd731 )
   begin
      output_num = { 13'd47 , 3'd0 };
   end
   else if ( input_num == 20'd732 )
   begin
      output_num = { 13'd47 , 3'd1 };
   end
   else if ( input_num == 20'd733 )
   begin
      output_num = { 13'd47 , 3'd2 };
   end
   else if ( input_num == 20'd734 )
   begin
      output_num = { 13'd47 , 3'd2 };
   end
   else if ( input_num == 20'd735 )
   begin
      output_num = { 13'd47 , 3'd3 };
   end
   else if ( input_num == 20'd736 )
   begin
      output_num = { 13'd47 , 3'd4 };
   end
   else if ( input_num == 20'd737 )
   begin
      output_num = { 13'd47 , 3'd4 };
   end
   else if ( input_num == 20'd738 )
   begin
      output_num = { 13'd47 , 3'd5 };
   end
   else if ( input_num == 20'd739 )
   begin
      output_num = { 13'd47 , 3'd6 };
   end
   else if ( input_num == 20'd740 )
   begin
      output_num = { 13'd47 , 3'd6 };
   end
   else if ( input_num == 20'd741 )
   begin
      output_num = { 13'd47 , 3'd7 };
   end
   else if ( input_num == 20'd742 )
   begin
      output_num = { 13'd48 , 3'd0 };
   end
   else if ( input_num == 20'd743 )
   begin
      output_num = { 13'd48 , 3'd0 };
   end
   else if ( input_num == 20'd744 )
   begin
      output_num = { 13'd48 , 3'd1 };
   end
   else if ( input_num == 20'd745 )
   begin
      output_num = { 13'd48 , 3'd2 };
   end
   else if ( input_num == 20'd746 )
   begin
      output_num = { 13'd48 , 3'd2 };
   end
   else if ( input_num == 20'd747 )
   begin
      output_num = { 13'd48 , 3'd3 };
   end
   else if ( input_num == 20'd748 )
   begin
      output_num = { 13'd48 , 3'd4 };
   end
   else if ( input_num == 20'd749 )
   begin
      output_num = { 13'd48 , 3'd5 };
   end
   else if ( input_num == 20'd750 )
   begin
      output_num = { 13'd48 , 3'd5 };
   end
   else if ( input_num == 20'd751 )
   begin
      output_num = { 13'd48 , 3'd6 };
   end
   else if ( input_num == 20'd752 )
   begin
      output_num = { 13'd48 , 3'd7 };
   end
   else if ( input_num == 20'd753 )
   begin
      output_num = { 13'd48 , 3'd7 };
   end
   else if ( input_num == 20'd754 )
   begin
      output_num = { 13'd49 , 3'd0 };
   end
   else if ( input_num == 20'd755 )
   begin
      output_num = { 13'd49 , 3'd1 };
   end
   else if ( input_num == 20'd756 )
   begin
      output_num = { 13'd49 , 3'd1 };
   end
   else if ( input_num == 20'd757 )
   begin
      output_num = { 13'd49 , 3'd2 };
   end
   else if ( input_num == 20'd758 )
   begin
      output_num = { 13'd49 , 3'd3 };
   end
   else if ( input_num == 20'd759 )
   begin
      output_num = { 13'd49 , 3'd4 };
   end
   else if ( input_num == 20'd760 )
   begin
      output_num = { 13'd49 , 3'd4 };
   end
   else if ( input_num == 20'd761 )
   begin
      output_num = { 13'd49 , 3'd5 };
   end
   else if ( input_num == 20'd762 )
   begin
      output_num = { 13'd49 , 3'd6 };
   end
   else if ( input_num == 20'd763 )
   begin
      output_num = { 13'd49 , 3'd6 };
   end
   else if ( input_num == 20'd764 )
   begin
      output_num = { 13'd49 , 3'd7 };
   end
   else if ( input_num == 20'd765 )
   begin
      output_num = { 13'd50 , 3'd0 };
   end
   else if ( input_num == 20'd766 )
   begin
      output_num = { 13'd50 , 3'd0 };
   end
   else if ( input_num == 20'd767 )
   begin
      output_num = { 13'd50 , 3'd1 };
   end
   else if ( input_num == 20'd768 )
   begin
      output_num = { 13'd50 , 3'd2 };
   end
   else if ( input_num == 20'd769 )
   begin
      output_num = { 13'd50 , 3'd3 };
   end
   else if ( input_num == 20'd770 )
   begin
      output_num = { 13'd50 , 3'd3 };
   end
   else if ( input_num == 20'd771 )
   begin
      output_num = { 13'd50 , 3'd4 };
   end
   else if ( input_num == 20'd772 )
   begin
      output_num = { 13'd50 , 3'd5 };
   end
   else if ( input_num == 20'd773 )
   begin
      output_num = { 13'd50 , 3'd5 };
   end
   else if ( input_num == 20'd774 )
   begin
      output_num = { 13'd50 , 3'd6 };
   end
   else if ( input_num == 20'd775 )
   begin
      output_num = { 13'd50 , 3'd7 };
   end
   else if ( input_num == 20'd776 )
   begin
      output_num = { 13'd51 , 3'd0 };
   end
   else if ( input_num == 20'd777 )
   begin
      output_num = { 13'd51 , 3'd0 };
   end
   else if ( input_num == 20'd778 )
   begin
      output_num = { 13'd51 , 3'd1 };
   end
   else if ( input_num == 20'd779 )
   begin
      output_num = { 13'd51 , 3'd2 };
   end
   else if ( input_num == 20'd780 )
   begin
      output_num = { 13'd51 , 3'd3 };
   end
   else if ( input_num == 20'd781 )
   begin
      output_num = { 13'd51 , 3'd3 };
   end
   else if ( input_num == 20'd782 )
   begin
      output_num = { 13'd51 , 3'd4 };
   end
   else if ( input_num == 20'd783 )
   begin
      output_num = { 13'd51 , 3'd5 };
   end
   else if ( input_num == 20'd784 )
   begin
      output_num = { 13'd51 , 3'd6 };
   end
   else if ( input_num == 20'd785 )
   begin
      output_num = { 13'd51 , 3'd6 };
   end
   else if ( input_num == 20'd786 )
   begin
      output_num = { 13'd51 , 3'd7 };
   end
   else if ( input_num == 20'd787 )
   begin
      output_num = { 13'd52 , 3'd0 };
   end
   else if ( input_num == 20'd788 )
   begin
      output_num = { 13'd52 , 3'd0 };
   end
   else if ( input_num == 20'd789 )
   begin
      output_num = { 13'd52 , 3'd1 };
   end
   else if ( input_num == 20'd790 )
   begin
      output_num = { 13'd52 , 3'd2 };
   end
   else if ( input_num == 20'd791 )
   begin
      output_num = { 13'd52 , 3'd3 };
   end
   else if ( input_num == 20'd792 )
   begin
      output_num = { 13'd52 , 3'd3 };
   end
   else if ( input_num == 20'd793 )
   begin
      output_num = { 13'd52 , 3'd4 };
   end
   else if ( input_num == 20'd794 )
   begin
      output_num = { 13'd52 , 3'd5 };
   end
   else if ( input_num == 20'd795 )
   begin
      output_num = { 13'd52 , 3'd6 };
   end
   else if ( input_num == 20'd796 )
   begin
      output_num = { 13'd52 , 3'd6 };
   end
   else if ( input_num == 20'd797 )
   begin
      output_num = { 13'd52 , 3'd7 };
   end
   else if ( input_num == 20'd798 )
   begin
      output_num = { 13'd53 , 3'd0 };
   end
   else if ( input_num == 20'd799 )
   begin
      output_num = { 13'd53 , 3'd1 };
   end
   else if ( input_num == 20'd800 )
   begin
      output_num = { 13'd53 , 3'd2 };
   end
   else if ( input_num == 20'd801 )
   begin
      output_num = { 13'd53 , 3'd2 };
   end
   else if ( input_num == 20'd802 )
   begin
      output_num = { 13'd53 , 3'd3 };
   end
   else if ( input_num == 20'd803 )
   begin
      output_num = { 13'd53 , 3'd4 };
   end
   else if ( input_num == 20'd804 )
   begin
      output_num = { 13'd53 , 3'd5 };
   end
   else if ( input_num == 20'd805 )
   begin
      output_num = { 13'd53 , 3'd5 };
   end
   else if ( input_num == 20'd806 )
   begin
      output_num = { 13'd53 , 3'd6 };
   end
   else if ( input_num == 20'd807 )
   begin
      output_num = { 13'd53 , 3'd7 };
   end
   else if ( input_num == 20'd808 )
   begin
      output_num = { 13'd54 , 3'd0 };
   end
   else if ( input_num == 20'd809 )
   begin
      output_num = { 13'd54 , 3'd0 };
   end
   else if ( input_num == 20'd810 )
   begin
      output_num = { 13'd54 , 3'd1 };
   end
   else if ( input_num == 20'd811 )
   begin
      output_num = { 13'd54 , 3'd2 };
   end
   else if ( input_num == 20'd812 )
   begin
      output_num = { 13'd54 , 3'd3 };
   end
   else if ( input_num == 20'd813 )
   begin
      output_num = { 13'd54 , 3'd4 };
   end
   else if ( input_num == 20'd814 )
   begin
      output_num = { 13'd54 , 3'd4 };
   end
   else if ( input_num == 20'd815 )
   begin
      output_num = { 13'd54 , 3'd5 };
   end
   else if ( input_num == 20'd816 )
   begin
      output_num = { 13'd54 , 3'd6 };
   end
   else if ( input_num == 20'd817 )
   begin
      output_num = { 13'd54 , 3'd7 };
   end
   else if ( input_num == 20'd818 )
   begin
      output_num = { 13'd55 , 3'd0 };
   end
   else if ( input_num == 20'd819 )
   begin
      output_num = { 13'd55 , 3'd0 };
   end
   else if ( input_num == 20'd820 )
   begin
      output_num = { 13'd55 , 3'd1 };
   end
   else if ( input_num == 20'd821 )
   begin
      output_num = { 13'd55 , 3'd2 };
   end
   else if ( input_num == 20'd822 )
   begin
      output_num = { 13'd55 , 3'd3 };
   end
   else if ( input_num == 20'd823 )
   begin
      output_num = { 13'd55 , 3'd4 };
   end
   else if ( input_num == 20'd824 )
   begin
      output_num = { 13'd55 , 3'd4 };
   end
   else if ( input_num == 20'd825 )
   begin
      output_num = { 13'd55 , 3'd5 };
   end
   else if ( input_num == 20'd826 )
   begin
      output_num = { 13'd55 , 3'd6 };
   end
   else if ( input_num == 20'd827 )
   begin
      output_num = { 13'd55 , 3'd7 };
   end
   else if ( input_num == 20'd828 )
   begin
      output_num = { 13'd56 , 3'd0 };
   end
   else if ( input_num == 20'd829 )
   begin
      output_num = { 13'd56 , 3'd0 };
   end
   else if ( input_num == 20'd830 )
   begin
      output_num = { 13'd56 , 3'd1 };
   end
   else if ( input_num == 20'd831 )
   begin
      output_num = { 13'd56 , 3'd2 };
   end
   else if ( input_num == 20'd832 )
   begin
      output_num = { 13'd56 , 3'd3 };
   end
   else if ( input_num == 20'd833 )
   begin
      output_num = { 13'd56 , 3'd4 };
   end
   else if ( input_num == 20'd834 )
   begin
      output_num = { 13'd56 , 3'd5 };
   end
   else if ( input_num == 20'd835 )
   begin
      output_num = { 13'd56 , 3'd5 };
   end
   else if ( input_num == 20'd836 )
   begin
      output_num = { 13'd56 , 3'd6 };
   end
   else if ( input_num == 20'd837 )
   begin
      output_num = { 13'd56 , 3'd7 };
   end
   else if ( input_num == 20'd838 )
   begin
      output_num = { 13'd57 , 3'd0 };
   end
   else if ( input_num == 20'd839 )
   begin
      output_num = { 13'd57 , 3'd1 };
   end
   else if ( input_num == 20'd840 )
   begin
      output_num = { 13'd57 , 3'd2 };
   end
   else if ( input_num == 20'd841 )
   begin
      output_num = { 13'd57 , 3'd2 };
   end
   else if ( input_num == 20'd842 )
   begin
      output_num = { 13'd57 , 3'd3 };
   end
   else if ( input_num == 20'd843 )
   begin
      output_num = { 13'd57 , 3'd4 };
   end
   else if ( input_num == 20'd844 )
   begin
      output_num = { 13'd57 , 3'd5 };
   end
   else if ( input_num == 20'd845 )
   begin
      output_num = { 13'd57 , 3'd6 };
   end
   else if ( input_num == 20'd846 )
   begin
      output_num = { 13'd57 , 3'd7 };
   end
   else if ( input_num == 20'd847 )
   begin
      output_num = { 13'd58 , 3'd0 };
   end
   else if ( input_num == 20'd848 )
   begin
      output_num = { 13'd58 , 3'd0 };
   end
   else if ( input_num == 20'd849 )
   begin
      output_num = { 13'd58 , 3'd1 };
   end
   else if ( input_num == 20'd850 )
   begin
      output_num = { 13'd58 , 3'd2 };
   end
   else if ( input_num == 20'd851 )
   begin
      output_num = { 13'd58 , 3'd3 };
   end
   else if ( input_num == 20'd852 )
   begin
      output_num = { 13'd58 , 3'd4 };
   end
   else if ( input_num == 20'd853 )
   begin
      output_num = { 13'd58 , 3'd5 };
   end
   else if ( input_num == 20'd854 )
   begin
      output_num = { 13'd58 , 3'd6 };
   end
   else if ( input_num == 20'd855 )
   begin
      output_num = { 13'd58 , 3'd7 };
   end
   else if ( input_num == 20'd856 )
   begin
      output_num = { 13'd58 , 3'd7 };
   end
   else if ( input_num == 20'd857 )
   begin
      output_num = { 13'd59 , 3'd0 };
   end
   else if ( input_num == 20'd858 )
   begin
      output_num = { 13'd59 , 3'd1 };
   end
   else if ( input_num == 20'd859 )
   begin
      output_num = { 13'd59 , 3'd2 };
   end
   else if ( input_num == 20'd860 )
   begin
      output_num = { 13'd59 , 3'd3 };
   end
   else if ( input_num == 20'd861 )
   begin
      output_num = { 13'd59 , 3'd4 };
   end
   else if ( input_num == 20'd862 )
   begin
      output_num = { 13'd59 , 3'd5 };
   end
   else if ( input_num == 20'd863 )
   begin
      output_num = { 13'd59 , 3'd6 };
   end
   else if ( input_num == 20'd864 )
   begin
      output_num = { 13'd59 , 3'd7 };
   end
   else if ( input_num == 20'd865 )
   begin
      output_num = { 13'd60 , 3'd0 };
   end
   else if ( input_num == 20'd866 )
   begin
      output_num = { 13'd60 , 3'd0 };
   end
   else if ( input_num == 20'd867 )
   begin
      output_num = { 13'd60 , 3'd1 };
   end
   else if ( input_num == 20'd868 )
   begin
      output_num = { 13'd60 , 3'd2 };
   end
   else if ( input_num == 20'd869 )
   begin
      output_num = { 13'd60 , 3'd3 };
   end
   else if ( input_num == 20'd870 )
   begin
      output_num = { 13'd60 , 3'd4 };
   end
   else if ( input_num == 20'd871 )
   begin
      output_num = { 13'd60 , 3'd5 };
   end
   else if ( input_num == 20'd872 )
   begin
      output_num = { 13'd60 , 3'd6 };
   end
   else if ( input_num == 20'd873 )
   begin
      output_num = { 13'd60 , 3'd7 };
   end
   else if ( input_num == 20'd874 )
   begin
      output_num = { 13'd61 , 3'd0 };
   end
   else if ( input_num == 20'd875 )
   begin
      output_num = { 13'd61 , 3'd1 };
   end
   else if ( input_num == 20'd876 )
   begin
      output_num = { 13'd61 , 3'd2 };
   end
   else if ( input_num == 20'd877 )
   begin
      output_num = { 13'd61 , 3'd3 };
   end
   else if ( input_num == 20'd878 )
   begin
      output_num = { 13'd61 , 3'd4 };
   end
   else if ( input_num == 20'd879 )
   begin
      output_num = { 13'd61 , 3'd5 };
   end
   else if ( input_num == 20'd880 )
   begin
      output_num = { 13'd61 , 3'd6 };
   end
   else if ( input_num == 20'd881 )
   begin
      output_num = { 13'd61 , 3'd7 };
   end
   else if ( input_num == 20'd882 )
   begin
      output_num = { 13'd62 , 3'd0 };
   end
   else if ( input_num == 20'd883 )
   begin
      output_num = { 13'd62 , 3'd1 };
   end
   else if ( input_num == 20'd884 )
   begin
      output_num = { 13'd62 , 3'd2 };
   end
   else if ( input_num == 20'd885 )
   begin
      output_num = { 13'd62 , 3'd3 };
   end
   else if ( input_num == 20'd886 )
   begin
      output_num = { 13'd62 , 3'd3 };
   end
   else if ( input_num == 20'd887 )
   begin
      output_num = { 13'd62 , 3'd4 };
   end
   else if ( input_num == 20'd888 )
   begin
      output_num = { 13'd62 , 3'd5 };
   end
   else if ( input_num == 20'd889 )
   begin
      output_num = { 13'd62 , 3'd6 };
   end
   else if ( input_num == 20'd890 )
   begin
      output_num = { 13'd62 , 3'd7 };
   end
   else if ( input_num == 20'd891 )
   begin
      output_num = { 13'd63 , 3'd0 };
   end
   else if ( input_num == 20'd892 )
   begin
      output_num = { 13'd63 , 3'd2 };
   end
   else if ( input_num == 20'd893 )
   begin
      output_num = { 13'd63 , 3'd3 };
   end
   else if ( input_num == 20'd894 )
   begin
      output_num = { 13'd63 , 3'd4 };
   end
   else if ( input_num == 20'd895 )
   begin
      output_num = { 13'd63 , 3'd5 };
   end
   else if ( input_num == 20'd896 )
   begin
      output_num = { 13'd63 , 3'd6 };
   end
   else if ( input_num == 20'd897 )
   begin
      output_num = { 13'd63 , 3'd7 };
   end
   else if ( input_num == 20'd898 )
   begin
      output_num = { 13'd64 , 3'd0 };
   end
   else if ( input_num == 20'd899 )
   begin
      output_num = { 13'd64 , 3'd1 };
   end
   else if ( input_num == 20'd900 )
   begin
      output_num = { 13'd64 , 3'd2 };
   end
   else if ( input_num == 20'd901 )
   begin
      output_num = { 13'd64 , 3'd3 };
   end
   else if ( input_num == 20'd902 )
   begin
      output_num = { 13'd64 , 3'd4 };
   end
   else if ( input_num == 20'd903 )
   begin
      output_num = { 13'd64 , 3'd5 };
   end
   else if ( input_num == 20'd904 )
   begin
      output_num = { 13'd64 , 3'd6 };
   end
   else if ( input_num == 20'd905 )
   begin
      output_num = { 13'd64 , 3'd7 };
   end
   else if ( input_num == 20'd906 )
   begin
      output_num = { 13'd65 , 3'd0 };
   end
   else if ( input_num == 20'd907 )
   begin
      output_num = { 13'd65 , 3'd1 };
   end
   else if ( input_num == 20'd908 )
   begin
      output_num = { 13'd65 , 3'd2 };
   end
   else if ( input_num == 20'd909 )
   begin
      output_num = { 13'd65 , 3'd3 };
   end
   else if ( input_num == 20'd910 )
   begin
      output_num = { 13'd65 , 3'd5 };
   end
   else if ( input_num == 20'd911 )
   begin
      output_num = { 13'd65 , 3'd6 };
   end
   else if ( input_num == 20'd912 )
   begin
      output_num = { 13'd65 , 3'd7 };
   end
   else if ( input_num == 20'd913 )
   begin
      output_num = { 13'd66 , 3'd0 };
   end
   else if ( input_num == 20'd914 )
   begin
      output_num = { 13'd66 , 3'd1 };
   end
   else if ( input_num == 20'd915 )
   begin
      output_num = { 13'd66 , 3'd2 };
   end
   else if ( input_num == 20'd916 )
   begin
      output_num = { 13'd66 , 3'd3 };
   end
   else if ( input_num == 20'd917 )
   begin
      output_num = { 13'd66 , 3'd4 };
   end
   else if ( input_num == 20'd918 )
   begin
      output_num = { 13'd66 , 3'd6 };
   end
   else if ( input_num == 20'd919 )
   begin
      output_num = { 13'd66 , 3'd7 };
   end
   else if ( input_num == 20'd920 )
   begin
      output_num = { 13'd67 , 3'd0 };
   end
   else if ( input_num == 20'd921 )
   begin
      output_num = { 13'd67 , 3'd1 };
   end
   else if ( input_num == 20'd922 )
   begin
      output_num = { 13'd67 , 3'd2 };
   end
   else if ( input_num == 20'd923 )
   begin
      output_num = { 13'd67 , 3'd3 };
   end
   else if ( input_num == 20'd924 )
   begin
      output_num = { 13'd67 , 3'd5 };
   end
   else if ( input_num == 20'd925 )
   begin
      output_num = { 13'd67 , 3'd6 };
   end
   else if ( input_num == 20'd926 )
   begin
      output_num = { 13'd67 , 3'd7 };
   end
   else if ( input_num == 20'd927 )
   begin
      output_num = { 13'd68 , 3'd0 };
   end
   else if ( input_num == 20'd928 )
   begin
      output_num = { 13'd68 , 3'd2 };
   end
   else if ( input_num == 20'd929 )
   begin
      output_num = { 13'd68 , 3'd3 };
   end
   else if ( input_num == 20'd930 )
   begin
      output_num = { 13'd68 , 3'd4 };
   end
   else if ( input_num == 20'd931 )
   begin
      output_num = { 13'd68 , 3'd5 };
   end
   else if ( input_num == 20'd932 )
   begin
      output_num = { 13'd68 , 3'd6 };
   end
   else if ( input_num == 20'd933 )
   begin
      output_num = { 13'd69 , 3'd0 };
   end
   else if ( input_num == 20'd934 )
   begin
      output_num = { 13'd69 , 3'd1 };
   end
   else if ( input_num == 20'd935 )
   begin
      output_num = { 13'd69 , 3'd2 };
   end
   else if ( input_num == 20'd936 )
   begin
      output_num = { 13'd69 , 3'd4 };
   end
   else if ( input_num == 20'd937 )
   begin
      output_num = { 13'd69 , 3'd5 };
   end
   else if ( input_num == 20'd938 )
   begin
      output_num = { 13'd69 , 3'd6 };
   end
   else if ( input_num == 20'd939 )
   begin
      output_num = { 13'd70 , 3'd0 };
   end
   else if ( input_num == 20'd940 )
   begin
      output_num = { 13'd70 , 3'd1 };
   end
   else if ( input_num == 20'd941 )
   begin
      output_num = { 13'd70 , 3'd2 };
   end
   else if ( input_num == 20'd942 )
   begin
      output_num = { 13'd70 , 3'd4 };
   end
   else if ( input_num == 20'd943 )
   begin
      output_num = { 13'd70 , 3'd5 };
   end
   else if ( input_num == 20'd944 )
   begin
      output_num = { 13'd70 , 3'd6 };
   end
   else if ( input_num == 20'd945 )
   begin
      output_num = { 13'd71 , 3'd0 };
   end
   else if ( input_num == 20'd946 )
   begin
      output_num = { 13'd71 , 3'd1 };
   end
   else if ( input_num == 20'd947 )
   begin
      output_num = { 13'd71 , 3'd3 };
   end
   else if ( input_num == 20'd948 )
   begin
      output_num = { 13'd71 , 3'd4 };
   end
   else if ( input_num == 20'd949 )
   begin
      output_num = { 13'd71 , 3'd5 };
   end
   else if ( input_num == 20'd950 )
   begin
      output_num = { 13'd71 , 3'd7 };
   end
   else if ( input_num == 20'd951 )
   begin
      output_num = { 13'd72 , 3'd0 };
   end
   else if ( input_num == 20'd952 )
   begin
      output_num = { 13'd72 , 3'd2 };
   end
   else if ( input_num == 20'd953 )
   begin
      output_num = { 13'd72 , 3'd3 };
   end
   else if ( input_num == 20'd954 )
   begin
      output_num = { 13'd72 , 3'd5 };
   end
   else if ( input_num == 20'd955 )
   begin
      output_num = { 13'd72 , 3'd6 };
   end
   else if ( input_num == 20'd956 )
   begin
      output_num = { 13'd73 , 3'd0 };
   end
   else if ( input_num == 20'd957 )
   begin
      output_num = { 13'd73 , 3'd2 };
   end
   else if ( input_num == 20'd958 )
   begin
      output_num = { 13'd73 , 3'd3 };
   end
   else if ( input_num == 20'd959 )
   begin
      output_num = { 13'd73 , 3'd5 };
   end
   else if ( input_num == 20'd960 )
   begin
      output_num = { 13'd73 , 3'd6 };
   end
   else if ( input_num == 20'd961 )
   begin
      output_num = { 13'd74 , 3'd0 };
   end
   else if ( input_num == 20'd962 )
   begin
      output_num = { 13'd74 , 3'd2 };
   end
   else if ( input_num == 20'd963 )
   begin
      output_num = { 13'd74 , 3'd3 };
   end
   else if ( input_num == 20'd964 )
   begin
      output_num = { 13'd74 , 3'd5 };
   end
   else if ( input_num == 20'd965 )
   begin
      output_num = { 13'd74 , 3'd7 };
   end
   else if ( input_num == 20'd966 )
   begin
      output_num = { 13'd75 , 3'd1 };
   end
   else if ( input_num == 20'd967 )
   begin
      output_num = { 13'd75 , 3'd2 };
   end
   else if ( input_num == 20'd968 )
   begin
      output_num = { 13'd75 , 3'd4 };
   end
   else if ( input_num == 20'd969 )
   begin
      output_num = { 13'd75 , 3'd6 };
   end
   else if ( input_num == 20'd970 )
   begin
      output_num = { 13'd76 , 3'd0 };
   end
   else if ( input_num == 20'd971 )
   begin
      output_num = { 13'd76 , 3'd2 };
   end
   else if ( input_num == 20'd972 )
   begin
      output_num = { 13'd76 , 3'd4 };
   end
   else if ( input_num == 20'd973 )
   begin
      output_num = { 13'd76 , 3'd6 };
   end
   else if ( input_num == 20'd974 )
   begin
      output_num = { 13'd77 , 3'd0 };
   end
   else if ( input_num == 20'd975 )
   begin
      output_num = { 13'd77 , 3'd2 };
   end
   else if ( input_num == 20'd976 )
   begin
      output_num = { 13'd77 , 3'd4 };
   end
   else if ( input_num == 20'd977 )
   begin
      output_num = { 13'd77 , 3'd6 };
   end
   else if ( input_num == 20'd978 )
   begin
      output_num = { 13'd78 , 3'd0 };
   end
   else if ( input_num == 20'd979 )
   begin
      output_num = { 13'd78 , 3'd2 };
   end
   else if ( input_num == 20'd980 )
   begin
      output_num = { 13'd78 , 3'd5 };
   end
   else if ( input_num == 20'd981 )
   begin
      output_num = { 13'd78 , 3'd7 };
   end
   else if ( input_num == 20'd982 )
   begin
      output_num = { 13'd79 , 3'd1 };
   end
   else if ( input_num == 20'd983 )
   begin
      output_num = { 13'd79 , 3'd4 };
   end
   else if ( input_num == 20'd984 )
   begin
      output_num = { 13'd79 , 3'd6 };
   end
   else if ( input_num == 20'd985 )
   begin
      output_num = { 13'd80 , 3'd1 };
   end
   else if ( input_num == 20'd986 )
   begin
      output_num = { 13'd80 , 3'd4 };
   end
   else if ( input_num == 20'd987 )
   begin
      output_num = { 13'd80 , 3'd7 };
   end
   else if ( input_num == 20'd988 )
   begin
      output_num = { 13'd81 , 3'd1 };
   end
   else if ( input_num == 20'd989 )
   begin
      output_num = { 13'd81 , 3'd4 };
   end
   else if ( input_num == 20'd990 )
   begin
      output_num = { 13'd82 , 3'd0 };
   end
   else if ( input_num == 20'd991 )
   begin
      output_num = { 13'd82 , 3'd3 };
   end
   else if ( input_num == 20'd992 )
   begin
      output_num = { 13'd82 , 3'd6 };
   end
   else if ( input_num == 20'd993 )
   begin
      output_num = { 13'd83 , 3'd2 };
   end
   else if ( input_num == 20'd994 )
   begin
      output_num = { 13'd83 , 3'd6 };
   end
   else if ( input_num == 20'd995 )
   begin
      output_num = { 13'd84 , 3'd3 };
   end
   else if ( input_num == 20'd996 )
   begin
      output_num = { 13'd84 , 3'd7 };
   end
   else if ( input_num == 20'd997 )
   begin
      output_num = { 13'd85 , 3'd5 };
   end
   else if ( input_num == 20'd998 )
   begin
      output_num = { 13'd86 , 3'd4 };
   end
   else if ( input_num == 20'd999 )
   begin
      output_num = { 13'd87 , 3'd4 };
   end
else
   output_num = { 13'd90 , 3'd0 };
end

endmodule
