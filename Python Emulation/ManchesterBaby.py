#!/usr/bin/env python
#
#   Class implementing the Manchester Baby controller for a console.
#
import Register
import StoreLines
import CPU
import Instructions

class ManchesterBaby:
    def __init__(self):
        '''Constructor'''
        self.__instructions = Instructions.Instructions()
        self.__cpu = None

    def PrintStoreLines(self):
        '''Print the contents of the store lines along with the disassembly.'''
        print('                 00000000001111111111222222222233')
        print('                 01234567890123456789012345678901')
        for lineNumber in range(self.__cpu.StoreLines.Length):
            line = self.__cpu.StoreLines.GetLine(lineNumber)
            decimal = line.Value
            if (decimal & 0x80000000):
                decimal -= 2**32
            print('{:02}: {} - {} {:16} ; {}'.format(lineNumber, line.Hex(), line.Binary(), self.Disassembly(line), decimal))

    def PrintRegisters(self):
        '''Display the contents of the registers.'''
        print('AC: {} - {} {}'.format(self.__cpu.Accumulator.Hex(), self.__cpu.Accumulator.Binary(), self.__cpu.Accumulator.ReverseBits()))
        print('CI: {} - {} {}'.format(self.__cpu.CI.Hex(), self.__cpu.CI.Binary(), self.__cpu.CI.Value))
        print('PI: {} - {} {}'.format(self.__cpu.PI.Hex(), self.__cpu.PI.Binary(), self.Disassembly(self.__cpu.PI)))

    def Print(self):
        '''Print a readable version of the internal state of the CPU.'''
        print('\n--------------- SSEM Machine State ---------------\n')
        self.PrintStoreLines()
        print('')
        self.PrintRegisters()

    def DecodeInstruction(self, register):
        '''Decode the instruction in the specified register object.

        Returns the mnemonic and the store line number to be operated on.'''
        opcode = self.__instructions.Opcode(register.Value)
        lineNumber = self.__instructions.LineNumber(register.Value)
        instruction = self.__instructions.Mnemonic(opcode)
        return(self.__instructions.Mnemonic(opcode), lineNumber)

    def Disassembly(self, register):
        '''Disassemble the instruction in the specified register.'''
        mnemonic, lineNumber = self.DecodeInstruction(register)
        if ((mnemonic == 'STOP') or (mnemonic == 'CMP')):
            instruction = mnemonic
        else:
            instruction = '{} {}'.format(mnemonic, lineNumber)
        return(instruction)

    def Assembler(self, fileName):
        '''Open the specified file and convert the assembler instructions into
        binary and save into the storeLines.'''
        with open(fileName, "r") as source:
            lineNumber = 0
            storeLines = StoreLines.StoreLines(32)
            for line in source:
                lineNumber += 1
                words = line.rstrip('\n').split()
                if (words[0] != '--'):
                    sl = int(words[0].strip(':'))
                    m = words[1].upper()
                    if (m == 'NUM'):
                        store = int(words[2])
                    else:
                        i = self.__instructions.Lookup(m)
                        if (i == None):
                            print('Cannot process line {}: {}'.format(lineNumber, line))
                            exit()
                        else:
                            opcode = i[0]['opcode']
                            if (m in ['STOP', 'HLT', 'CMP', 'SKN']):
                                ln = 0
                            else:
                                ln = int(words[2])
                            store = ln | (opcode << 13)
                    storeLines.SetLine(sl, Register.Register(store))
            self.__cpu = CPU.CPU(storeLines)

    def RunProgram(self, debugging = False):
        '''Run the program contained in the store.'''
        if (self.__cpu.StoreLines == None):
            raise RuntimeError
        self.__cpu.Reset()
        instructionCount = 0
        self.__cpu.Stopped = False
        print('\nExecuting program:')
        while (self.__cpu.Stopped == False):
            self.__cpu.SingleStep()
            instructionCount = instructionCount + 1
            # if (debugging):
            #     self.Print()
            #     command = raw_input()
            #     if (command == 'stop'): return
        print('Executed {} instruction(s)'.format(instructionCount))

#
#   The Manchester Baby
#
if (__name__ == '__main__'):
    baby = ManchesterBaby()
    baby.Assembler('Samples/hfr989.asm')
    baby.Print()
    baby.RunProgram(debugging = False)
    baby.Print()
