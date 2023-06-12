library ieee;
use ieee.std_logic_1164.all;

entity lab2 is
	port (clock_50 		: in std_logic;
			PS2_DAT			: in std_logic;
			vga_b 			: out std_logic_vector(7 downto 0);
			vga_g 			: out std_logic_vector(7 downto 0);
			vga_r 			: out std_logic_vector(7 downto 0);
			VGA_BLANK_N 	: out std_logic;
			VGA_CLK 			: out std_logic;
			VGA_HS 			: out std_logic;
			VGA_SYNC_N 		: out std_logic;
			VGA_VS 			: out std_logic;
			PS2_CLK 			: out std_logic;
			DRAM_CLK			: out std_logic_vector;
			DRAM_ADDR		: out std_logic_vector(12 downto 0);	
			DRAM_BA			: out std_logic_vector(2 downto 0);
			DRAM_CAS_N		: out std_logic;
			DRAM_CKE			: out std_logic;
			DRAM_CS_N		: out std_logic;
			DRAM_DQ			: out std_logic_vector(15 downto 0);
			DRAM_LDQM		: out std_logic;
			DRAM_UDQM		: out std_logic;
			DRAM_RAS_N		: out std_logic;
			DRAM_WE_N		: out std_logic
	);
end lab2;

architecture structural of lab2 is

	component lab2Arc is
		port(
			clk_clk                       : in    std_logic                     := '0';             --                     clk.clk
			ps2_external_CLK              : inout std_logic                     := '0';             --            ps2_external.CLK
			ps2_external_DAT              : inout std_logic                     := '0';             --                        .DAT
			reset_reset                   : in    std_logic                     := '0';             --                   reset.reset
			sdram_clk_clk                 : out   std_logic;                                        --               sdram_clk.clk
			sdram_wires_addr              : out   std_logic_vector(12 downto 0);                    --             sdram_wires.addr
			sdram_wires_ba                : out   std_logic_vector(1 downto 0);                     --                        .ba
			sdram_wires_cas_n             : out   std_logic;                                        --                        .cas_n
			sdram_wires_cke               : out   std_logic;                                        --                        .cke
			sdram_wires_cs_n              : out   std_logic;                                        --                        .cs_n
			sdram_wires_dq                : inout std_logic_vector(63 downto 0) := (others => '0'); --                        .dq
			sdram_wires_dqm               : out   std_logic_vector(7 downto 0);                     --                        .dqm
			sdram_wires_ras_n             : out   std_logic;                                        --                        .ras_n
			sdram_wires_we_n              : out   std_logic;                                        --                        .we_n
			vga_controller_external_CLK   : out   std_logic;                                        -- vga_controller_external.CLK
			vga_controller_external_HS    : out   std_logic;                                        --                        .HS
			vga_controller_external_VS    : out   std_logic;                                        --                        .VS
			vga_controller_external_BLANK : out   std_logic;                                        --                        .BLANK
			vga_controller_external_SYNC  : out   std_logic;                                        --                        .SYNC
			vga_controller_external_R     : out   std_logic_vector(7 downto 0);                     --                        .R
			vga_controller_external_G     : out   std_logic_vector(7 downto 0);                     --                        .G
			vga_controller_external_B     : out   std_logic_vector(7 downto 0)                      --                        .B
		);
	end component;
	
begin
	nios_system : lab2arc
	port map(
		clk_clk => clock_50,
		--reset_reset
		ps2_external_CLK 					=> PS2_CLK,
		ps2_external_DAT 					=> PS2_DAT,
		sdram_clk_clk 						=> DRAM_CLK,
		sdram_wires_addr 					=> DRAM_ADDR,
		sdram_wires_ba 					=> DRAM_BA,
		sdram_wires_cas_n 				=> DRAM_CAS_N,
		sdram_wires_cke 					=> DRAM_CKE,
		sdram_wires_cs_n 					=> DRAM_CS_N,
		sdram_wires_dq(15 downto 0) 	=> DRAM_DQ, --	pas un 64 bit de large
		--sdram_wires_dqm 									Pas certain de ce qu'il faut assign la dedans
		sdram_wires_ras_n 				=> DRAM_RAS_N,
		sdram_wires_we_n 					=> DRAM_WE_N,
		vga_controller_external_CLK 	=> VGA_CLK,
		vga_controller_external_HS 	=> VGA_HS,
		vga_controller_external_VS 	=> VGA_VS,
		vga_controller_external_BLANK => VGA_BLANK_N,
		vga_controller_external_SYNC 	=> VGA_SYNC_N,
		vga_controller_external_R 		=> vga_r,
		vga_controller_external_G 		=> vga_g,
		vga_controller_external_B 		=> vga_b
	);
end structural
		
		
		
		
		
			