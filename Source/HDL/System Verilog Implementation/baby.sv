/**********************************************************
 *  This work by Richard Leivers (rjl67@cam.ac.uk)        * 
 *  is licenced under the Creative Commons                *
 *  Attribution-Non-Commercial-Share Alike 2.0 UK:        *
 *  England & Wales License.                              *
 *                                                        *
 *  To view a copy of this licence, visit                 *
 *  http://creativecommons.org/licenses/by-nc-sa/2.0/uk/  *
 *  or send a letter to Creative Commons,                 *
 *  171 Second Street, Suite 300, San Francisco,          *
 *  California 94105, USA.                                *
 *                                                        *
 **********************************************************	
 *	Manchester Baby in System Verilog  	*
 * 	Richard Leivers - July 2009        	*
 * 										*
 *	TOP LEVEL MODULE & TYPEDEFS			*
 ****************************************/

module baby (
	//clocks
	input 			CLOCK_50,
	
	//user input
	input 	[17:0] 	SW,
	input 	[3:0] 	KEY,
	
	//output LEDs
	output 	[8:0] 	LEDG,
	output 	[6:0] 	HEX7,
	output 	[6:0] 	HEX6,
	output 	[6:0] 	HEX5,
	output 	[6:0] 	HEX4,
	output 	[6:0] 	HEX3,
	output 	[6:0] 	HEX2,
	output 	[6:0] 	HEX1,
	output 	[6:0] 	HEX0,
	
	//vga ports
	output 	[9:0] 	VGA_R,
	output 	[9:0] 	VGA_G,
	output 	[9:0] 	VGA_B,
	output 			VGA_CLK,
	output 			VGA_BLANK,
	output 			VGA_HS,
	output 			VGA_VS,
	output 			VGA_SYNC
	);


	// INSTANTIATE 3 PROGRAMS TO BE SWITCHED USING SW 17-15
	// Chosen prog no is assigned to prog which is copied into memory on initialisation
	// progPROGRAM files can be created using the BabySNPtoV tool
	reg signed [31:0] prog [0:31];
	reg signed [31:0] prog1 [0:31];
	reg signed [31:0] prog2 [0:31];
	reg signed [31:0] prog3 [0:31];

	progNIGHT prog1inst (
		.prog(prog1)
		);
	progNOODLE prog2inst (
		.prog(prog2)
		);
	progFIB prog3inst (
		.prog(prog3)
		);

	reg [4:0] row = 0;
	always @ (posedge CLOCK_40) begin
		row <= row + 5'd1;
		
		if (SW[17]) prog[row] <= prog3[row];
		else if (SW[16]) prog[row] <= prog2[row];
		else if (SW[15]) prog[row] <= prog1[row];
		else prog[row] <= prog1[row];
	end
	
	
	// Reset button
	wire reset = ~KEY[0];
	
	// Set HEX7-4 to off
	assign HEX7 = -7'b1;
	assign HEX6 = -7'b1;
	assign HEX5 = -7'b1;
	assign HEX4 = -7'b1;	

	// Create signal wires (to come from babyproc)
	// Output on HEX3-0 accordingly
	wire initsig;
	wire stopsig;
	wire [1:0] stepsig;
	always_comb begin
		// Display 'Init'
		if (initsig) begin
			HEX3[6:0] = 7'b1111001;
			HEX2[6:0] = 7'b0101011;
			HEX1[6:0] = 7'b1111011;
			HEX0[6:0] = 7'b0000111;
		end
		// Display 'StoP'
		else if (stopsig) begin
			HEX3[6:0] = 7'b0010010;
			HEX2[6:0] = 7'b0000111;
			HEX1[6:0] = 7'b0100011;
			HEX0[6:0] = 7'b0001100;
		end	
		// Display 'run' and a step indicator
		else begin
			HEX3[6:0] = 7'b0101111;
			HEX2[6:0] = 7'b1100011;
			HEX1[6:0] = 7'b0101011;
			HEX0[6] = ~stepsig[1] || stepsig[0];
			HEX0[5] = ~stepsig[1] || ~stepsig[0];
			HEX0[4:2] = 3'b111;
			HEX0[1] = stepsig[1] || ~stepsig[0];
			HEX0[0] = stepsig[1] || stepsig[0];
		end		
	end
	
	// Diagnostic LEDs
	assign LEDG[7] = enable;
	assign LEDG[6] = update;
	assign LEDG[3:0] = KEY[3:0];
	
	// CLOCK
	wire CLOCK_40;
	
	// INSTANTIATE VGA MODULES
	assign VGA_CLK = CLOCK_40;
	assign VGA_SYNC = 0;
	wire candraw;
	wire [10:0] x;
	wire [10:0] y;
	wire start;
	

	// 40MHz Clock
	pll40MHz pll2(.inclk0(CLOCK_50), .c0(CLOCK_40));
	
	// Timing parameter module
	params p(
		.clk(CLOCK_40),
		.vsync(VGA_VS),
		.hsync(VGA_HS),
		.x(x),
		.y(y),
		.can_draw(candraw),
		.start_of_frame(start)
		);
	
	// Renderer module
	renderer r(
		.clk(CLOCK_40),
		.candraw(candraw),
		.x(x),
		.y(y),
		.red(VGA_R),
		.green(VGA_G),
		.blue(VGA_B),
		.n_vga_blank(VGA_BLANK),
		.update,
		.updatedata,
		.updateaddr,
		.updateACC,
		.newACC,
		.rCI(rCI),
		.initsig,
		.stopsig,
		.stepsig
		);


	// INSTANTIATE CLOCK DIVIDER
	// Divides clock and handles manual triggering depending on settings
	wire enable;
	clockdiv clockdivider (
		.CLOCK_40(CLOCK_40),
		.select(SW[2:0]),
		.trigger(~KEY[3]),
		.multitrigger(~KEY[2]),
		.enable
		);
	
	
	// Wires to pass updates from processor to VGA modules
	wire update;
	wire [31:0] updatedata;
	wire [4:0] updateaddr;	
	wire updateACC;
	wire signed [31:0] newACC;
	wire [4:0]	rCI;
	
	
	// MAIN PROCESSOR INSTANTIATION
	babyproc procinstance (
		.clk(CLOCK_40),
		.enable,
		.prog,
		.update,
		.updatedata,
		.updateaddr,
		.updateACC,
		.newACC,
		.rCI,
		.initsig,
		.stopsig,
		.stepsig,
		.reset
		);

endmodule
