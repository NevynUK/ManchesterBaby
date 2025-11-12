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
 *	CLOCK DIVIDER MODULE				*
 ****************************************/

module clockdiv (
	input 				CLOCK_40,
	input 		[2:0] 	select,
	input 				trigger,
	input				multitrigger,
	output reg 			enable
	);

// Sets speed register depending on speed setting
// Counter counts up to this value, so larger value = slower clock
logic [25:0] clkspeed = 0;
always_comb
	case (select)
		3'd0 : clkspeed <= 0; 			// stop
		3'd1 : clkspeed <= 25'd5000000; // 1 per sec
		3'd2 : clkspeed <= 25'd1000000; // 5 per sec
		3'd3 : clkspeed <= 25'd50000; 	// 100 per sec
		3'd4 : clkspeed <= 25'd6000; 	// approx speed of original (1.2ms per instruction)
		3'd5 : clkspeed <= 25'd2500; 	// 2KHz
		3'd6 : clkspeed <= 25'd50; 	// 100 KHz
		3'd7 : clkspeed <= 25'd5; 		// 1MHz
	endcase

// Register for counting up to clkspeed
reg [25:0] clkcount = 0;

// Registers to hold previous button state
// for detecting new button presses vs continuous presses
reg oldtrigger = 0, oldmulti = 0;

// For a multi (four time) trigger this register is used as a counter
// Top 2 bits are used to count, with the rest used as a delay
reg [22:0] multi;



always @ (posedge CLOCK_40) begin
	// If clock is not stopped, count clkcount up to clkspeed and invert clock
	// One output clock cycle is 2 * clkspeed input clock cycles
	// Reset clkcount when clkspeed is reached
	if (clkspeed != 0 && clkcount >= clkspeed) begin
		clkcount <= 0;
		enable <= 1;
	end
	
	// If clock is stopped then go into manual triggering mode
	else if (clkspeed == 0) begin
		// Enable should only go high for one clock pulse at a time so set low if high
		if (enable) enable <= 0;
		// If we still have triggers in a multi trigger to do, then decrease count
		// If delay complete (given by lower bits being all 0) then pulse enable
		else if (multi > 0)  begin
			multi <= multi - 23'd1;
			if (|multi[20:0] == 0) enable <= 1;
		end
		// If trigger button has been pressed set enable high (and multi to 0 for single pulse)
		else if (trigger && ~oldtrigger) begin
			multi <= 23'd0;
			enable <= 1;
		end
		// If multi trigger button pressed reset counter and do first pulse
		else if (multitrigger && ~oldmulti) begin
			multi <= 23'b11111111111111111111111;
			enable <= 1;
		end
		
		// Remember the previous state of the buttons
		// so we don't trigger more than once on each press
		oldtrigger <= trigger;
		oldmulti <= multitrigger;
	end
	
	// If clock is not stopped but clkcount has not reached clkspeed then increase
	else begin
		clkcount <= clkcount + 26'd1;
		enable <= 0;
	end
end

endmodule

