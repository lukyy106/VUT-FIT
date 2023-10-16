library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
use IEEE.STD_LOGIC_arith.ALL;
use ieee.numeric_std.all;
use work.newspaper_pack.all;
use work.counter;

--entity tlv_gp_ifc is
--	Port (
--		EN : in std_logic;
--		X : out std_logic_vector(45 downto 6);
--		clk : in std_logic
--		);
		
--end tlv_gp_ifc;

architecture Behavioral of tlv_gp_ifc is
	signal A : std_logic_vector(3 downto 0) := "0000";
	signal dir : std_logic_vector(3 downto 0) := "0000";
	signal C : std_logic_vector(3 downto 0) := "0000";
	signal f_anim : std_logic_vector(7 downto 0) := "00000000";
	signal cnt_anim : std_logic_vector(2 downto 0) := "000";
	type rom_array is array (0 to 15) of std_logic_vector (7 downto 0);
	signal rom : rom_array;
	signal STATE : std_logic_vector(1 downto 0) := "00";
	signal COL : integer range 0 to 15 := 0;
	signal col_cnt : integer := 0;
	signal R : std_logic_vector(7 downto 0) := "01011100";
	signal cnt : std_logic_vector(23 downto 0) := (others => '0');  -- 1s - 20MHz / 20M ~
	signal data : std_logic_vector(127 downto 0);
	signal obr4 :  std_logic_vector(127 downto 0) := "01110000011110000111110001111110011101100110110001111000011000000111100001111100011111100111011001101100011110000111000000000000";
	signal obr3 :  std_logic_vector(127 downto 0) := "00000000000110000000000000100100000110000100001000100100100110010010010001000010000110000010010000000000000110000000000000000000";
	signal obr2 :  std_logic_vector(127 downto 0) := "00011000001001000100001001000101010000100100010001000100010001000101010001000010001001010001001000010100000101000000100000000000";
	signal obr1 : std_logic_vector(127 downto 0) := 
		"00000000" &
		"00111000" &
		"01111110" &
		"01001110" &
		"01001000" &
		"01001110" &
		"00111110" &
		"00000000" &
		"00000000" &
		"00111000" &
		"01111110" &
		"01001110" &
		"01001000" &
		"01001110" &
		"00111110" &
		"00000000" ;

begin

gen: for i in 0 to 15 generate 
begin
		rom(i) <= GETCOLUMN(data, i, 8);
end generate;


	process (clk) is
		
	begin
		if RESET = '1' then
			data <= obr1;
			A <= "0000";
			dir <= "0000";
			C <= "0000";
			f_anim <= "00000000";
			cnt_anim <= "000";
			STATE <= "00";
			COL <= 0;
			col_cnt <= 0;
		elsif rising_edge(clk) then
			if STATE = "00" then
				data <= obr1;
			elsif STATE = "01" then
				data <= obr2;
			elsif STATE = "10" then
				data <= obr3;
			elsif STATE = "11" then
				data <= obr4;
			end if;
			cnt <= cnt + 1;
			
			if EN = '1' then
				
				if col_cnt < 97 then
					A <= A + 1;
					R <= rom(COL);
					COL <= conv_integer(C);
					C <= A + dir;
				elsif col_cnt = 97 then
					A <= "0000";
					R <= "00000001";
					f_anim <= "00000000";
					cnt_anim <= "000";
				else
					f_anim <= f_anim + 1;
					if f_anim = "00000000" then
						A <= A + 1;
						if A = "1111" then
							cnt_anim <= cnt_anim + 1;
							R <= R + R;
							if cnt_anim = "111" then
								col_cnt <= 0;
								STATE <= STATE + 1;
							end if;
						end if;
					end if;
				end if;
			end if;
			if conv_integer(cnt) = 0 then
				col_cnt <= col_cnt + 1;
				if col_cnt < 49 then
					dir <= dir + 1;
				elsif col_cnt < 97 then
					dir <= dir - 1;
				elsif col_cnt < 145 then

				end if;
			end if;
		end if;
	end process;

	

    -- mapovani vystupu
    -- nemenit
    X(6) <= A(3);
    X(8) <= A(1);
    X(10) <= A(0);
    X(7) <= '0'; -- en_n
    X(9) <= A(2);

    X(16) <= R(1);
    X(18) <= R(0);
    X(20) <= R(7);
    X(22) <= R(2);
  
    X(17) <= R(4);
    X(19) <= R(3);
    X(21) <= R(6);
    X(23) <= R(5);
end Behavioral;

