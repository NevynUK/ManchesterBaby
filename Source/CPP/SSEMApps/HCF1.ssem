01: LDN 24   -- Accumulator := -Initial Trial Divisor
02: STO 26   -- Store as -Trial Divisor
03: LDN 26   -- Accumulator := -(-Trial Divisor)
04: STO 27   -- Store as +Trial Divisor
05: LDN 23   -- Accumulator := -(-Number)
06: SUB 27   -- Subtract +Trial Divisor
07: SKN      -- Skip if Negative
08: JMR 20   --   otherwise go to line 6 via indirect relative jump
09: SUB 26   -- Subtract -Trial Divisor to get +Remainder
10: STO 25   -- Store Remainder
11: LDN 25   -- Accumulator := -Remainder
12: SKN      -- Skip if Negative ; Remainder is not Zero
13: HLT      --   otherwise Stop ; Trial Divisor divides Number
14: LDN 26   -- Accumulator := -(-Trial Divisor)
15: SUB 21   --   and Subtract +1 to decrement Trial Divisor
16: STO 27   -- Store new +Trial Divisor
17: LDN 27   -- Accumulator := -Trial Divisor
18: STO 26   -- Store -Trial Divisor
19: JMP 22   -- Go to Line 5
20: NUM -3   -- jump offset
21: NUM 1    -- constant
22: NUM 4    -- jump address
23: NUM -35  -- negative Number
24: NUM 34   -- Initial Trial Divisor (typically Number-1)
25: NUM 0    -- Remainder
26: NUM 0    -- -Trial Divisor
27: NUM 0    -- +Trial Divisor and Answer
