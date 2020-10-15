//Switch between each pallette based on the state of select
module pallette_controller(output [7:0] R, output [7:0] G, output [7:0] B, input [3:0] fg, input [3:0] bg, input [1:0] select);
	//Arrays for multiplexing between
	wire [7:0] RED [3:0];
	wire [7:0] GREEN [3:0];
	wire [7:0] BLUE [3:0];
	
	color_pallette1(RED[0], GREEN[0], BLUE[0], fg, bg);
	color_pallette2(RED[1], GREEN[1], BLUE[1], fg, bg);
	color_pallette3(RED[2], GREEN[2], BLUE[2], fg, bg);
	color_pallette4(RED[3], GREEN[3], BLUE[3], fg, bg);
	
	assign R = RED[select];
	assign G = GREEN[select];
	assign B = BLUE[select];
endmodule