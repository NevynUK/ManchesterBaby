00: NUM 19   -- jump address
01: LDN 31   -- Accumulator := -A
02: STO 31   -- Store as -A
03: LDN 31   -- Accumulator := -(-A) i.e., +A
04: SUB 30   -- Subtract B*2^n ; Accumulator = A - B*2^n
05: SKN      -- Skip if (A-B*2^n) is Negative
06: JMP 0    --   otherwise go to line 20 ( A-B*2^n >= 0 )
07: LDN 31   -- Accumulator := -(-A)
08: STO 31   -- Store as +A
09: LDN 28   -- Accumulator := -Quotient
10: SUB 28   -- Accumulator := -Quotient - Quotient (up-shift)
11: STO 28   -- Store -2*Quotient as Quotient (up-shifted)
12: LDN 31   -- Accumulator := -A
13: SUB 31   -- Accumulator := -A-A (up-shift A)
14: STO 31   -- Store -2*A (up-shifted A)
15: LDN 28   -- Accumulator := -Quotient
16: STO 28   -- Store as +Quotient (restore shifted Quotient)
17: SKN      -- Skip if MSB of Quotient is 1 (at end)
18: JMP 26   --   otherwise go to line 3 (repeat)
19: HLT      -- Stop ; Quotient in line 28
20: STO 31   -- From line 6 - Store A-B*2^n as A
21: LDN 29   -- Routine to set bit d of Quotient
22: SUB 28   --   and up-shift
23: SUB 28   --   Quotient
24: STO 28   -- Store -(2*Quotient)-1 as Quotient
25: JMP 27   -- Go to line 12
26: NUM 2    -- jump address
27: NUM 11   -- jump address
28:          -- Quotient (Answer appears here)
29: BNUM 00000000000000000000000000000100 -- 2^d where d=31-n, see line 30 for n
30: NUM 20     -- B (Divisor*2^n) (example: 5*2^2=20)
31: NUM 36     -- A (initial Dividend) (example: 36/5=7)
