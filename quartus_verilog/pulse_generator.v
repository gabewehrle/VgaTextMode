//Generates a pulse equal in length to one clock cycle when trig is brought hi
module pulse_generator(pulse, trig, CLK);

output reg pulse;
input trig, CLK;

//Make sure pulse is in a known state
initial begin
	pulse = 1'b0;
end

//Only pulses once, and only uses 1 reg
always @(posedge CLK) begin
	pulse <= ~(pulse) & trig;
end

endmodule