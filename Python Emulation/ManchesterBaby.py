#!/usr/bin/env python
#
#   Manchester Baby simulator
#
import Register
import StoreLines
import CPU

#   Dictionary containing the instruction set, the associated 'mnemonic' and the 'description' of the instruction.
#
#   Instructions are stored in a list with each entry being a dictionary item.  The dictionary item contains the
#   SSEM opcode for instruction along with information about the instruction:
#
#   { mnemonic: code, instruction: details }
#
#   The instruction detail is a dictionary item containing the following items:
#       Opcode
#       English description of the purpose of the instruction.
#
instructions = [
    { 'mnemonic': 'JMP', 'opcode': 0, 'description': 'Copy the contents of store line to CI' },
    { 'mnemonic': 'JRP', 'opcode': 1, 'description': 'Add the content of the store line to CI' },
    { 'mnemonic': 'JPR', 'opcode': 1, 'description': 'Add the content of the store line to CI' },
    { 'mnemonic': 'JMR', 'opcode': 1, 'description': 'Add the content of the store line to CI' },
    { 'mnemonic': 'LDN', 'opcode': 2, 'description': 'Copy the content of the store line, negated, into the Accumulator' },
    { 'mnemonic': 'STO', 'opcode': 3, 'description': 'Copy the contents of the Accumulator to the store line' },
    { 'mnemonic': 'SUB', 'opcode': 4, 'description': 'Subtract the contents of the store line from the Accumulator' },
    { 'mnemonic': 'CMP', 'opcode': 6, 'description': 'Skip the next instruction if the content of the Accumulator is negative' },
    { 'mnemonic': 'SKN', 'opcode': 6, 'description': 'Skip the next instruction if the content of the Accumulator is negative' },
    { 'mnemonic': 'STOP', 'opcode': 7, 'description': 'Light the stop light and halt the machine' },
    { 'mnemonic': 'HLT', 'opcode': 7, 'description': 'Light the stop light and halt the machine' },
    { 'mnemonic': 'STP', 'opcode': 7, 'description': 'Light the stop light and halt the machine' }
    ]

def ReverseBits(value, bitCount = 32):
    '''Reverse the bits in the specified value.  This method provides the CPU
    with the ability to translate SSEM numbers into conventional twos complement
    numbers used in modern computers.

    SSEM numbers are twos complement numbers with the LSB and MSB reversed
    compared to conventional twos complement form.'''
    result = 0
    while (bitCount > 0):
        result <<= 1
        if (value & 1):
            result |= 1
        value >>= 1
        bitCount -= 1
    return(result)

def Instruction(name):
    '''Look up the mnemonic to obtain the instruction information.
    '''
    i = [element for element in instructions if element['mnemonic'] == name]
    if (len(i) != 1):
        result = None
    else:
        result = i[0]
    return result

def Assembler(fileName, storeLines):
    '''Open the specified file and convert the assembler instructions into
    binary and save into the storeLines.'''
    with open(fileName, "r") as source:
        lineNumber = 0
        for line in source:
            lineNumber += 1
            words = line.rstrip('\n').split()
            if (words[0] != '--'):
                sl = int(words[0].strip(':'))
                m = words[1].upper()
                if (m == 'NUM'):
                    store = ReverseBits(int(words[2]))
                else:
                    #
                    #   The instruction below will work with python on computer but not with CircuitPython.
                    #
                    # i = next((i for i, x in enumerate(instructions) if (x['mnemonic'] == m)), None)
                    #
                    #   The instruction below works with both python on a computer and with CircuitPython.
                    #
                    i = Instruction(m)
                    if (i == None):
                        print('Cannot process line {}: {}'.format(lineNumber, line))
                        exit()
                    else:
                        opcode = i['opcode']
                        if (m in ['STOP', 'HLT', 'CMP', 'SKN']):
                            ln = 0
                        else:
                            ln = int(words[2])
                        store = ReverseBits(ln | (opcode << 13))
                storeLines.SetLine(sl, Register.Register(store))

def RunProgram(cpu, debugging = False):
    '''Run the program contained in the store.'''
    cpu.Reset()
    instructionCount = 0
    cpu.Stopped = False
    print('\nExecuting program:')
    while (cpu.Stopped == False):
        cpu.SingleStep()
        instructionCount = instructionCount + 1
        # if (debugging):
        #     self.Print()
        #     command = raw_input()
        #     if (command == 'stop'): return
    cpu.Print()
    print('Executed {} instruction(s)'.format(instructionCount))

#
#   The Manchester Baby
#
if (__name__ == '__main__'):
    storeLines = StoreLines.StoreLines(32)
    Assembler('Samples/hfr989.asm', storeLines)
    cpu = CPU.CPU(storeLines)
    RunProgram(cpu, debugging = False)
    # cpu.RunProgram(debugging = False)
