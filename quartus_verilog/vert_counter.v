//Keep track of the VGA v address on 10-bit line, Q
//Only incremement v when EN is HI
//Rest brings the address back to 0
module vert_counter(Q, EN, CLK, RST);

output reg [9:0] Q; //10 bits needed for up to 524
input EN, CLK, RST;

//Make sure everything's in a known state
initial
begin
	Q <= 524;
end

//On each clock pulse...
always @(posedge EN)
begin
	//Check reset condition
	if (RST)
		Q <= 0;
	//Rollover if we hit max
	else if (Q == 524)
		Q <= 0;
	//Check that we're supposed to increment (TC from horiz_counter)
	else
		Q <= Q + 1;
end

endmodule