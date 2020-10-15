//Return a boolean value, OUT, if we're currently in horizontal sync time according to the value of horiz_c
module check_horiz_sync(OUT, horiz_c, CLK);

output reg OUT;
input [9:0] horiz_c;
input CLK;

initial begin
	OUT <= 0;
end

//Bring line low if we're between 0 and 96 (h sync time)
always @(posedge CLK)
begin
	if (horiz_c < 96)
		OUT <= 1;
	else
		OUT <= 0;
end

endmodule