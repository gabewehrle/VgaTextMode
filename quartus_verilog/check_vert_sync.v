//Return a boolean value, OUT, if we're currently in vertical sync time according to the value of vert_c
module check_vert_sync(OUT, vert_c, CLK);

output reg OUT;
input [9:0] vert_c;
input CLK;

initial begin
	OUT <= 0;
end

//Bring line low if we're between 0 and 2 (v sync time)
always @(posedge CLK)
begin
	if (vert_c < 2)
		OUT <= 1;
	else
		OUT <= 0;
end

endmodule