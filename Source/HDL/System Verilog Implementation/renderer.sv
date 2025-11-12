/**************************************** 	
 *	Manchester Baby in System Verilog  	*
 * 	Richard Leivers - July 2009        	*
 * 										*
 *	WILLIAMS TUBE RENDERER				*
 *  Adapted from ECAD Labs				*
 ****************************************/

module renderer(
	input 						clk,
	
	input 						candraw,
	input 				[10:0] 	x,
	input 				[10:0] 	y,
	output 				[9:0] 	red,
	output 				[9:0] 	green,
	output 				[9:0] 	blue,
	output reg 					n_vga_blank,
	
	input 						update,
	input reg 			[31:0] 	updatedata,
	input reg 			[4:0] 	updateaddr,
	input reg 					updateACC,
	input reg signed	[31:0]	newACC,
	input reg		 	[4:0]	rCI,
	
	input 						initsig,
	input 						stopsig,
	input 				[1:0] 	stepsig
	);
	
	// Copy of processor's memory and ACC and CI registers (PI not shown)
	reg signed [31:0] MEM [0:31];
	reg signed [31:0] ACC = 0;
	reg [4:0] CI = 0;
	
	// For current (x,y) pixel, calculate which memory cell this represents
	wire [5:0] cellx = x >> 4;
	wire [5:0] celly = y >> 4;
	
	// Make colour assignment easier by assigning to single 'colour' reg then decomposing
	reg [29:0] colour;
	assign red = colour[29:20];
	assign green = colour[19:10];
	assign blue = colour[9:0];
	
	// Colours to assign to 'colour' reg
	wire [29:0] white = {10'd1023,10'd1023,10'd1023};
	wire [29:0] black = {10'd0,10'd0,10'd0};
	wire [29:0] bgreen = {10'd0,10'd1023,10'd0};
	wire [29:0] dgreen = {10'd0,10'd400,10'd0};
	wire [29:0] bred = {10'd1023,10'd0,10'd0};
	wire [29:0] dred = {10'd400,10'd0,10'd0};
	
	// Instantiate masks and wires for on/off dots
	wire indoton, indotoff;
	doton mydoton (x[3:0], y[3:0], indoton);
	dotoff mydotoff (x[3:0], y[3:0], indotoff);
	
	// Instantiate number masks
	wire indotno1, indotno2, indotno3, indotno4, indotno11, indotno12;
	wire [7:0] rowno = celly - 8'd1;
	wire [3:0] no1 = rowno[7:4];
	wire [3:0] no2 = rowno[3:0];
	wire [31:0] signedline  = (linedata[31]) ? 1+ ~linedata : linedata; //2's complement
	
	reg [4:0] no3b;
	wire [3:0] no3;
	
	assign no3[0] = signedline[no3b];
	assign no3[1] = signedline[no3b+1];
	assign no3[2] = signedline[no3b+2];
	assign no3[3] = signedline[no3b+3];
	dotnos mydotnos1 (x[3:0], y[3:0], no1, indotno1); // row MSB
	dotnos mydotnos2 (x[3:0], y[3:0], no2, indotno2); // row LSB
	dotnos mydotnos3 (x[3:0], y[3:0], no3, indotno3); // mem line value

	// Instantiate masks for ACC numbers
	wire [31:0] signedACC  = (ACC[31]) ? 1+ ~ACC : ACC;
	wire [3:0] no4;
	assign no4[0] = signedACC[no3b];
	assign no4[1] = signedACC[no3b+1];
	assign no4[2] = signedACC[no3b+2];
	assign no4[3] = signedACC[no3b+3];
	dotnos mydotnos4 (x[3:0], y[3:0], no4, indotno4);

	// Instantiate masks for CI numbers
	wire [3:0] no11 = CI[4];
	wire [3:0] no12 = CI[3:0];
	dotnos mydotnos11 (x[3:0], y[3:0], no11, indotno11);
	dotnos mydotnos12 (x[3:0], y[3:0], no12, indotno12);
	
	// Instantiate masks for ACC / CI labels
	wire indotaccl, indotaccr, indotcir, indotcil;
	dotacc mydotacc (x[3:0], y[3:0], indotaccl, indotaccr);
	dotci mydotci (x[3:0], y[3:0], indotcil, indotcir);
	
	// Instantiate minus sign mask
	wire indotminus;
	dotminus mydotminus (x[3:0], y[3:0], indotminus);
	
	// Instantiate masks for INIT/RUN/STOP labels
	wire indotstopl, indotstopr, indotstopc;
	wire indotinitl, indotinitr, indotinitc;
	wire indotrunl, indotrunr, indotrunc;
	dotinit mydotinit (x[3:0], y[3:0], indotinitl, indotinitc, indotinitr);
	dotstop mydotstop (x[3:0], y[3:0], indotstopl, indotstopc, indotstopr);
	dotrun mydotrun (x[3:0], y[3:0], indotrunl, indotrunc, indotrunr);
	
	
	// Line number/data holds number/contents of line to be fetched fro memory	
	// Used to clock access to memory and load on previous cycle
	reg [4:0] lineno = 0;
	reg [31:0] linedata;
	
	// Registers to hold old 'update' values do detect change in two phase signal
	reg oldupdate, oldupdateACC;

	
	
	always @(posedge clk) begin
		// When update signals from processor changes
		// update VGA's memory
		if (update != oldupdate) begin
			MEM[updateaddr] <= updatedata;
			oldupdate <= update;
		end
		if (updateACC != oldupdateACC) begin
			ACC <= newACC;
			oldupdateACC <= updateACC;
		end
		CI <= rCI; // CI updates every cycle

		// At end of scan line, if at bottom of cell, fetch next line of data into linedata
		if (x == 800 & y[3:0] == 15) lineno <= celly;
		linedata <= MEM[lineno];
		
		// Use to calculate number mask for data line values
		if (cellx >= 38 && cellx <= 45) no3b <= 28- ((cellx-38)<<2);
		
		// if in drawable area
		if (candraw) begin
			n_vga_blank <= 1'b1;
			
			// MEMORY DOTS
			if (cellx >= 4 && cellx <= 35 && celly >= 1 && celly <= 32) begin
				if (linedata[cellx-4] && indoton) colour <= bgreen;
				else if (indotoff) colour <= dgreen;
				else  colour <= black;
			end
			else if (cellx >= 4 && cellx <= 8 && celly == 34) begin
				if (CI[cellx-4] && indoton) colour <= bgreen;
				else if (indotoff) colour <= dgreen;
				else colour <= black;
			end
			else if (cellx >= 4 && cellx <= 35 && celly == 36) begin
				if (ACC[cellx-4] && indoton) colour <= bgreen;
				else if (indotoff) colour <= dgreen;
				else colour <= black;
			end
			
			// CI / ACC LABELS
			else if (cellx == 1 && celly == 34 && indotcil) colour <= white;
			else if (cellx == 2 && celly == 34 && indotcir) colour <= white;
			else if (cellx == 1 && celly == 36 && indotaccl) colour <= white;
			else if (cellx == 2 && celly == 36 && indotaccr) colour <= white;
			
			// INIT LABEL AND DOT
			else if (cellx == 14 && celly == 34 && indotinitl) colour <= white;
			else if (cellx == 15 && celly == 34 && indotinitc) colour <= white;
			else if (cellx == 16 && celly == 34 && indotinitr) colour <= white;
			else if (cellx == 18 && celly == 34 && initsig && indoton) colour <= bred;
			else if (cellx == 18 && celly == 34 && ~initsig && indotoff) colour <= dred;
			
			// RUN LABEL AND DOTS
			else if (cellx == 21 && celly == 34 && indotrunl) colour <= white;
			else if (cellx == 22 && celly == 34 && indotrunc) colour <= white;
			else if (cellx == 23 && celly == 34 && indotrunr) colour <= white;
			else if (cellx == 25 && celly == 34) begin
				if (~initsig && ~stopsig && ~stepsig[0] && ~stepsig[1] && indoton) colour <= bred;
				else if (indotoff) colour <= dred;
				else colour <= black;
				end
			else if (cellx == 26 && celly == 34) begin
				if (~initsig && ~stopsig && stepsig[0] && ~stepsig[1] && indoton) colour <= bred;
				else if (indotoff) colour <= dred;
				else colour <= black;
				end
			else if (cellx == 27 && celly == 34) begin
				if (~initsig && ~stopsig && ~stepsig[0] && stepsig[1] && indoton) colour <= bred;
				else if (indotoff) colour <= dred;
				else colour <= black;
				end
			else if (cellx == 28 && celly == 34) begin
				if (~initsig && ~stopsig && stepsig[0] && stepsig[1] && indoton) colour <= bred;
				else if (indotoff) colour <= dred;
				else colour <= black;
				end
			else if (cellx == 25 && celly == 34 && ~(~initsig && ~stopsig) && indotoff) colour <= dred;
			
			// STOP LABEL AND DOT
			else if (cellx == 31 && celly == 34 && indotstopl) colour <= white;
			else if (cellx == 32 && celly == 34 && indotstopc) colour <= white;
			else if (cellx == 33 && celly == 34 && indotstopr) colour <= white;
			else if (cellx == 35 && celly == 34 && stopsig && indoton) colour <= bred;
			else if (cellx == 35 && celly == 34 && ~stopsig && indotoff) colour <= dred;
			
			// ROW LABEL
			else if (cellx == 1 && celly >= 1 && celly <= 32 && indotno1) colour <= white;
			else if (cellx == 2 && celly >= 1 && celly <= 32 && indotno2) colour <= white;
			
			// MINUS SIGN ON DATA ROW
			else if (cellx == 37 && celly >= 1 && celly <= 32 && linedata[31] && indotminus) colour <= white;
			
			// DATA ROW VALUES
			else if (cellx >= 38 && cellx <= 45 && celly >= 1 && celly <= 32 && indotno3) colour <= white;
			
			// ACC ROW VALUES
			else if (cellx >= 38 && cellx <= 45 && celly == 36 && indotno4) colour <= white;
			
			// MINUS SIGN ON ACC ROW
			else if (cellx == 37 && celly == 36 && ACC[31] && indotminus) colour <= white;
			
			// CI VALUES
			else if (cellx == 10 && celly == 34 && indotno11) colour <= white;
			else if (cellx == 11 && celly == 34 && indotno12) colour <= white;
			
			// BACKGROUND
			else colour <= black;
		end else begin
			// if we are not in the visible area, we must set the screen blank
			n_vga_blank <= 1'b0;
		end
	end
endmodule 