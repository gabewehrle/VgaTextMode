//Store the colors for color pallete 3 and output the correct RGB value based on fg and bg value
module color_pallette3(output [7:0] R, output [7:0] G, output [7:0] B, input [3:0] fg, input [3:0] bg);

//Different Color Palettes
//https://www.fountainware.com/EXPL/vga_color_palettes.htm

//Standard pallette:
wire [11:0] pallette [15:0]; //Array of RGB values so we can do multiplexing
//                     RGB
assign pallette[0] = 12'h000;
assign pallette[1] = 12'h00A;
assign pallette[2] = 12'h0A0;
assign pallette[3] = 12'h0AA;
assign pallette[4] = 12'hA00;
assign pallette[5] = 12'hA0A;
assign pallette[6] = 12'hA50;
assign pallette[7] = 12'hAAA;
assign pallette[8] = 12'h555;
assign pallette[9] = 12'h55F;
assign pallette[10] = 12'h5F5;
assign pallette[11] = 12'h5FF;
assign pallette[12] = 12'hF55;
assign pallette[13] = 12'hF5F;
assign pallette[14] = 12'hFF5;
assign pallette[15] = 12'hFFF;

wire [11:0] pallette_fg, pallette_bg;
//Index into the array to make a multiplexer
assign pallette_fg = pallette[fg];
assign pallette_bg = pallette[bg];

assign R = {pallette_fg[11:8], pallette_bg[11:8]};
assign G = {pallette_fg[7:4], pallette_bg[7:4]};
assign B = {pallette_fg[3:0], pallette_bg[3:0]};

endmodule