module test_params;

    reg clk;
    wire [10:0] x, y;
    wire vga_hsync, vga_vsync, can_draw, start_of_frame;
	
    // Instantiate the params module for testing
    params p(
        .clk(clk),
        .vsync(vga_vsync),
        .hsync(vga_hsync),
        .can_draw(can_draw),
        .start_of_frame(start_of_frame),
        .x(x),
        .y(y)
        );

    initial begin
        // Initialise the clock to zero
        clk = 0;
        // Then at every 10th time unit, invert the clock
        // This generates the clock signal for simulation
        forever #10 clk = !clk;
    end
endmodule 
