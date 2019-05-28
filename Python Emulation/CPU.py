#!/usr/bin/env python
#
#   CPU for the Manchester Baby.
#
import Register
import StoreLines

#
#   Dictionary containing the instruction set, the associted 'mnemonic' and the 'description' of the instruction.
#
#   Instructions are stored in a list with each entry being a dictionary item.  The dictionary item contains the
#   SSEM opcode for instruction along with information about the instruction:
#
#   { opcode: code, instruction: details }
#
#   The instruction detail is a dictionary item containing the following items:
#       Conventional twos complement form of the opcode.
#       mnemonic
#       English description of the purpose of the instruction.
#
instructions = [
    { 'opcode': 0, 'instruction': { 'twoComplementOpCode': 0, 'mnemonic': 'JMP', 'description': 'Copy the contents of store line to CI' }},
    { 'opcode': 1, 'instruction': { 'twoComplementOpCode': 4, 'mnemonic': 'JRP', 'description': 'Add the content of the store line to CI' }},
    { 'opcode': 2, 'instruction': { 'twoComplementOpCode': 2, 'mnemonic': 'LDN', 'description': 'Copy the content of the store line, negated, into the Accumulator' }},
    { 'opcode': 3, 'instruction': { 'twoComplementOpCode': 6, 'mnemonic': 'STO', 'description': 'Copy the contents of the Accumulator to the store line' }},
    { 'opcode': 4, 'instruction': { 'twoComplementOpCode': 1, 'mnemonic': 'SUB', 'description': 'Subtract the contents of the store line from the Accumulator' }},
    { 'opcode': 5, 'instruction': { 'twoComplementOpCode': 5, 'mnemonic': '---', 'description': 'Same as function number 4, SUB' }},
    { 'opcode': 6, 'instruction': { 'twoComplementOpCode': 3, 'mnemonic': 'CMP', 'description': 'Skip the next instruction if the content of the Accumulator is negative' }},
    { 'opcode': 7, 'instruction': { 'twoComplementOpCode': 7, 'mnemonic': 'STOP', 'description': 'Light the stop light and halt the machine' }}
    ]

#
#   Implement the SSEM CPU.
#
class CPU:
    def __init__(self, storeLines = None):
        '''Initialise the CPU.'''
        self.__ci = Register.Register()
        self.__pi = Register.Register()
        self.__storeLines = storeLines
        self.__accumulator = Register.Register()

    def PrintStoreLines(self):
        '''Print the contents of the store lines along with the disassembly.'''
        print '                 00000000001111111111222222222233'
        print '                 01234567890123456789012345678901'
        for lineNumber in range(self.__storeLines.Length):
            line = self.__storeLines.GetLine(lineNumber)
            print '{:02}: {} - {} {:16} ; {}'.format(lineNumber, line.Hex(), line.Binary(), self.Disassembly(line), self.ReverseBits(line.Value, 32))

    def PrintRegisters(self):
        '''Display the contents of the registers.'''
        print '\nAC: {} - {} {}'.format(self.__accumulator.Hex(), self.__accumulator.Binary(), self.ReverseBits(self.__accumulator.Value, 32))
        print 'CI: {} - {} {}'.format(self.__ci.Hex(), self.__ci.Binary(), self.ReverseBits(self.LineNumber(self.__ci.Value), 5))
        print 'PI: {} - {} {}'.format(self.__pi.Hex(), self.__pi.Binary(), self.Disassembly(self.__pi))

    def Print(self):
        '''Print a readable version of the internal state of the CPU.'''
        print '\n--------------- SSEM Machine State ---------------\n'
        self.PrintStoreLines()
        self.PrintRegisters()

    def LineNumber(self, value):
        '''Extract the line number from the supplied register object.'''
        return((value >> 27) & 0x1f)

    def Instruction(self, value):
        '''Extract the instruction from the supplied register object.'''
        return((value >> 16) & 0x7)

    def Disassembly(self, register):
        '''Disassemble the instruction in the specified register.'''
        mnemonic, lineNumber = self.DecodeInstruction(register)
        if ((mnemonic == 'STOP') or (mnemonic == 'CMP')):
            instruction = mnemonic
        else:
            instruction = '{} {}'.format(mnemonic, lineNumber)
        return(instruction)

    def ReverseBits(self, value, bitCount):
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

    def Add(self, registerA, registerB):
        '''Add the two registers together and return the result in a new Register object.'''
        a = self.ReverseBits(registerA.Value, 32)
        b = self.ReverseBits(registerB.Value, 32)
        return(Register.Register(self.ReverseBits((a + b) & 0xffffffff, 32)))

    def Sub(self, registerA, registerB):
        '''Subtract registerB from registerA and return the result as a new Register objectr.'''
        a = self.ReverseBits(registerA.Value, 32)
        b = self.ReverseBits(registerB.Value, 32)
        return(Register.Register(self.ReverseBits((a - b) & 0xffffffff, 32)))

    def DecodeInstruction(self, register):
        '''Decode the instruction in the specified register object.

        Returns the mnemonic and the store line number to be operated on.'''
        opcode = self.ReverseBits(self.Instruction(register.Value), 3)
        lineNumber = self.ReverseBits(self.LineNumber(register.Value), 5)
        instruction = instructions[opcode]
        return((instruction['instruction']['mnemonic'], lineNumber))

    def IncrementCI(self):
        '''Increment the control register by one.'''
        lineNumber = self.ReverseBits(self.__ci.Value, 32)
        lineNumber = (lineNumber + 1)
        self.__ci.Value = self.ReverseBits(lineNumber, 32)

    def RunProgram(self, debugging = False):
        '''Run the program contained in the store.'''
        self.__accumulator = Register.Register(0)
        running = True
        instructionCount = 0
        self.__ci = Register.Register(0)
        self.Print()
        print '\nExecuting program:'
        while running:
            #
            #   First, increment CI (the program counter).
            #
            self.IncrementCI()
            #
            #   Extract the store line given by CI from memory and put it in PI.
            #
            storeLineNumber = self.ReverseBits(self.LineNumber(self.__ci.Value), 5)
            self.__pi = self.__storeLines.GetLine(storeLineNumber)
            #
            #   Decode the instruction.
            #
            instructionCount += 1
            mnemonic, lineNumber = self.DecodeInstruction(self.__pi)
            if ((mnemonic == 'STOP') or (mnemonic == 'CMP')):
                instruction = mnemonic
            else:
                instruction = '{} {}'.format(mnemonic, lineNumber)
            print '{:-5} - {:02}: {}'.format(instructionCount, storeLineNumber, instruction)
            #
            #   Execute the instruction.
            #
            if (mnemonic == 'JMP'):
                self.__ci = Register.Register(self.__storeLines.GetLine(lineNumber).Value)
            elif (mnemonic == 'JRP'):
                self.__ci = self.Add(self.__ci, self.__storeLines.GetLine(lineNumber))
            elif (mnemonic == 'LDN'):
                line = self.__storeLines.GetLine(lineNumber)
                negatedValue = self.ReverseBits(line.Value, 32) * -1
                self.__accumulator.Value = self.ReverseBits(negatedValue, 32)
            elif (mnemonic == 'STO'):
                self.__storeLines.SetLine(lineNumber, Register.Register(self.__accumulator.Value))
            elif ((mnemonic == 'SUB') or (mnemonic == '---')):
                self.__accumulator = self.Sub(self.__accumulator, self.__storeLines.GetLine(lineNumber))
            elif (mnemonic == 'CMP'):
                if (self.__accumulator.Value & 0x1):
                    self.IncrementCI()
            elif (mnemonic == 'STOP'):
                running = False
            else:
                raise ValueError
            if (running and debugging):
                self.Print()
                command = raw_input()
                if (command == 'stop'): return
        self.Print()
        print 'Executed {} instruction(s)'.format(instructionCount)

#
#   Main program loop implementing tests for the CPU class.  The loop only executes
#   when the file is executed as a stand alone program.
#
if (__name__ == '__main__'):
    sl = StoreLines.StoreLines()
    cpu = CPU(sl)
    for i in instructions:
        if (i['opcode'] != cpu.ReverseBits(i['instruction']['twoComplementOpCode'], 3)):
            raise ValueError
#    if (cpu.Instruction(sl.GetLine(0).Value) != 0b100): raise ValueError
#    if (cpu.LineNumber(sl.GetLine(0).Value != 0b1000)): raise ValueError
    #                                  00000000001111111111222222222233
    #                                  01234567890123456789012345678901
    sl.SetLine(01, Register.Register(0b11001000000000010000000000000000))
    sl.SetLine(02, Register.Register(0b00000000000000110000000000000000))
    sl.SetLine(03, Register.Register(0b01010000000000000000000000000000))
    sl.SetLine(04, Register.Register(0b00000000000001110000000000000000))
    sl.SetLine(10, Register.Register(0b00000000000000000000000000000000))
    sl.SetLine(19, Register.Register(0b11001000000000000000000000000000))
    cpu.RunProgram()
    print 'CPU tests completed successfully.'
