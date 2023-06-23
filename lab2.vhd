-------------------------------------------------------------------------------
--lab2.vhd
--
--	created on 2023-05-05
--
--	Autor: Robin Galipeau, Nicolas Gagnier
--
--	Description:
--		Material implementation of a VGA video output and a PS2 mouse port
--		used to support a simple drawing aplication on a NIOS II cpu core.
--		
--		module used:	-Onchip memory						--Nios2 core
--							-SDRAM	controller				--JTAG controller
--							-pixel and carachter buffer	--PS2 contoller
--							-RGB resample						--sysID
--							-Scaler								--IO port for push button
--							-Alpha Blender
--							-FIFO
--							-VGA controller
--							video PLL
--					 
-------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;

entity lab2 is
	port (clock_50 		: in std_logic;
			PS2_DAT			: inout std_logic;
			key				: in std_logic_vector(3 downto 0);
			vga_b 			: out std_logic_vector(7 downto 0);
			vga_g 			: out std_logic_vector(7 downto 0);
			vga_r 			: out std_logic_vector(7 downto 0);
			VGA_BLANK_N 	: out std_logic;
			VGA_CLK 			: out std_logic;
			VGA_HS 			: out std_logic;
			VGA_SYNC_N 		: out std_logic;
			VGA_VS 			: out std_logic;
			PS2_CLK 			: inout std_logic;
			DRAM_CLK			: out std_logic;
			DRAM_ADDR		: out std_logic_vector(12 downto 0);	
			DRAM_BA			: out std_logic_vector(1 downto 0);
			DRAM_CAS_N		: out std_logic;
			DRAM_CKE			: out std_logic;
			DRAM_CS_N		: out std_logic;
			DRAM_DQ			: inout std_logic_vector(15 downto 0);
			DRAM_LDQM		: out std_logic;
			DRAM_UDQM		: out std_logic;
			DRAM_RAS_N		: out std_logic;
			DRAM_WE_N		: out std_logic
	);
end lab2;

architecture structural of lab2 is

	signal reset_n : std_logic;

	component lab2Arc is
		port(
			-- clk.clk		
			clk_clk                       : in    std_logic	:= '0';
			-- ps2_external.CLK			
			ps2_external_CLK              : inout std_logic := '0';
			-- .DAT			
			ps2_external_DAT              : inout std_logic := '0';
			-- reset.reset			
			reset_reset                   : in    std_logic := '0';             
			-- sdram_clk.clk
			sdram_clk_clk                 : out   std_logic;     
			-- sdram_wires.addr
			sdram_wires_addr              : out   std_logic_vector(12 downto 0);
			-- .ba
			sdram_wires_ba                : out   std_logic_vector(1 downto 0);
			-- .cas_n
			sdram_wires_cas_n             : out   std_logic;
			-- .cke
			sdram_wires_cke               : out   std_logic;
			-- .cs_n
			sdram_wires_cs_n              : out   std_logic; 
			-- .dq
			sdram_wires_dq                : inout std_logic_vector(15 downto 0) 
														:= (others => '0');
			-- .dqm
			sdram_wires_dqm               : out   std_logic_vector(1 downto 0);      
			-- .ras_n
			sdram_wires_ras_n             : out   std_logic;     
			-- .we_n
			sdram_wires_we_n              : out   std_logic;      
			-- vga_controller_external.CLK
			vga_controller_external_CLK   : out   std_logic;
			-- .HS
			vga_controller_external_HS    : out   std_logic;  
			-- .VS
			vga_controller_external_VS    : out   std_logic;    
			-- .BLANK
			vga_controller_external_BLANK : out   std_logic;    
			-- .SYNC
			vga_controller_external_SYNC  : out   std_logic;   
			-- .R
			vga_controller_external_R     : out   std_logic_vector(7 downto 0);     
			-- .G
			vga_controller_external_G     : out   std_logic_vector(7 downto 0); 
			-- .B
			vga_controller_external_B     : out   std_logic_vector(7 downto 0)                      
		);
	end component;
	
begin
	reset_n <= not key(3); --invert the cpu reset signal from KEY(3)
	
	
	nios_system : lab2arc
	port map(
		clk_clk 								=> clock_50,
		reset_reset 						=> reset_n,
		--connection to PS2 port
		ps2_external_CLK 					=> PS2_CLK,
		ps2_external_DAT 					=> PS2_DAT,
		--connections to SDRAM chip
		sdram_clk_clk 						=> DRAM_CLK,
		sdram_wires_addr 					=> DRAM_ADDR,
		sdram_wires_ba 					=> DRAM_BA,
		sdram_wires_cas_n 				=> DRAM_CAS_N,
		sdram_wires_cke 					=> DRAM_CKE,
		sdram_wires_cs_n 					=> DRAM_CS_N,
		sdram_wires_dq					 	=> DRAM_DQ, 
		sdram_wires_dqm(0)				=> DRAM_LDQM, 
		sdram_wires_dqm(1)				=> DRAM_UDQM,
		sdram_wires_ras_n 				=> DRAM_RAS_N,
		sdram_wires_we_n 					=> DRAM_WE_N,
		--Connection to VGA port
		vga_controller_external_CLK 	=> VGA_CLK,
		vga_controller_external_HS 	=> VGA_HS,
		vga_controller_external_VS 	=> VGA_VS,
		vga_controller_external_BLANK => VGA_BLANK_N,
		vga_controller_external_SYNC 	=> VGA_SYNC_N,
		vga_controller_external_R 		=> vga_r,
		vga_controller_external_G 		=> vga_g,
		vga_controller_external_B 		=> vga_b
	);
end structural;
			