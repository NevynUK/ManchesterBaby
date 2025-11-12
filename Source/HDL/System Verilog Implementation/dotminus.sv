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
 *	'MINUS' DOT BIT MASK				*
 ****************************************/
 
module dotminus (
	input [3:0] x,
	input [3:0] y,
	output pixel
	);

	assign pixel = linepattern[15-x];

	reg [15:0] linepattern;

	always @(*)
		case(y)
			5'h00: linepattern <= 16'b0000000000000000;
			5'h01: linepattern <= 16'b0000000000000000;
			5'h02: linepattern <= 16'b0000000000000000;
			5'h03: linepattern <= 16'b0000000000000000;
			5'h04: linepattern <= 16'b0000000000000000;
			5'h05: linepattern <= 16'b0000000000000000;
			5'h06: linepattern <= 16'b0000000000000000;
			5'h07: linepattern <= 16'b0011111111111100;
			5'h08: linepattern <= 16'b0011111111111100;
			5'h09: linepattern <= 16'b0000000000000000;
			5'h0a: linepattern <= 16'b0000000000000000;
			5'h0b: linepattern <= 16'b0000000000000000;
			5'h0c: linepattern <= 16'b0000000000000000;
			5'h0d: linepattern <= 16'b0000000000000000;
			5'h0e: linepattern <= 16'b0000000000000000;
			5'h0f: linepattern <= 16'b0000000000000000;

	endcase
endmodule   
