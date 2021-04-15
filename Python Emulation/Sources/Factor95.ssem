--
--  Test the Jump (JMP) instruction.
--
--  At the end of the program the accumulator will hold 1 for a failure
--  and 2 for success.
--
00: NUM 0
01: JMP 28      -- Set CI to the link number in store line 28
02: LDN 29      -- Set the accumulator to value in store line 29 (i.e JMP has failed.)
03: STOP
10: LDN 30      -- set the accumulator to the success value.
11: STOP        -- Store it again.
28: NUM 9       -- Line number for the success condition.
29: NUM -1      -- Fail condition value.
30: NUM -2      -- Success condition value.
