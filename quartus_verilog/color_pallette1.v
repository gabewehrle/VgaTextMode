//Store the colors for color pallete 1 and output the correct RGB value based on fg and bg value
module color_pallette1(output [7:0] R, output [7:0] G, output [7:0] B, input [3:0] fg, input [3:0] bg);

//Different Color Palettes
//https://www.fountainware.com/EXPL/vga_color_palettes.htm

//Standard pallette:
wire [11:0] pallette [15:0]; //Array of RGB values so we can do multiplexing
//                       RGB
assign pallette[0] = 12'h000;
assign pallette[1] = 12'h00F;
assign pallette[2] = 12'h0F0;
assign pallette[3] = 12'h0FF;
assign pallette[4] = 12'hF00;
assign pallette[5] = 12'hF0F;
assign pallette[6] = 12'hFF0;
assign pallette[7] = 12'hFFF;
assign pallette[8] = 12'h555;
assign pallette[9] = 12'h007;
assign pallette[10] = 12'h070;
assign pallette[11] = 12'h077;
assign pallette[12] = 12'h700;
assign pallette[13] = 12'h707;
assign pallette[14] = 12'h770;
assign pallette[15] = 12'h777;

wire [11:0] pallette_fg, pallette_bg;
//Index into the array to make a multiplexer
assign pallette_fg = pallette[fg];
assign pallette_bg = pallette[bg];

assign R = {pallette_fg[11:8], pallette_bg[11:8]};
assign G = {pallette_fg[7:4], pallette_bg[7:4]};
assign B = {pallette_fg[3:0], pallette_bg[3:0]};

endmodule