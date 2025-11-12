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
 *	'ACC' DOT BIT MASK					*
 ****************************************/
 
module dotacc(
	input [3:0] x,
	input [3:0] y,
	output pixell,
	output pixelr
	);
	
	assign pixell = linepattern[31-x];
	assign pixelr = linepattern[15-x];
	
	reg [31:0] linepattern;

	always @(*)
		case(y)
			5'h00: linepattern <= 32'b00000000000000000000000000000000;
			5'h01: linepattern <= 32'b00000110000000001111110000111111;
			5'h02: linepattern <= 32'b00000110000000011111110001111111;
			5'h03: linepattern <= 32'b00001111000000111000000011100000;
			5'h04: linepattern <= 32'b00001111000001110000000111000000;
			5'h05: linepattern <= 32'b00011001100011100000001110000000;
			5'h06: linepattern <= 32'b00011001100011000000001100000000;
			5'h07: linepattern <= 32'b00010000100011000000001100000000;
			5'h08: linepattern <= 32'b00110000110011000000001100000000;
			5'h09: linepattern <= 32'b00111111110011100000001110000000;
			5'h0a: linepattern <= 32'b01111111111001110000000111000000;
			5'h0b: linepattern <= 32'b01100000011000110000000011000000;
			5'h0c: linepattern <= 32'b11100000011100111000000011100000;
			5'h0d: linepattern <= 32'b11000000001100011111110001111111;
			5'h0e: linepattern <= 32'b11000000001100001111110000111111;
			5'h0f: linepattern <= 32'b00000000000000000000000000000000;
	endcase
endmodule   
