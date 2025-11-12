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
 *	'INIT' DOT BIT MASK					*
 ****************************************/
 
module dotrun(
	input [3:0] x,
	input [3:0] y,
	output pixell,
	output pixelc,
	output pixelr
	);
	
	assign pixell = linepattern[47-x];
	assign pixelc = linepattern[31-x];
	assign pixelr = linepattern[15-x];
	
	reg [47:0] linepattern;

	always @(*)
		case(y)
			5'h00: linepattern <= 48'b000000000000000000000000000000000000000000000000;
			5'h01: linepattern <= 48'b000000111111111110001100000000110011110000001100;
			5'h02: linepattern <= 48'b000000111111111111001100000000110011110000001100;
			5'h03: linepattern <= 48'b000000110000000111001100000000110011111000001100;
			5'h04: linepattern <= 48'b000000110000000011001100000000110011011000001100;
			5'h05: linepattern <= 48'b000000110000000011001100000000110011011100001100;
			5'h06: linepattern <= 48'b000000110000000111001100000000110011001100001100;
			5'h07: linepattern <= 48'b000000111111111111001100000000110011001110001100;
			5'h08: linepattern <= 48'b000000111111111110001100000000110011000110001100;
			5'h09: linepattern <= 48'b000000111110000000001100000000110011000011001100;
			5'h0a: linepattern <= 48'b000000110011100000001100000000110011000011101100;
			5'h0b: linepattern <= 48'b000000110001110000001100000000110011000001101100;
			5'h0c: linepattern <= 48'b000000110000111000001110000001110011000001111100;
			5'h0d: linepattern <= 48'b000000110000011110001111111111110011000000111100;
			5'h0e: linepattern <= 48'b000000110000000111000111111111100011000000111100;
			5'h0f: linepattern <= 48'b000000000000000000000000000000000000000000000000;
	endcase
endmodule   
