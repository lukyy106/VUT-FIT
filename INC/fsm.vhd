-- fsm.vhd: Finite State Machine
-- Author(s): 
--
library ieee;
use ieee.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   type t_state is (TEST_ERROR, TEST_FINAL, TEST_1, TEST_2, TEST_3x, TEST_4x, TEST_5x, TEST_6x, TEST_7x, TEST_8x, TEST_9x, TEST_10x, TEST_x3, TEST_x4, TEST_x5, TEST_x6, TEST_x7, TEST_x8, TEST_x9, TEST_x10, PRINT_ERROR, PRINT_OK, FINISH);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= TEST_1;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_1 =>
      next_state <= TEST_1;
		if (KEY(1) = '1') then
			next_state <= TEST_2;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERROR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= TEST_ERROR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_2 =>
      next_state <= TEST_2;
		if (KEY(1) = '1') then
			next_state <= TEST_3x;
		elsif (KEY(7) = '1') then
			next_state <= TEST_x3;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERROR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= TEST_ERROR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_3x =>
      next_state <= TEST_3x;
		if (KEY(7) = '1') then
			next_state <= TEST_4x;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERROR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= TEST_ERROR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_4x =>
      next_state <= TEST_4x;
		if (KEY(2) = '1') then
			next_state <= TEST_5x;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERROR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= TEST_ERROR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_5x =>
      next_state <= TEST_5x;
		if (KEY(3) = '1') then
			next_state <= TEST_6x;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERROR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= TEST_ERROR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_6x =>
      next_state <= TEST_6x;
		if (KEY(5) = '1') then
			next_state <= TEST_7x;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERROR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= TEST_ERROR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_7x =>
      next_state <= TEST_7x;
		if (KEY(0) = '1') then
			next_state <= TEST_8x;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERROR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= TEST_ERROR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_8x =>
      next_state <= TEST_8x;
		if (KEY(2) = '1') then
			next_state <= TEST_9x;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERROR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= TEST_ERROR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_9x =>
      next_state <= TEST_9x;
		if (KEY(0) = '1') then
			next_state <= TEST_10x;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERROR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= TEST_ERROR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_10x =>
      next_state <= TEST_10x;
		if (KEY(5) = '1') then
			next_state <= TEST_FINAL;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERROR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= TEST_ERROR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_x3 =>
      next_state <= TEST_x3;
		if (KEY(5) = '1') then
			next_state <= TEST_x4;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERROR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= TEST_ERROR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_x4 =>
      next_state <= TEST_x4;
		if (KEY(8) = '1') then
			next_state <= TEST_x5;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERROR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= TEST_ERROR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_x5 =>
      next_state <= TEST_x5;
		if (KEY(5) = '1') then
			next_state <= TEST_x6;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERROR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= TEST_ERROR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_x6 =>
      next_state <= TEST_x6;
		if (KEY(2) = '1') then
			next_state <= TEST_x7;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERROR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= TEST_ERROR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_x7 =>
      next_state <= TEST_x7;
		if (KEY(5) = '1') then
			next_state <= TEST_x8;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERROR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= TEST_ERROR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_x8 =>
      next_state <= TEST_x8;
		if (KEY(3) = '1') then
			next_state <= TEST_x9;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERROR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= TEST_ERROR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_x9 =>
      next_state <= TEST_x9;
		if (KEY(0) = '1') then
			next_state <= TEST_x10;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERROR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= TEST_ERROR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_x10 =>
      next_state <= TEST_x10;
		if (KEY(8) = '1') then
			next_state <= TEST_FINAL;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERROR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= TEST_ERROR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_FINAL =>
      next_state <= TEST_FINAL;
      if (KEY(15) = '1') then
         next_state <= PRINT_OK;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= TEST_ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_ERROR =>
      next_state <= TEST_ERROR;
      if (KEY(15) = '1') then
         next_state <= PRINT_ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_ERROR =>
      next_state <= PRINT_ERROR;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_OK =>
      next_state <= PRINT_OK;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      next_state <= FINISH;
      if (KEY(15) = '1') then
         next_state <= TEST_1; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      next_state <= TEST_1;
   end case;
end process next_state_logic;

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0';
   FSM_MX_MEM     <= '0';
   FSM_MX_LCD     <= '0';
   FSM_LCD_WR     <= '0';
   FSM_LCD_CLR    <= '0';

   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_ERROR | TEST_FINAL | TEST_1 | TEST_2 | TEST_3x | TEST_4x | TEST_5x | TEST_6x | TEST_7x | TEST_8x | TEST_9x | TEST_10x | TEST_x3 | TEST_x4 | TEST_x5 | TEST_x6 | TEST_x7 | TEST_x8 | TEST_x9 | TEST_x10 =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_ERROR =>
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
	when PRINT_OK =>
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
		FSM_MX_MEM     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
   end case;
end process output_logic;

end architecture behavioral;

