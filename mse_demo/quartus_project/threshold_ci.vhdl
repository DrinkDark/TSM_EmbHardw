library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;

entity sobel_threshold_ci is
  port (
    signal sum          : in std_logic_vector(31 downto 0);
    signal threshold    : in std_logic_vector(31 downto 0);
    signal thresholdResult : out std_logic_vector(31 downto 0)
  );
end sobel_threshold_ci;

architecture behavioral of sobel_threshold_ci is

begin
  process(sum)
  begin
    if sum > threshold then
        thresholdResult <= (others=>'1');
    else
        thresholdResult <= (others=>'0');
    end if;
  end process;
end behavioral;
