/**************************************** 	
 *	Manchester Baby in System Verilog  	*
 * 	Richard Leivers - July 2009        	*
 * 										*
 *	'ON' DOT BIT MASK					*
 ****************************************/
 
module dotnos(
	input [3:0] x,
	input [3:0] y,
	input [3:0] no,
	output pixel
	);
	
	wire [7:0] line = {no[3:0], y[3:0]};
	
	assign pixel = linepattern[15-x];

	reg [15:0] linepattern;

	always @(*)
		case(line)
			5'h00: linepattern <= 16'b0000000000000000;
			5'h01: linepattern <= 16'b0000000010000000;
			5'h02: linepattern <= 16'b0000000110000000;
			5'h03: linepattern <= 16'b0000011110000000;
			5'h04: linepattern <= 16'b0000000110000000;
			5'h05: linepattern <= 16'b0000000110000000;
			5'h06: linepattern <= 16'b0000000110000000;
			5'h07: linepattern <= 16'b0000000110000000;
			5'h08: linepattern <= 16'b0000000110000000;
			5'h09: linepattern <= 16'b0000000110000000;
			5'h0a: linepattern <= 16'b0000000110000000;
			5'h0b: linepattern <= 16'b0000000110000000;
			5'h0c: linepattern <= 16'b0000000110000000;
			5'h0d: linepattern <= 16'b0000000110000000;
			5'h0e: linepattern <= 16'b0000111111110000;
			5'h0f: linepattern <= 16'b0000000000000000;

			5'h10: linepattern <= 16'b0000000000000000;
			5'h11: linepattern <= 16'b0000000010000000;
			5'h12: linepattern <= 16'b0000000110000000;
			5'h13: linepattern <= 16'b0000011110000000;
			5'h14: linepattern <= 16'b0000000110000000;
			5'h15: linepattern <= 16'b0000000110000000;
			5'h16: linepattern <= 16'b0000000110000000;
			5'h17: linepattern <= 16'b0000000110000000;
			5'h18: linepattern <= 16'b0000000110000000;
			5'h19: linepattern <= 16'b0000000110000000;
			5'h1a: linepattern <= 16'b0000000110000000;
			5'h1b: linepattern <= 16'b0000000110000000;
			5'h1c: linepattern <= 16'b0000000110000000;
			5'h1d: linepattern <= 16'b0000000110000000;
			5'h1e: linepattern <= 16'b0000111111110000;
			5'h1f: linepattern <= 16'b0000000000000000;

			5'h20: linepattern <= 16'b0000000000000000;
			5'h21: linepattern <= 16'b0000000010000000;
			5'h22: linepattern <= 16'b0000000110000000;
			5'h23: linepattern <= 16'b0000011110000000;
			5'h24: linepattern <= 16'b0000000110000000;
			5'h25: linepattern <= 16'b0000000110000000;
			5'h26: linepattern <= 16'b0000000110000000;
			5'h27: linepattern <= 16'b0000000110000000;
			5'h28: linepattern <= 16'b0000000110000000;
			5'h29: linepattern <= 16'b0000000110000000;
			5'h2a: linepattern <= 16'b0000000110000000;
			5'h2b: linepattern <= 16'b0000000110000000;
			5'h2c: linepattern <= 16'b0000000110000000;
			5'h2d: linepattern <= 16'b0000000110000000;
			5'h2e: linepattern <= 16'b0000111111110000;
			5'h2f: linepattern <= 16'b0000000000000000;

			5'h30: linepattern <= 16'b0000000000000000;
			5'h31: linepattern <= 16'b0000000010000000;
			5'h32: linepattern <= 16'b0000000110000000;
			5'h33: linepattern <= 16'b0000011110000000;
			5'h34: linepattern <= 16'b0000000110000000;
			5'h35: linepattern <= 16'b0000000110000000;
			5'h36: linepattern <= 16'b0000000110000000;
			5'h37: linepattern <= 16'b0000000110000000;
			5'h38: linepattern <= 16'b0000000110000000;
			5'h39: linepattern <= 16'b0000000110000000;
			5'h3a: linepattern <= 16'b0000000110000000;
			5'h3b: linepattern <= 16'b0000000110000000;
			5'h3c: linepattern <= 16'b0000000110000000;
			5'h3d: linepattern <= 16'b0000000110000000;
			5'h3e: linepattern <= 16'b0000111111110000;
			5'h3f: linepattern <= 16'b0000000000000000;

			5'h40: linepattern <= 16'b0000000000000000;
			5'h41: linepattern <= 16'b0000000010000000;
			5'h42: linepattern <= 16'b0000000110000000;
			5'h43: linepattern <= 16'b0000011110000000;
			5'h44: linepattern <= 16'b0000000110000000;
			5'h45: linepattern <= 16'b0000000110000000;
			5'h46: linepattern <= 16'b0000000110000000;
			5'h47: linepattern <= 16'b0000000110000000;
			5'h48: linepattern <= 16'b0000000110000000;
			5'h49: linepattern <= 16'b0000000110000000;
			5'h4a: linepattern <= 16'b0000000110000000;
			5'h4b: linepattern <= 16'b0000000110000000;
			5'h4c: linepattern <= 16'b0000000110000000;
			5'h4d: linepattern <= 16'b0000000110000000;
			5'h4e: linepattern <= 16'b0000111111110000;
			5'h4f: linepattern <= 16'b0000000000000000;

			5'h50: linepattern <= 16'b0000000000000000;
			5'h51: linepattern <= 16'b0000000010000000;
			5'h52: linepattern <= 16'b0000000110000000;
			5'h53: linepattern <= 16'b0000011110000000;
			5'h54: linepattern <= 16'b0000000110000000;
			5'h55: linepattern <= 16'b0000000110000000;
			5'h56: linepattern <= 16'b0000000110000000;
			5'h57: linepattern <= 16'b0000000110000000;
			5'h58: linepattern <= 16'b0000000110000000;
			5'h59: linepattern <= 16'b0000000110000000;
			5'h5a: linepattern <= 16'b0000000110000000;
			5'h5b: linepattern <= 16'b0000000110000000;
			5'h5c: linepattern <= 16'b0000000110000000;
			5'h5d: linepattern <= 16'b0000000110000000;
			5'h5e: linepattern <= 16'b0000111111110000;
			5'h5f: linepattern <= 16'b0000000000000000;

			5'h60: linepattern <= 16'b0000000000000000;
			5'h61: linepattern <= 16'b0000000010000000;
			5'h62: linepattern <= 16'b0000000110000000;
			5'h63: linepattern <= 16'b0000011110000000;
			5'h64: linepattern <= 16'b0000000110000000;
			5'h65: linepattern <= 16'b0000000110000000;
			5'h66: linepattern <= 16'b0000000110000000;
			5'h67: linepattern <= 16'b0000000110000000;
			5'h68: linepattern <= 16'b0000000110000000;
			5'h69: linepattern <= 16'b0000000110000000;
			5'h6a: linepattern <= 16'b0000000110000000;
			5'h6b: linepattern <= 16'b0000000110000000;
			5'h6c: linepattern <= 16'b0000000110000000;
			5'h6d: linepattern <= 16'b0000000110000000;
			5'h6e: linepattern <= 16'b0000111111110000;
			5'h6f: linepattern <= 16'b0000000000000000;

			5'h70: linepattern <= 16'b0000000000000000;
			5'h71: linepattern <= 16'b0000000010000000;
			5'h72: linepattern <= 16'b0000000110000000;
			5'h73: linepattern <= 16'b0000011110000000;
			5'h74: linepattern <= 16'b0000000110000000;
			5'h75: linepattern <= 16'b0000000110000000;
			5'h76: linepattern <= 16'b0000000110000000;
			5'h77: linepattern <= 16'b0000000110000000;
			5'h78: linepattern <= 16'b0000000110000000;
			5'h79: linepattern <= 16'b0000000110000000;
			5'h7a: linepattern <= 16'b0000000110000000;
			5'h7b: linepattern <= 16'b0000000110000000;
			5'h7c: linepattern <= 16'b0000000110000000;
			5'h7d: linepattern <= 16'b0000000110000000;
			5'h7e: linepattern <= 16'b0000111111110000;
			5'h7f: linepattern <= 16'b0000000000000000;

			5'h80: linepattern <= 16'b0000000000000000;
			5'h81: linepattern <= 16'b0000000010000000;
			5'h82: linepattern <= 16'b0000000110000000;
			5'h83: linepattern <= 16'b0000011110000000;
			5'h84: linepattern <= 16'b0000000110000000;
			5'h85: linepattern <= 16'b0000000110000000;
			5'h86: linepattern <= 16'b0000000110000000;
			5'h87: linepattern <= 16'b0000000110000000;
			5'h88: linepattern <= 16'b0000000110000000;
			5'h89: linepattern <= 16'b0000000110000000;
			5'h8a: linepattern <= 16'b0000000110000000;
			5'h8b: linepattern <= 16'b0000000110000000;
			5'h8c: linepattern <= 16'b0000000110000000;
			5'h8d: linepattern <= 16'b0000000110000000;
			5'h8e: linepattern <= 16'b0000111111110000;
			5'h8f: linepattern <= 16'b0000000000000000;

			5'h90: linepattern <= 16'b0000000000000000;
			5'h91: linepattern <= 16'b0000000010000000;
			5'h92: linepattern <= 16'b0000000110000000;
			5'h93: linepattern <= 16'b0000011110000000;
			5'h94: linepattern <= 16'b0000000110000000;
			5'h95: linepattern <= 16'b0000000110000000;
			5'h96: linepattern <= 16'b0000000110000000;
			5'h97: linepattern <= 16'b0000000110000000;
			5'h98: linepattern <= 16'b0000000110000000;
			5'h99: linepattern <= 16'b0000000110000000;
			5'h9a: linepattern <= 16'b0000000110000000;
			5'h9b: linepattern <= 16'b0000000110000000;
			5'h9c: linepattern <= 16'b0000000110000000;
			5'h9d: linepattern <= 16'b0000000110000000;
			5'h9e: linepattern <= 16'b0000111111110000;
			5'h9f: linepattern <= 16'b0000000000000000;

			5'hA0: linepattern <= 16'b0000000000000000;
			5'hA1: linepattern <= 16'b0000000010000000;
			5'hA2: linepattern <= 16'b0000000110000000;
			5'hA3: linepattern <= 16'b0000011110000000;
			5'hA4: linepattern <= 16'b0000000110000000;
			5'hA5: linepattern <= 16'b0000000110000000;
			5'hA6: linepattern <= 16'b0000000110000000;
			5'hA7: linepattern <= 16'b0000000110000000;
			5'hA8: linepattern <= 16'b0000000110000000;
			5'hA9: linepattern <= 16'b0000000110000000;
			5'hAa: linepattern <= 16'b0000000110000000;
			5'hAb: linepattern <= 16'b0000000110000000;
			5'hAc: linepattern <= 16'b0000000110000000;
			5'hAd: linepattern <= 16'b0000000110000000;
			5'hAe: linepattern <= 16'b0000111111110000;
			5'hAf: linepattern <= 16'b0000000000000000;

			5'hB0: linepattern <= 16'b0000000000000000;
			5'hB1: linepattern <= 16'b0000000010000000;
			5'hB2: linepattern <= 16'b0000000110000000;
			5'hB3: linepattern <= 16'b0000011110000000;
			5'hB4: linepattern <= 16'b0000000110000000;
			5'hB5: linepattern <= 16'b0000000110000000;
			5'hB6: linepattern <= 16'b0000000110000000;
			5'hB7: linepattern <= 16'b0000000110000000;
			5'hB8: linepattern <= 16'b0000000110000000;
			5'hB9: linepattern <= 16'b0000000110000000;
			5'hBa: linepattern <= 16'b0000000110000000;
			5'hBb: linepattern <= 16'b0000000110000000;
			5'hBc: linepattern <= 16'b0000000110000000;
			5'hBd: linepattern <= 16'b0000000110000000;
			5'hBe: linepattern <= 16'b0000111111110000;
			5'hBf: linepattern <= 16'b0000000000000000;

			5'hC0: linepattern <= 16'b0000000000000000;
			5'hC1: linepattern <= 16'b0000000010000000;
			5'hC2: linepattern <= 16'b0000000110000000;
			5'hC3: linepattern <= 16'b0000011110000000;
			5'hC4: linepattern <= 16'b0000000110000000;
			5'hC5: linepattern <= 16'b0000000110000000;
			5'hC6: linepattern <= 16'b0000000110000000;
			5'hC7: linepattern <= 16'b0000000110000000;
			5'hC8: linepattern <= 16'b0000000110000000;
			5'hC9: linepattern <= 16'b0000000110000000;
			5'hCa: linepattern <= 16'b0000000110000000;
			5'hCb: linepattern <= 16'b0000000110000000;
			5'hCc: linepattern <= 16'b0000000110000000;
			5'hCd: linepattern <= 16'b0000000110000000;
			5'hCe: linepattern <= 16'b0000111111110000;
			5'hCf: linepattern <= 16'b0000000000000000;

			5'hD0: linepattern <= 16'b0000000000000000;
			5'hD1: linepattern <= 16'b0000000010000000;
			5'hD2: linepattern <= 16'b0000000110000000;
			5'hD3: linepattern <= 16'b0000011110000000;
			5'hD4: linepattern <= 16'b0000000110000000;
			5'hD5: linepattern <= 16'b0000000110000000;
			5'hD6: linepattern <= 16'b0000000110000000;
			5'hD7: linepattern <= 16'b0000000110000000;
			5'hD8: linepattern <= 16'b0000000110000000;
			5'hD9: linepattern <= 16'b0000000110000000;
			5'hDa: linepattern <= 16'b0000000110000000;
			5'hDb: linepattern <= 16'b0000000110000000;
			5'hDc: linepattern <= 16'b0000000110000000;
			5'hDd: linepattern <= 16'b0000000110000000;
			5'hDe: linepattern <= 16'b0000111111110000;
			5'hDf: linepattern <= 16'b0000000000000000;

			5'hE0: linepattern <= 16'b0000000000000000;
			5'hE1: linepattern <= 16'b0000000010000000;
			5'hE2: linepattern <= 16'b0000000110000000;
			5'hE3: linepattern <= 16'b0000011110000000;
			5'hE4: linepattern <= 16'b0000000110000000;
			5'hE5: linepattern <= 16'b0000000110000000;
			5'hE6: linepattern <= 16'b0000000110000000;
			5'hE7: linepattern <= 16'b0000000110000000;
			5'hE8: linepattern <= 16'b0000000110000000;
			5'hE9: linepattern <= 16'b0000000110000000;
			5'hEa: linepattern <= 16'b0000000110000000;
			5'hEb: linepattern <= 16'b0000000110000000;
			5'hEc: linepattern <= 16'b0000000110000000;
			5'hEd: linepattern <= 16'b0000000110000000;
			5'hEe: linepattern <= 16'b0000111111110000;
			5'hEf: linepattern <= 16'b0000000000000000;

			5'hF0: linepattern <= 16'b0000000000000000;
			5'hF1: linepattern <= 16'b0000000010000000;
			5'hF2: linepattern <= 16'b0000000110000000;
			5'hF3: linepattern <= 16'b0000011110000000;
			5'hF4: linepattern <= 16'b0000000110000000;
			5'hF5: linepattern <= 16'b0000000110000000;
			5'hF6: linepattern <= 16'b0000000110000000;
			5'hF7: linepattern <= 16'b0000000110000000;
			5'hF8: linepattern <= 16'b0000000110000000;
			5'hF9: linepattern <= 16'b0000000110000000;
			5'hFa: linepattern <= 16'b0000000110000000;
			5'hFb: linepattern <= 16'b0000000110000000;
			5'hFc: linepattern <= 16'b0000000110000000;
			5'hFd: linepattern <= 16'b0000000110000000;
			5'hFe: linepattern <= 16'b0000111111110000;
			5'hFf: linepattern <= 16'b0000000000000000;
	endcase
endmodule   
