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
 *  Manchester Baby in System Verilog            	*
 *  Richard Leivers - July 2009                  	*
 *                                               	*
 *  SNPtoV Converter tool	                    	*
 *  Generates files for loading into SystemVerilog  *
 *  Baby from assembled Baby programs				*
 ****************************************************/

import java.io.*;

public class BabySNPtoV {

	public static void main(String[] args) {
		
		// General catch-all for exceptions
		try{
			// Check valid number of arguments are supplied
			if (args.length != 1) throw new Exception("Usage: java BabySNPtoV source-file");
			
			// Access file given by argument
			// Incorrect file name will throw an exception to be printed
			File in = new File(args[0]);
			FileInputStream fis = new FileInputStream(in);
			DataInputStream dis = new DataInputStream(fis);
		    BufferedReader br = new BufferedReader(new InputStreamReader(dis));
		    
		    // Check file is a .SNP file, and strip extension to get name of program
		    String inName = in.getName();
		    if (!inName.endsWith(".SNP")) throw new Exception("Input file must end with .SNP");
		    inName = inName.substring(0, inName.length()-4);
		    
		    // Create output name, filename and path from input name/path
		    String outName = "prog"+inName;
		    String outPath = (in.getParent() == null) ? outName+".sv" : in.getParent()+File.separator+outName+".sv";
		    System.out.println ("Output will be to " + outPath);
		    
		    // Create output file, problems will throw exceptions
		    FileOutputStream fos = new FileOutputStream(outPath);
			DataOutputStream dos = new DataOutputStream(fos);
		    BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(dos));
		    
		    // Array of strings to hold binary text representation of string
			String[] mem = new String[32];
			
			// Initialise to zero (in case some lines are not specified in input)
			for (int i = 0; i < 32; i++) mem[i] = "00000000000000000000000000000000";
			
			// Loop through file, skipping first line which records no of lines in file
			boolean first = true;
			String line;
		    while ((line = br.readLine()) != null)   {
				
				int lineNo; // no of line in memory
				String lineContent; // to hold textual representation of data
				
				if (!first) {
					// number of line in memory is first 4 characters, in decimal
					lineNo = Integer.parseInt(line.substring(0,4),10);
					
					// binary representation of data is remainder of line (after no and colon)
					lineContent = line.substring(5);
					
					// data should be 32-bits long!
					if (lineContent.length() != 32) throw new Exception("Line "+lineNo+" is not 32 bits long!!");

					// store data in array
					mem[lineNo] = lineContent;
				}
				else {
					first = false;
				}
			}
		
		    // Add nice header to top of file, including program name etc
		    bw.write ("/************************************************"); 	
		    bw.newLine();
		    bw.write ("*  Manchester Baby in System Verilog            *");
		    bw.newLine();
		    bw.write ("*  Richard Leivers - July 2009                  *");
		    bw.newLine();
		    bw.write ("*                                               *");
		    bw.newLine();
		    bw.write ("*  ");
		    bw.write (inName);
		    bw.write (" PROGRAM MEMORY FILE");
		    for (int i = 0; i<25-inName.length(); i++) bw.write(" ");
		    bw.write ("*");
		    bw.newLine();
		    bw.write ("*  Automatically generated from "); 
		    bw.write (inName);
		    bw.write (".SNP");
		    for (int i = 0; i<12-inName.length(); i++) bw.write(" ");
		    bw.write ("*");
		    bw.newLine();
		    bw.write ("*                                               *");
		    bw.newLine();
		    bw.write ("************************************************/");
		    bw.newLine();
		    bw.newLine();
		    
		    // Add verilog 'module' block
		    bw.write ("module "+outName+" (");
		    bw.newLine();
		    bw.write ("    output reg signed [31:0] prog [0:31]");
		    bw.newLine();
		    bw.write ("    );");    
		    bw.newLine();
		    bw.newLine();
		    
		    // For each line of memory add a line of verilog e.g.
		    // reverser r0 (.in(32'b00000110101001000100000100000100),.out(prog[0][31:0]));
		    // This reverses the data from the Baby's LSB on the left notation to the more usual LSB on the right
		    // and stores the result in the prog array ready for outputting
		    for (int j=0; j<32; j++) {
		    	bw.write ("    reverser r"+j+" (.in(32'b"+mem[j]+"),.out(prog["+j+"][31:0]));");
			    bw.newLine();
		    }
		    
		    // Add verilog 'endmodule' block
		    bw.newLine();
		    bw.write ("endmodule");
		    bw.newLine();
		
			// Close files
			bw.close();
			dis.close();
			dos.close();
		}
		
		// Catch and print any expressions
		catch (Exception e){
			System.err.println("Error: " + e);
		}
	}
}
