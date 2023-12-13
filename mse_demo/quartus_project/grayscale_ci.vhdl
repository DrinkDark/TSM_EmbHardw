library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
use IEEE.NUMERIC_STD.ALL;

entity Grayscale is
    Port (a : in  STD_LOGIC_VECTOR(31 downto 0);
          result : out  STD_LOGIC_VECTOR(31 downto 0));

	SIGNAL red : std_logic_vector (4 DOWNTO 0);
	SIGNAL r1 : std_logic_vector(15 DOWNTO 0);
	SIGNAL r2 : std_logic_vector(15 DOWNTO 0);
	
	SIGNAL green : std_logic_vector (4 DOWNTO 0);
	SIGNAL g1 : std_logic_vector(15 DOWNTO 0);
	SIGNAL g2 : std_logic_vector(15 DOWNTO 0);
	SIGNAL g3 : std_logic_vector(15 DOWNTO 0);

	SIGNAL blue : std_logic_vector (4 DOWNTO 0);
	SIGNAL b1 : std_logic_vector(15 DOWNTO 0);
	
	SIGNAL sum : std_logic_vector (15 DOWNTO 0);
		   
end Grayscale;
	
	

architecture Behavioral of Grayscale is
begin
	
	blue <= a(4 DOWNTO 0);
	green <= a(10 DOWNTO 6);
	red <= a(15 DOWNTO 11);
	
	g1(15 DOWNTO 11) <= "00000";
	g1(10 DOWNTO 6) <= green;
	g1(5 DOWNTO 0) <= "000000";
	
	g2(15 DOWNTO 9) <= "0000000";
	g2(8 DOWNTO 4) <= green;
	g2(3 DOWNTO 0) <= "0000";
	
	g3(15 DOWNTO 8) <= "00000000";
	g3(7 DOWNTO 3) <= green;
	g3(2 DOWNTO 0) <= "000";
	
	r1(15 DOWNTO 9) <= "0000000";
	r1(8 DOWNTO 4) <= red;
	r1(3 DOWNTO 0) <= "0000";
	
	r2(15 DOWNTO 8) <= "00000000";
	r2(7 DOWNTO 3) <= blue;
	r2(2 DOWNTO 0) <= "000";
	
	b1(15 DOWNTO 9) <= "0000000";
	b1(8 DOWNTO 4) <= blue;
	b1(3 DOWNTO 0) <= "0000";
	
	sum <= g1 + g2 + g3 +  r1 + r2 + b1;
	
	result(11 DOWNTO 0) <= sum(15 DOWNTO 4);
	result(31 DOWNTO 12) <= "00000000000000000000";
 	
end Behavioral;