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
 *	BABY PROCESSOR MODULE				*
 ****************************************/

// Instruction type
typedef struct packed {
	bit [15:0] notused;
	bit [2:0] fun;
	bit [7:0] crtno;
	bit [4:0] line;
	} instruction;

// Functions
typedef enum {
	JMP, JRP, LDN, STO, SUB, SUB5, TEST, STOP
	} ifunc;
	

module babyproc (
	input 					clk,
	input					enable,
	input 					reset,
	
	// Initial program to load
	input reg signed [31:0] prog [0:31],
	
	// For notifying VGA renderer of updates to memory
	output	reg 			update,
	output 	reg		[31:0] 	updatedata,
	output 	reg		[4:0] 	updateaddr,
	output	reg 			updateACC,
	output  reg signed [31:0] newACC,
	output 	reg		[4:0]	rCI,
	
	// State signals for outputting info on HEX displays etc
	output 					initsig,
	output 					stopsig,
	output 			[1:0] 	stepsig
	
	);

	// PROCESSOR REGISTERS (and intermediate stores, used in stage 3/4)
	reg signed [31:0] ACC = 0; // Accumulator
	reg [31:0] CI = 0, newCI; // Line 1 of Control Register, holds program count
	instruction PI = 0; // Line 2 of Control Register, holds currently executing instruction
	
	// Update flags for CI/ACC/MEM, used in stage 4
	reg uCI = 0, uACC = 0, uMEM = 0; 
	
	// Output program counter to VGA display
	assign rCI = CI[4:0];
	
	// MEMORY - 32x32
	reg signed [31:0] MEM [0:31];
	
	// Registers to hold processor state
	reg [1:0] step = 0; // start on step 1 of 4-step cycle
	reg notstop = 1;    // initially going (not stopped)
	reg initialise = 1; // initialise (copy prog to mem) first
	
	// State signals for outputting info on HEX displays etc
	assign initsig = initialise;
	assign stopsig = ~notstop;
	assign stepsig = step;
	
	always @ (posedge clk, posedge reset) begin
		// If reset then reset all registers to start values
		if (reset) begin
			CI <= 0;
			ACC <= 0;
			uCI <= 0;
			uACC <= 0;
			uMEM <= 0;
			initialise <= 1;
			notstop <= 1;
			step <= 0;
		end
		// INITIALISATION - Copies prog into MEM and sends updates to VGA display
		// This mimics the regular 4-stage cycle (below)
		else if (enable) begin
		if (initialise) case(step)
			// STAGE 1
			2'd0 : 	begin
						// CI used to keep count of row currently being copied
						// Does 1 through 31 then ends with row 0
						CI <= CI + 32'd1;
						step <= 1;
					end
			// STAGE 2
			2'd1 :	begin
						step <= 2;
					end
			// STAGE 3
			2'd2 :	begin
						// Load prog as a regular memory update (CI is row no.)
						updateaddr <= CI[4:0];
						updatedata <= prog[CI[4:0]];
						step <= 3;
					end
			// STAGE 4
			2'd3 :	begin
						// Update MEM and pulse 'update' to transfer to VGA memory
						MEM[updateaddr] <= updatedata;
						update <= ~update;
						step <= 0;		
						
						// Row 0 is the last to do, so when done exit initialisation stage
						if (CI[4:0] == 0) initialise <= 0;
					end		
		endcase
		
		// NORMAL PROCESSOR EXECUTION
		// Until notstop signal goes low		
		else if (notstop) case(step)
		
			// STAGE 1 :: INCREMENT CI
			2'd0 : 	begin
						// CI is incremented
						// Set 'update' to 0 incase it was pulsed high in previous stage 4
						CI <= CI + 32'd1;	
						step <= 1;
					end
					
			// STAGE 2 :: LOAD INSTRUCTION INTO PI AND DECODE		
			2'd1 :	begin
						// Load from CIth place in memory (first 5 bits used)
						// Decode is done by assigning to an Instruction struct
						PI <= MEM[CI[4:0]];
						step <= 2;
					end
					
			
			// STAGE 3 :: EXECUTE INSTRUCTION
			2'd2 :	begin
						// Execution depends on instruction's function
						// Function types given by ifunc enum
						// Extra fake registers used to store intermediate values
						// Flags to show what has been updated
						case (PI.fun)
							// JMP sets CR to the contents of LINE
							JMP : 	begin
										newCI <= MEM[PI.line];
										uCI <= 1;
									end
							// JRP adds the contents of LINE to CR
							JRP:	begin
										newCI <= CI + MEM[PI.line];
										uCI <= 1;
									end
							// LDN gets the contents of LINE, negated, into ACC
							LDN:	begin
										newACC <= -MEM[PI.line];
										uACC <= 1;
									end
							// STO stores the contents of ACC into LINE
							STO:	begin
										updateaddr <= PI.line;
										updatedata <= ACC;
										uMEM <= 1;
									end
							// SUB subtracts the contents of LINE from ACC putting the result in ACC
							// (Both 001 and 101 are codes for SUB)
							SUB, SUB5:	begin
										newACC <= ACC - MEM[PI.line];
										uACC <= 1;
									end
							// TEST if the contents of ACC are less than zero, add 1 to CR
							TEST:	begin
										if (ACC < 0) begin
											newCI <= CI + 32'd1;
											uCI <= 1;
										end
									end	
							// STOP halts the machine
							// Setting notstop low prevents this part from running
							STOP:	notstop <= 0;
									
						endcase
						
						step <= 3;
					end
					
					
			// STAGE 4 :: WRITEBACK - UPDATE MEM AND REGISTERS		
			2'd3 :	begin
						// If memory update flag set, update mem and pulse 'update' to notify vga
						if(uMEM) begin
							MEM[updateaddr] <= updatedata;
							update <= ~update;
							uMEM <= 0;
						end
						
						// If ACC / CI update flags set, update them
						if(uACC) begin
							ACC <= newACC;
							updateACC <= ~updateACC;
							uACC <= 0;
						end
						if(uCI) begin
							CI <= newCI;
							uCI <= 0;
						end
						
						step <= 0;
					end		
		endcase
		end
	end
	
endmodule
