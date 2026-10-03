# SSEM Assembler Files

The assembler provided in the ManchesterBaby.py file is primitive and provides little error checking. It is still useful for loading applications into the store lines ready for execution. The file format is best explained by examining a sample file:

```
--
--  Test the Load and subtract functions.
--
--  At the end of the program execution the accumulator should hold the value 15.
--
00: NUM 0
01: LDN 20      -- Load accumulator with the contents of line 20 (-A)
02: SUB 21      -- Subtract the contents of line 21 from the accumulator (-A - B)
03: STO 22      -- Store the result in line 22
04: LDN 22      -- Load accumulator (negated) with line 22 (-1 * (-A - B))
05: STOP        -- End of the program.
20: NUM 10      -- A
21: NUM 5       -- B
22: NUM 0       -- Result
```

Two minus signs indicate an inline comment. Everything following is ignored.

An instruction line has the following form:

```
Store line number: Instruction Operand
```

The *store line number* is the location in the Store that will be used hold the instruction or data.

*Instruction* is the mnemonic for the instruction. Some of the instructions have synonyms:

| Mnemonic | Synonyms   |
|----------|------------|
| JRP      | JPR, JMR   |
| CMP      | SKN        |
| STOP     | HLT, STP   |

As well as instructions a number may also be given using the *NUM* mnemonic.  Binary numbers are also allowed with the *BIN* or *BNUM* mnemonic.

All of the mnemonics requiring a store number (all except STOP and CMP) read the Operand field as the store line number. The *NUM* mnemonic stores the Operand in the store line as is.

## Compiler and Disassembler

The system contains a compiler that can take a text file and populate the store lines.

The *Register* class supports disassembling the contents of the register back into readable format.