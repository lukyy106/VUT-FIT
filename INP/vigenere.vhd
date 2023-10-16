library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

-- rozhrani Vigenerovy sifry
entity vigenere is
   port(
         CLK : in std_logic;
         RST : in std_logic;
         DATA : in std_logic_vector(7 downto 0);
         KEY : in std_logic_vector(7 downto 0);

         CODE : out std_logic_vector(7 downto 0)
    );
end vigenere;


architecture behavioral of vigenere is

	signal shift: std_logic_vector(7 downto 0);
	signal corected_plus: std_logic_vector(7 downto 0);
	signal corected_minus: std_logic_vector(7 downto 0);
	type t_state is (add, reduce);
	signal state: t_state := add;
	signal next_state: t_state := reduce;
	signal output: std_logic_vector(1 downto 0);
	signal hashtag: std_logic_vector(7 downto 0) := "00100011";

begin


	state_logic: process (CLK, RST) is
	begin
		if RST = '1' then
			state <= add;
		elsif (CLK'event) and (CLK='1') then
			state <= next_state;
		end if;
	end process;

	fsm_mealy: process (state, DATA, RST) is
	begin
		if (state = add) then
			next_state <= reduce;
			output <= "01";
		elsif (state = reduce) then
			next_state <= add;
			output <= "10";
		end if;
		if (DATA > 47 and DATA < 58) then
			output <= "00";
		end if;

		if RST = '1' then 
			output <= "00";
		end if;

	end process;	

	shitf_process: process (DATA, KEY) is
	begin
		shift <= KEY - 64;
	end process;

	plus_process: process (shift, DATA) is
		variable tmp: std_logic_vector(7 downto 0);
	begin
		tmp := DATA + shift;
		if (tmp > 90) then
			tmp := tmp - 26;
		end if;
		corected_plus <= tmp;
	end process;

	minus_process: process (shift, DATA) is
		variable tmp: std_logic_vector(7 downto 0);
	begin
		tmp := DATA - shift;
		if (tmp < 65) then
			tmp := tmp + 26;
		end if;
		corected_minus <= tmp;
	end process;

	mux: process (output, corected_plus, corected_minus) is
	begin
		case output is
			when "01" =>
				CODE <= corected_plus;
			when "10" => 
				CODE <= corected_minus;
			when others =>
				CODE <= hashtag;
		end case;
	end process;

end behavioral;
