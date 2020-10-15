//Return a boolean value, OUT, if we're currently in addressable video time according to the value of horiz_c and vert_c
module check_display(OUT, horiz_c, vert_c, CLK);

output reg OUT;
input [9:0] horiz_c, vert_c;
input CLK;

initial begin
	OUT <= 0;
end

//Check that haddr is between 143 and 784, and that v is between 34 and 515 (displayable time)
always @(posedge CLK)
begin
	if ((horiz_c > 143) && (horiz_c < 784) && (vert_c > 34) && (vert_c < 515)) //143, 784, 34, 515
		OUT <= 1;
	else
		OUT <= 0;
end

endmodule