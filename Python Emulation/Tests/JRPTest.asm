--
--  Test the JRP instruction.
--
--  If successful the accumulator will hold the value 2.
01: JRP 10
02: LDN 11
03: STOP
04: LDN 12
05: STOP
10: NUM 2
11: NUM -1
12: NUM -2
