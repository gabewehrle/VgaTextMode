//Keep track of the VGA h address, outputting it as a 10-bit number, Q
//TC is brought HI when the vertical address should incremement by 1
//RST brings the address line back to 0
module horiz_counter(Q, TC, CLK, RST);

output reg [9:0] Q; //10 bits needed for up to 800
output reg TC;
input CLK, RST;

//Make sure everything's in a known state before we start
initial
begin
	Q <= 799;
	TC <= 0;
end

//On each clock pulse...
always @(posedge CLK)
begin
	//Check reset condition
	if (RST)
	begin
		Q <= 0;
		TC <= 0;
	end
	//Rollover haddr if we've reached max
	else if (Q == 799)
	begin
		Q <= 0;
		TC <= 1;
	end
	//Incremement haddr
	else
	begin
		Q <= Q + 1;
	end
	//Signal that vcounter should incremement by 1
	if (Q < 798)
	begin
		TC <= 0;
	end
end

endmodule