`timescale 1ns/1ns

module vga_ram_tb();
	reg clk;
	reg vgaclk;
	
	DE10_LITE uut(
		.MAX10_CLK1_50(clk),
		.clk_vga(vgaclk)
	);
	
	initial	begin
		//Set clk value
		clk = 0;
		
		forever	begin
			#20; //50 Mhz
			clk = ~clk;
		end
	end
	
	initial	begin
		//Set clk value
		vgaclk = 0;
		
		forever	begin
			#40; //50 Mhz
			vgaclk = ~vgaclk;
		end
	end
endmodule