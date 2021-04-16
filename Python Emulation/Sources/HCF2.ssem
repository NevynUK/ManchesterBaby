00: NUM 0    -- jump address
01: LDN 30   -- Accumulator := -A
02: STO 29   -- Store -A
03: LDN 31   -- Accumulator := -B
04: STO 31   -- store -B
05: LDN 31   -- Accumulator := -(-B) = B
06: STO 30   -- Store B as new A
07: LDN 29   -- Accumulator := -(-A) ; old A from line 29
08: SUB 30   -- Subtract B so Accumulator equals A - B*n
09: SKN      -- Skip if negative: B*n > A
10: JPR 27   --   otherwise go to line 8 and subtract B again
11: SUB 31   -- Add B to A-B*n to obtain Remainder
12: STO 31   -- Store Remainder as new B
13: SUB 28   -- Subtract +2 to check if Remainder>=2
14: SKN      -- Skip if Negative as Remainder<2
15: JMP 0    --   otherwise go to line 1 and repeat
16: HLT      -- Halt if Remainder 0 or 1
27: NUM -3   -- jump address
28: NUM 2    -- constant
29: NUM 0    -- will hold -A
30: NUM 314159265  -- A is the Larger Number (Dividend)
31: NUM 271828183  -- B is the Smaller Number (Divisor)
