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
 *	CHARACTER BIT MASKS					*
 ****************************************/
 
module dotnos(
	input [3:0] x,
	input [3:0] y,
	input [3:0] no,
	output pixel
	);
	
	wire [7:0] line = {no[3:0], y[3:0]};
	assign pixel = linepattern[line][15-x];

	reg [15:0] linepattern [0:255];

	always @(*) begin
			linepattern[8'h00] <= 16'b0000000000000000;
			linepattern[8'h01] <= 16'b0000001111100000;
			linepattern[8'h02] <= 16'b0000011111110000;
			linepattern[8'h03] <= 16'b0000111000111000;
			linepattern[8'h04] <= 16'b0000110000011000;
			linepattern[8'h05] <= 16'b0001110000011100;
			linepattern[8'h06] <= 16'b0001100000001100;
			linepattern[8'h07] <= 16'b0001100000001100;
			linepattern[8'h08] <= 16'b0001100000001100;
			linepattern[8'h09] <= 16'b0001100000001100;
			linepattern[8'h0a] <= 16'b0001110000011100;
			linepattern[8'h0b] <= 16'b0000110000011000;
			linepattern[8'h0c] <= 16'b0000111000111000;
			linepattern[8'h0d] <= 16'b0000011111110000;
			linepattern[8'h0e] <= 16'b0000001111100000;
			linepattern[8'h0f] <= 16'b0000000000000000;

			linepattern[8'h10][15:0] <= 16'b0000000000000000;
			linepattern[8'h11][15:0] <= 16'b0000000110000000;
			linepattern[8'h12][15:0] <= 16'b0000001110000000;
			linepattern[8'h13][15:0] <= 16'b0000011110000000;
			linepattern[8'h14][15:0] <= 16'b0000110110000000;
			linepattern[8'h15][15:0] <= 16'b0000000110000000;
			linepattern[8'h16][15:0] <= 16'b0000000110000000;
			linepattern[8'h17][15:0] <= 16'b0000000110000000;
			linepattern[8'h18][15:0] <= 16'b0000000110000000;
			linepattern[8'h19][15:0] <= 16'b0000000110000000;
			linepattern[8'h1a][15:0] <= 16'b0000000110000000;
			linepattern[8'h1b][15:0] <= 16'b0000000110000000;
			linepattern[8'h1c][15:0] <= 16'b0000000110000000;
			linepattern[8'h1d][15:0] <= 16'b0000111111110000;
			linepattern[8'h1e][15:0] <= 16'b0000111111110000;
			linepattern[8'h1f][15:0] <= 16'b0000000000000000;

			linepattern[8'h20][15:0] <= 16'b0000000000000000;
			linepattern[8'h21][15:0] <= 16'b0000001111000000;
			linepattern[8'h22][15:0] <= 16'b0000011111100000;
			linepattern[8'h23][15:0] <= 16'b0000111001110000;
			linepattern[8'h24][15:0] <= 16'b0000110000110000;
			linepattern[8'h25][15:0] <= 16'b0000000000110000;
			linepattern[8'h26][15:0] <= 16'b0000000000110000;
			linepattern[8'h27][15:0] <= 16'b0000000001110000;
			linepattern[8'h28][15:0] <= 16'b0000000011100000;
			linepattern[8'h29][15:0] <= 16'b0000000111000000;
			linepattern[8'h2a][15:0] <= 16'b0000001110000000;
			linepattern[8'h2b][15:0] <= 16'b0000011100000000;
			linepattern[8'h2c][15:0] <= 16'b0000111000000000;
			linepattern[8'h2d][15:0] <= 16'b0000111111110000;
			linepattern[8'h2e][15:0] <= 16'b0000111111110000;
			linepattern[8'h2f][15:0] <= 16'b0000000000000000;

			linepattern[8'h30][15:0] <= 16'b0000000000000000;
			linepattern[8'h31][15:0] <= 16'b0000011111000000;
			linepattern[8'h32][15:0] <= 16'b0000111111100000;
			linepattern[8'h33][15:0] <= 16'b0001110001110000;
			linepattern[8'h34][15:0] <= 16'b0001100000110000;
			linepattern[8'h35][15:0] <= 16'b0000000000110000;
			linepattern[8'h36][15:0] <= 16'b0000000001110000;
			linepattern[8'h37][15:0] <= 16'b0000011111100000;
			linepattern[8'h38][15:0] <= 16'b0000011111100000;
			linepattern[8'h39][15:0] <= 16'b0000000001110000;
			linepattern[8'h3a][15:0] <= 16'b0000000000110000;
			linepattern[8'h3b][15:0] <= 16'b0000000000110000;
			linepattern[8'h3c][15:0] <= 16'b0001110001110000;
			linepattern[8'h3d][15:0] <= 16'b0000111111100000;
			linepattern[8'h3e][15:0] <= 16'b0000011111000000;
			linepattern[8'h3f][15:0] <= 16'b0000000000000000;

			linepattern[8'h40][15:0] <= 16'b0000000000000000;
			linepattern[8'h41][15:0] <= 16'b0000001100000000;
			linepattern[8'h42][15:0] <= 16'b0000001100000000;
			linepattern[8'h43][15:0] <= 16'b0000011100000000;
			linepattern[8'h44][15:0] <= 16'b0000011000000000;
			linepattern[8'h45][15:0] <= 16'b0000111000000000;
			linepattern[8'h46][15:0] <= 16'b0000110000000000;
			linepattern[8'h47][15:0] <= 16'b0001110000000000;
			linepattern[8'h48][15:0] <= 16'b0001100000000000;
			linepattern[8'h49][15:0] <= 16'b0011100011000000;
			linepattern[8'h4a][15:0] <= 16'b0011000011000000;
			linepattern[8'h4b][15:0] <= 16'b0011111111111000;
			linepattern[8'h4c][15:0] <= 16'b0011111111111000;
			linepattern[8'h4d][15:0] <= 16'b0000000011000000;
			linepattern[8'h4e][15:0] <= 16'b0000000011000000;
			linepattern[8'h4f][15:0] <= 16'b0000000000000000;

			linepattern[8'h50][15:0] <= 16'b0000000000000000;
			linepattern[8'h51][15:0] <= 16'b0000111111110000;
			linepattern[8'h52][15:0] <= 16'b0000111111110000;
			linepattern[8'h53][15:0] <= 16'b0000110000000000;
			linepattern[8'h54][15:0] <= 16'b0000110000000000;
			linepattern[8'h55][15:0] <= 16'b0000110000000000;
			linepattern[8'h56][15:0] <= 16'b0000111111000000;
			linepattern[8'h57][15:0] <= 16'b0000111111100000;
			linepattern[8'h58][15:0] <= 16'b0000000001110000;
			linepattern[8'h59][15:0] <= 16'b0000000000110000;
			linepattern[8'h5a][15:0] <= 16'b0000000000110000;
			linepattern[8'h5b][15:0] <= 16'b0000110000110000;
			linepattern[8'h5c][15:0] <= 16'b0000111001110000;
			linepattern[8'h5d][15:0] <= 16'b0000011111100000;
			linepattern[8'h5e][15:0] <= 16'b0000001111000000;
			linepattern[8'h5f][15:0] <= 16'b0000000000000000;

			linepattern[8'h60][15:0] <= 16'b0000000000000000;
			linepattern[8'h61][15:0] <= 16'b0000001111100000;
			linepattern[8'h62][15:0] <= 16'b0000011111110000;
			linepattern[8'h63][15:0] <= 16'b0000111000000000;
			linepattern[8'h64][15:0] <= 16'b0000110000000000;
			linepattern[8'h65][15:0] <= 16'b0000110000000000;
			linepattern[8'h66][15:0] <= 16'b0000110111100000;
			linepattern[8'h67][15:0] <= 16'b0000111111110000;
			linepattern[8'h68][15:0] <= 16'b0000111000111000;
			linepattern[8'h69][15:0] <= 16'b0000110000011000;
			linepattern[8'h6a][15:0] <= 16'b0000110000011000;
			linepattern[8'h6b][15:0] <= 16'b0000110000011000;
			linepattern[8'h6c][15:0] <= 16'b0000111000111000;
			linepattern[8'h6d][15:0] <= 16'b0000011111110000;
			linepattern[8'h6e][15:0] <= 16'b0000000111000000;
			linepattern[8'h6f][15:0] <= 16'b0000000000000000;

			linepattern[8'h70][15:0] <= 16'b0000000000000000;
			linepattern[8'h71][15:0] <= 16'b0001111111111000;
			linepattern[8'h72][15:0] <= 16'b0001111111111000;
			linepattern[8'h73][15:0] <= 16'b0000000000011000;
			linepattern[8'h74][15:0] <= 16'b0000000000111000;
			linepattern[8'h75][15:0] <= 16'b0000000001110000;
			linepattern[8'h76][15:0] <= 16'b0000000011100000;
			linepattern[8'h77][15:0] <= 16'b0000000111000000;
			linepattern[8'h78][15:0] <= 16'b0000001110000000;
			linepattern[8'h79][15:0] <= 16'b0000011100000000;
			linepattern[8'h7a][15:0] <= 16'b0000011000000000;
			linepattern[8'h7b][15:0] <= 16'b0000011000000000;
			linepattern[8'h7c][15:0] <= 16'b0000011000000000;
			linepattern[8'h7d][15:0] <= 16'b0000011000000000;
			linepattern[8'h7e][15:0] <= 16'b0000011000000000;
			linepattern[8'h7f][15:0] <= 16'b0000000000000000;

			linepattern[8'h80][15:0] <= 16'b0000000000000000;
			linepattern[8'h81][15:0] <= 16'b0000011111000000;
			linepattern[8'h82][15:0] <= 16'b0000111111100000;
			linepattern[8'h83][15:0] <= 16'b0001110001110000;
			linepattern[8'h84][15:0] <= 16'b0001100000110000;
			linepattern[8'h85][15:0] <= 16'b0001100000110000;
			linepattern[8'h86][15:0] <= 16'b0001110001110000;
			linepattern[8'h87][15:0] <= 16'b0000111111100000;
			linepattern[8'h88][15:0] <= 16'b0000111111100000;
			linepattern[8'h89][15:0] <= 16'b0001110001110000;
			linepattern[8'h8a][15:0] <= 16'b0001100000110000;
			linepattern[8'h8b][15:0] <= 16'b0001100000110000;
			linepattern[8'h8c][15:0] <= 16'b0001110001110000;
			linepattern[8'h8d][15:0] <= 16'b0000111111100000;
			linepattern[8'h8e][15:0] <= 16'b0000011111000000;
			linepattern[8'h8f][15:0] <= 16'b0000000000000000;

			linepattern[8'h90][15:0] <= 16'b0000000000000000;
			linepattern[8'h91][15:0] <= 16'b0000001111110000;
			linepattern[8'h92][15:0] <= 16'b0000111111111000;
			linepattern[8'h93][15:0] <= 16'b0000110000011000;
			linepattern[8'h94][15:0] <= 16'b0000110000011000;
			linepattern[8'h95][15:0] <= 16'b0000110000011000;
			linepattern[8'h96][15:0] <= 16'b0000110000011000;
			linepattern[8'h97][15:0] <= 16'b0000110000011000;
			linepattern[8'h98][15:0] <= 16'b0000011111111000;
			linepattern[8'h99][15:0] <= 16'b0000001111111000;
			linepattern[8'h9a][15:0] <= 16'b0000000000011000;
			linepattern[8'h9b][15:0] <= 16'b0000000000011000;
			linepattern[8'h9c][15:0] <= 16'b0000000000011000;
			linepattern[8'h9d][15:0] <= 16'b0000000000011000;
			linepattern[8'h9e][15:0] <= 16'b0000000000011000;
			linepattern[8'h9f][15:0] <= 16'b0000000000000000;

			linepattern[8'hA0][15:0] <= 16'b0000000000000000;
			linepattern[8'hA1][15:0] <= 16'b0000000110000000;
			linepattern[8'hA2][15:0] <= 16'b0000000110000000;
			linepattern[8'hA3][15:0] <= 16'b0000001111000000;
			linepattern[8'hA4][15:0] <= 16'b0000001111000000;
			linepattern[8'hA5][15:0] <= 16'b0000011001100000;
			linepattern[8'hA6][15:0] <= 16'b0000011001100000;
			linepattern[8'hA7][15:0] <= 16'b0000010000100000;
			linepattern[8'hA8][15:0] <= 16'b0000110000110000;
			linepattern[8'hA9][15:0] <= 16'b0000111111110000;
			linepattern[8'hAa][15:0] <= 16'b0001111111111000;
			linepattern[8'hAb][15:0] <= 16'b0001100000011000;
			linepattern[8'hAc][15:0] <= 16'b0011100000011100;
			linepattern[8'hAd][15:0] <= 16'b0011000000001100;
			linepattern[8'hAe][15:0] <= 16'b0011000000001100;
			linepattern[8'hAf][15:0] <= 16'b0000000000000000;

			linepattern[8'hB0][15:0] <= 16'b0000000000000000;
			linepattern[8'hB1][15:0] <= 16'b0000111111100000;
			linepattern[8'hB2][15:0] <= 16'b0000111111110000;
			linepattern[8'hB3][15:0] <= 16'b0000110000111000;
			linepattern[8'hB4][15:0] <= 16'b0000110000011000;
			linepattern[8'hB5][15:0] <= 16'b0000110000011000;
			linepattern[8'hB6][15:0] <= 16'b0000110000111000;
			linepattern[8'hB7][15:0] <= 16'b0000111111110000;
			linepattern[8'hB8][15:0] <= 16'b0000111111110000;
			linepattern[8'hB9][15:0] <= 16'b0000110000111000;
			linepattern[8'hBa][15:0] <= 16'b0000110000011000;
			linepattern[8'hBb][15:0] <= 16'b0000110000011000;
			linepattern[8'hBc][15:0] <= 16'b0000110000111000;
			linepattern[8'hBd][15:0] <= 16'b0000111111110000;
			linepattern[8'hBe][15:0] <= 16'b0000111111100000;
			linepattern[8'hBf][15:0] <= 16'b0000000000000000;

			linepattern[8'hC0][15:0] <= 16'b0000000000000000;
			linepattern[8'hC1][15:0] <= 16'b0000011111110000;
			linepattern[8'hC2][15:0] <= 16'b0000111111111000;
			linepattern[8'hC3][15:0] <= 16'b0001110000011000;
			linepattern[8'hC4][15:0] <= 16'b0011100000000000;
			linepattern[8'hC5][15:0] <= 16'b0011000000000000;
			linepattern[8'hC6][15:0] <= 16'b0011000000000000;
			linepattern[8'hC7][15:0] <= 16'b0011000000000000;
			linepattern[8'hC8][15:0] <= 16'b0011000000000000;
			linepattern[8'hC9][15:0] <= 16'b0011000000000000;
			linepattern[8'hCa][15:0] <= 16'b0011000000000000;
			linepattern[8'hCb][15:0] <= 16'b0011100000000000;
			linepattern[8'hCc][15:0] <= 16'b0001110000011000;
			linepattern[8'hCd][15:0] <= 16'b0000111111111000;
			linepattern[8'hCe][15:0] <= 16'b0000011111110000;
			linepattern[8'hCf][15:0] <= 16'b0000000000000000;

			linepattern[8'hD0][15:0] <= 16'b0000000000000000;
			linepattern[8'hD1][15:0] <= 16'b0011111111000000;
			linepattern[8'hD2][15:0] <= 16'b0011111111110000;
			linepattern[8'hD3][15:0] <= 16'b0011000000111000;
			linepattern[8'hD4][15:0] <= 16'b0011000000011100;
			linepattern[8'hD5][15:0] <= 16'b0011000000001100;
			linepattern[8'hD6][15:0] <= 16'b0011000000001100;
			linepattern[8'hD7][15:0] <= 16'b0011000000001100;
			linepattern[8'hD8][15:0] <= 16'b0011000000001100;
			linepattern[8'hD9][15:0] <= 16'b0011000000001100;
			linepattern[8'hDa][15:0] <= 16'b0011000000001100;
			linepattern[8'hDb][15:0] <= 16'b0011000000011000;
			linepattern[8'hDc][15:0] <= 16'b0011000000110000;
			linepattern[8'hDd][15:0] <= 16'b0011111111110000;
			linepattern[8'hDe][15:0] <= 16'b0011111111000000;
			linepattern[8'hDf][15:0] <= 16'b0000000000000000;

			linepattern[8'hE0][15:0] <= 16'b0000000000000000;
			linepattern[8'hE1][15:0] <= 16'b0011111111111000;
			linepattern[8'hE2][15:0] <= 16'b0011111111111000;
			linepattern[8'hE3][15:0] <= 16'b0011000000000000;
			linepattern[8'hE4][15:0] <= 16'b0011000000000000;
			linepattern[8'hE5][15:0] <= 16'b0011000000000000;
			linepattern[8'hE6][15:0] <= 16'b0011000000000000;
			linepattern[8'hE7][15:0] <= 16'b0011111111100000;
			linepattern[8'hE8][15:0] <= 16'b0011111111100000;
			linepattern[8'hE9][15:0] <= 16'b0011000000000000;
			linepattern[8'hEa][15:0] <= 16'b0011000000000000;
			linepattern[8'hEb][15:0] <= 16'b0011000000000000;
			linepattern[8'hEc][15:0] <= 16'b0011000000000000;
			linepattern[8'hEd][15:0] <= 16'b0011111111111000;
			linepattern[8'hEe][15:0] <= 16'b0011111111111000;
			linepattern[8'hEf][15:0] <= 16'b0000000000000000;

			linepattern[8'hF0][15:0] <= 16'b0000000000000000;
			linepattern[8'hF1][15:0] <= 16'b0011111111111000;
			linepattern[8'hF2][15:0] <= 16'b0011111111111000;
			linepattern[8'hF3][15:0] <= 16'b0011000000000000;
			linepattern[8'hF4][15:0] <= 16'b0011000000000000;
			linepattern[8'hF5][15:0] <= 16'b0011000000000000;
			linepattern[8'hF6][15:0] <= 16'b0011000000000000;
			linepattern[8'hF7][15:0] <= 16'b0011111111100000;
			linepattern[8'hF8][15:0] <= 16'b0011111111100000;
			linepattern[8'hF9][15:0] <= 16'b0011000000000000;
			linepattern[8'hFa][15:0] <= 16'b0011000000000000;
			linepattern[8'hFb][15:0] <= 16'b0011000000000000;
			linepattern[8'hFc][15:0] <= 16'b0011000000000000;
			linepattern[8'hFd][15:0] <= 16'b0011000000000000;
			linepattern[8'hFe][15:0] <= 16'b0011000000000000;
			linepattern[8'hFf][15:0] <= 16'b0000000000000000;

	end
endmodule   
