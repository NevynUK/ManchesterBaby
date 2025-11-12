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
 *	'STOP' DOT BIT MASK					*
 ****************************************/
 
module dotstop(
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
			5'h01: linepattern <= 48'b001111111111001111111111000111111110001111111110;
			5'h02: linepattern <= 48'b011111111111001111111111001111111111001111111111;
			5'h03: linepattern <= 48'b011100000000000000110000001110000111001100000111;
			5'h04: linepattern <= 48'b011000000000000000110000001100000011001100000011;
			5'h05: linepattern <= 48'b011000000000000000110000001100000011001100000011;
			5'h06: linepattern <= 48'b011100000000000000110000001100000011001100000111;
			5'h07: linepattern <= 48'b011111111110000000110000001100000011001111111111;
			5'h08: linepattern <= 48'b001111111111000000110000001100000011001111111110;
			5'h09: linepattern <= 48'b000000000111000000110000001100000011001100000000;
			5'h0a: linepattern <= 48'b000000000011000000110000001100000011001100000000;
			5'h0b: linepattern <= 48'b000000000011000000110000001100000011001100000000;
			5'h0c: linepattern <= 48'b000000000111000000110000001110000111001100000000;
			5'h0d: linepattern <= 48'b011111111111000000110000001111111111001100000000;
			5'h0e: linepattern <= 48'b011111111110000000110000000111111110001100000000;
			5'h0f: linepattern <= 48'b000000000000000000000000000000000000000000000000;
	endcase
endmodule   
