//Keep track of the current char we're drawing inside the frame buffer
//Only incremement glyph_row, cur_char, and v_en when the right lines are high
//Reset brings the address back to 0
module char_row_counter(glyph_row, char_row, cur_char, v_en, valid, vert_c, CLK, RST);

output reg [11:0] cur_char; //Current character from 0 to 4800
output reg [3:0] glyph_row; //From row 0 to 15 of each glyph
output reg [2:0] char_row; //From 0 to 7 across each char row
input v_en, valid, CLK, RST;

input [9:0] vert_c;

reg [4:0] cur_row; //Current row 0 - 29

//Make sure everything's in a known state
initial
begin
	glyph_row = 0;
	char_row = 0;
	cur_char = 0;
	cur_row = 0;
end

always @(posedge CLK)
begin
	//Check reset condition
	if (RST) begin
		glyph_row <= 0;
		char_row <= 0;
		cur_char <= 0;
		cur_row <= 0;
	end
	else begin
		if (cur_char == 2400)
			cur_char <= 0;
		else if (v_en)
			cur_char <= (cur_row == 0) ? 0 : //Zeroth row
							(cur_row == 1) ? 80: //First row - set to 2 * 80 (80 col per row, 2 bytes per char in frame)
							(cur_row == 2) ? 160:
							(cur_row == 3) ? 240:
							(cur_row == 4) ? 320:
							(cur_row == 5) ? 400:
							(cur_row == 6) ? 480:
							(cur_row == 7) ? 560:
							(cur_row == 8) ? 640:
							(cur_row == 9) ? 720:
							(cur_row == 10) ? 800:
							(cur_row == 11) ? 880:
							(cur_row == 12) ? 960:
							(cur_row == 13) ? 1040:
							(cur_row == 14) ? 1120:
							(cur_row == 15) ? 1200:
							(cur_row == 16) ? 1280:
							(cur_row == 17) ? 1360:
							(cur_row == 18) ? 1440:
							(cur_row == 19) ? 1520:
							(cur_row == 20) ? 1600:
							(cur_row == 21) ? 1680:
							(cur_row == 22) ? 1760:
							(cur_row == 23) ? 1840:
							(cur_row == 24) ? 1920:
							(cur_row == 25) ? 2000:
							(cur_row == 26) ? 2080:
							(cur_row == 27) ? 2160:
							(cur_row == 28) ? 2240:
							2320; //29
		else if (char_row == 7) begin
			//No need to check max condition since this will just rollover
			cur_char <= cur_char + 1;
		end
		if ((vert_c > 35) && (vert_c < 516) && v_en) begin
			//Only increment glyph row when we go to the next row and we're in the valid region vertically
			glyph_row <= glyph_row + 1;
			if (glyph_row == 15) begin
				//Current row (in terms of how many chars) only increments every 16 ACTUAL rows since each char is 16 pixels high
				if (cur_row == 29)
					cur_row <= 0;
				else
					cur_row <= cur_row + 1;
			end
		end
		if (valid)
			//No need to check max condition since this will just rollover
			char_row <= char_row + 1;
	end
end

endmodule