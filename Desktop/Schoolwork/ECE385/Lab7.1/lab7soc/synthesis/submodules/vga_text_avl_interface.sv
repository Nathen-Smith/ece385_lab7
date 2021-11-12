/************************************************************************
Avalon-MM Interface VGA Text mode display

Register Map:
0x000-0x0257 : VRAM, 80x30 (2400 byte, 600 word) raster order (first column then row)
0x258        : control register

VRAM Format:
X->
[ 31  30-24][ 23  22-16][ 15  14-8 ][ 7    6-0 ]
[IV3][CODE3][IV2][CODE2][IV1][CODE1][IV0][CODE0]

IVn = Draw inverse glyph
CODEn = Glyph code from IBM codepage 437

Control Register Format:
[[31-25][24-21][20-17][16-13][ 12-9][ 8-5 ][ 4-1 ][   0    ] 
[[RSVD ][FGD_R][FGD_G][FGD_B][BKG_R][BKG_G][BKG_B][RESERVED]

VSYNC signal = bit which flips on every Vsync (time for new frame), used to synchronize software
BKG_R/G/B = Background color, flipped with foreground when IVn bit is set
FGD_R/G/B = Foreground color, flipped with background when Inv bit is set

************************************************************************/
`define NUM_REGS 601 //80*30 characters / 4 characters per register
`define CTRL_REG 600 //index of control register

module vga_text_avl_interface (
	// Avalon Clock Input, note this clock is also used for VGA, so this must be 50Mhz
	// We can put a clock divider here in the future to make this IP more generalizable
	input logic Clk,
	
	// Avalon Reset Input
	input logic Reset,
	
	// Avalon-MM Slave Signals
	input  logic AVL_READ,					// Avalon-MM Read
	input  logic AVL_WRITE,					// Avalon-MM Write
	input  logic AVL_CS,					// Avalon-MM Chip Select
	input  logic [3:0] AVL_BYTE_EN,			// Avalon-MM Byte Enable
	input  logic [11:0] AVL_ADDR,			// Avalon-MM Address
	input  logic [31:0] AVL_WRITEDATA,		// Avalon-MM Write Data
	output logic [31:0] AVL_READDATA,		// Avalon-MM Read Data
	
	// Exported Conduit (mapped to VGA port - make sure you export in Platform Designer)
	output logic [3:0]  red, green, blue,	// VGA color channels (mapped to output pins in top-level)
	output logic hs, vs						// VGA HS/VS
);



/*
logic [31:0] LOCAL_REG       [`NUM_REGS]; // Registers
*/

logic [31:0] PALETTE_REG [8];



always_ff @(posedge Clk)
begin
	if(AVL_WRITE & AVL_ADDR[11])
		PALETTE_REG[AVL_ADDR[2:0]] <= AVL_WRITEDATA;

	
	else if(AVL_READ & AVL_ADDR[11])
		AVL_READDATA <= PALETTE_REG[AVL_ADDR[2:0]];
		
end


//put other local variables here

logic pixel_clk, blank, sync, bitdraw;
logic [9:0] DrawX, DrawY;

logic [10:0] rom_addr;
logic [7:0] font_rom_data;

//Declare submodules..e.g. VGA controller, ROMS, etc

font_rom r1(.addr(rom_addr), .data(font_rom_data));

vga_controller vga_C(.Clk(Clk), .Reset(Reset), .pixel_clk(pixel_clk), .hs(hs), .vs(vs), .blank(blank), .sync(sync), .DrawX(DrawX), .DrawY(DrawY));

//ram on_chip(
//	.address(AVL_ADDR),
//	.byteena(AVL_BYTE_EN), 
//	.clock(Clk), 
//	.data(AVL_WRITEDATA), 
//	.rden(AVL_READ), 
//	.wren(AVL_WRITE), 
//	.q(AVL_READDATA)
//	
//);

// two port
// A is ready in one clock cycle
// use B later

//byteena_a,
//	clock,
//	data,
//	rdaddress,
//	rden,
//	wraddress,
//	wren,
//	q);
ram on_chip(
	.byteena_a(AVL_BYTE_EN),
	.clock(Clk), 
	.data(AVL_WRITEDATA),
	.rdaddress(ram_addr),
	.rden(1'b1),
	.wraddress(AVL_ADDR),
	.wren(AVL_WRITE & ~AVL_ADDR[11] & AVL_CS),
	.q(ram_out)
	);

logic [11:0] ram_addr;
logic [31:0] ram_out;
// if we are getting a palette location, we need to check the avl write
// and use the weird indexing to retrieve rgb. byte at a time

// ram 2-port
// input	[11:0]  address_a;
//	input	[11:0]  address_b;
//	input	[3:0]  byteena_a;
//	input	[3:0]  byteena_b;
//	input	  clock;
//	input	[31:0]  data_a;
//	input	[31:0]  data_b;
//	input	  rden_a;
//	input	  rden_b;
//	input	  wren_a;
//	input	  wren_b;
//	output	[31:0]  q_a;
//	output	[31:0]  q_b;


//always_ff @(posedge Clk or posedge Reset) begin
//	// avalon controls
//	if (Reset)
//	begin
//		// ???
////		AVL_WRITEDATA <= 32'h0;
//	end
//	
//	else if (AVL_READ)
//		AVL_READDATA <= ram_out_a;
//		
//	else if (AVL_WRITE)
//	begin
//		case (AVL_BYTE_EN)
//			4'b1111 : ram_data_a <= AVL_WRITEDATA;					 
//					 
//			4'b1100 : ram_data_a[31:16] <= AVL_WRITEDATA[31:16];
//
//			4'b0011 : ram_data_a[15:0] <= AVL_WRITEDATA[15:0];
//		
//			4'b1000 : ram_data_a[31:24] <= AVL_WRITEDATA[31:24];
//		
//			4'b0100 : ram_data_a[23:16] <= AVL_WRITEDATA[23:16];
//		
//			4'b0010 : ram_data_a[15:8] <= AVL_WRITEDATA[15:8];
//		
//			4'b0001 : ram_data_a[7:0] <= AVL_WRITEDATA[7:0];
//		endcase
//	end
//end

//always_ff @(posedge Clk or posedge Reset) begin
//
//	if (Reset)
//	begin
//		integer i;
//		for(i = 0; i < `NUM_REGS; i++)
//		LOCAL_REG[i] <= 32'h0;
//	end
//	
//	else if (AVL_WRITE)
//	begin
//	
//		case (AVL_BYTE_EN)  
//	
//		4'b1111 :  LOCAL_REG[AVL_ADDR] <= AVL_WRITEDATA;
//					 
//					 
//		4'b1100 : LOCAL_REG[AVL_ADDR][31:16] <= AVL_WRITEDATA[31:16];
//
//		4'b0011 : LOCAL_REG[AVL_ADDR][15:0] <= AVL_WRITEDATA[15:0];
//		
//		4'b1000 : LOCAL_REG[AVL_ADDR][31:24] <= AVL_WRITEDATA[31:24];
//		
//		4'b0100 : LOCAL_REG[AVL_ADDR][23:16] <= AVL_WRITEDATA[23:16];
//		
//		4'b0010 : LOCAL_REG[AVL_ADDR][15:8] <= AVL_WRITEDATA[15:8];
//		
//		4'b0001 : LOCAL_REG[AVL_ADDR][7:0] <= AVL_WRITEDATA[7:0];
//			
//		
//		endcase
//		
//	end
//	
//	
//	else if (AVL_READ)
//	begin
//		AVL_READDATA <= LOCAL_REG[AVL_ADDR];
//	end
//
//end

logic [11:0] A;
logic [15:0] char_data;
always_comb
	begin
	A = (DrawX[9:3]) + 80*DrawY[9:4]; // address of character (there are 2400)
	
	// 2 chars per register, find by dividing by 2
	// get 16 bit data: invert bit, CODE (to font rom), FGD_IDX, BGD_IDX
	// if A[0] is 1 choose code1
	// if A[0] is 0 choose code0
	ram_addr = A[11:1]; // gets data in one clock cycle
	
	char_data = A[0] ? (ram_out[31:16]) : (ram_out[15:0]);
	
	rom_addr = {char_data[14:8], DrawY[3:0]}; // get current row drawing
	bitdraw = font_rom_data[~DrawX[2:0]]; // get the bit pixel from font rom
	// bitdraw tells us if pixel is FGD or BGD
	
end

//logic [11:0] A;
//logic [7:0] char_data;
//always_comb
//	begin
//	A = (DrawX[9:3]) + 80*DrawY[9:4];
//	// output A[11:2] to read from vram
//	ram_addr = A[11:2];
//	case(A[1:0])
//		2'b00 : char_data = ram_out[7:0];
//		2'b01 : char_data = ram_out[15:8];
//		2'b10 : char_data = ram_out[23:16];
//		2'b11 : char_data = ram_out[31:24];
//	endcase
//
//	rom_addr = {char_data[6:0], DrawY[3:0]};
//	bitdraw = font_rom_data[~DrawX[2:0]];
//	
//end


always_ff@(posedge pixel_clk)
begin
	if (~blank)
	begin
		red <= 3'b000;
		green <= 3'b000;
		blue <= 3'b000;
	end

		
		else if (bitdraw ^ char_data[15])
		begin
			// foreground
			red <= char_data[4] ? PALETTE_REG[char_data[7:5]][23:20] : PALETTE_REG[char_data[7:5]][11:8];
			green <= char_data[4] ? PALETTE_REG[char_data[7:5]][19:16] : PALETTE_REG[char_data[7:5]][7:4];
			blue <= char_data[4] ? PALETTE_REG[char_data[7:5]][15:12] : PALETTE_REG[char_data[7:5]][3:0];
		end
		else
		begin
			// background		
			red <= char_data[0] ? PALETTE_REG[char_data[3:1]][23:20] : PALETTE_REG[char_data[3:1]][11:8];
			green <= char_data[0] ? PALETTE_REG[char_data[3:1]][19:16] : PALETTE_REG[char_data[3:1]][7:4];
			blue <= char_data[0] ? PALETTE_REG[char_data[3:1]][15:12] : PALETTE_REG[char_data[3:1]][3:0];
		end


end


/*
always_ff@(posedge pixel_clk)
begin
if(~blank)
	begin
		red <= 3'b000;
		green <= 3'b000;
		blue <= 3'b000;
	end
	else if (bitdraw ^ char_data[7])
	begin
		//foreground
		red <= FGR_R;
		green <= FGR_G;
		blue <= FGR_B;
	end
	else
	begin
		//background
		red <= BKG_R;
		green <= BKG_G;
		blue <= BKG_B;
	end


end
*/

























endmodule
