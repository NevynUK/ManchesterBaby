#!/usr/bin/env python
#
#   CPU for the Manchester Baby.
#
import Register
import StoreLines

#
#   Dictionary containing the instruction set, the associated 'mnemonic' and the 'description' of the instruction.
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
    '''Implement the methods needed to simulate the operation of the CPU
    in the SSEM (Manchester Baby).'''
#------------------------------------------------------------------------------
#
#                       Class construction.
#
#------------------------------------------------------------------------------
    def __init__(self, storeLines = None):
        '''Initialise the CPU.'''
        self.CI = Register.Register()
        self.PI = Register.Register()
        self.StoreLines = storeLines
        self.Accumulator = Register.Register()
        self.Stopped = True

#------------------------------------------------------------------------------
#
#                           Properties.
#
#------------------------------------------------------------------------------
    def __GetStopped(self):
        '''Is the CPU stopped?'''
        return(self.__stopped)

    def __SetStopped(self, stopped):
        '''Is the CPU stopped'''
        self.__stopped = stopped

    Stopped = property(__GetStopped, __SetStopped, None, None)

    def __GetCI(self):
        '''Current instruction register (CI)'''
        return(self.__ci)

    def __SetCI(self, ci):
        '''Current instruction register (CI)'''
        self.__ci = ci

    CI = property(__GetCI, __SetCI, None, None)

    def __GetPI(self):
        '''Present instruction register.'''
        return(self.__pi)

    def __SetPI(self, pi):
        '''Present instruction register.'''
        self.__pi = pi

    PI = property(__GetPI, __SetPI, None, None)

    def __GetAccumulator(self):
        '''Accumulator register.'''
        return(self.__accumulator)

    def __SetAccumulator(self, accumulator):
        '''Accumulator register.'''
        self.__accumulator = accumulator

    Accumulator = property(__GetAccumulator, __SetAccumulator, None, None)

    def __GetStoreLines(self):
        '''Store lines holding the application to be / being executed.'''
        return(self.__storeLines)

    def __SetStoreLines(self, storeLines):
        '''Store lines holding the application to be / being executed.'''
        self.__storeLines = storeLines

    StoreLines = property(__GetStoreLines, __SetStoreLines, None, None)

#------------------------------------------------------------------------------
#
#                               Methods.
#
#------------------------------------------------------------------------------
    def PrintStoreLines(self):
        '''Print the contents of the store lines along with the disassembly.'''
        print('                 00000000001111111111222222222233')
        print('                 01234567890123456789012345678901')
        for lineNumber in range(self.StoreLines.Length):
            line = self.StoreLines.GetLine(lineNumber)
            print('{:02}: {} - {} {:16} ; {}'.format(lineNumber, line.Hex(), line.Binary(), self.Disassembly(line), self.ReverseBits(line.Value, 32)))

    def PrintRegisters(self):
        '''Display the contents of the registers.'''
        print('\nAC: {} - {} {}'.format(self.Accumulator.Hex(), self.Accumulator.Binary(), self.ReverseBits(self.Accumulator.Value, 32)))
        print('CI: {} - {} {}'.format(self.CI.Hex(), self.CI.Binary(), self.ReverseBits(self.LineNumber(self.CI.Value), 5)))
        print('PI: {} - {} {}'.format(self.PI.Hex(), self.PI.Binary(), self.Disassembly(self.PI)))

    def Print(self):
        '''Print a readable version of the internal state of the CPU.'''
        print('\n--------------- SSEM Machine State ---------------\n')
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

    def Reset(self):
        '''Reset the CPU so that it is ready to execute the program in the store lines.'''
        self.Accumulator = Register.Register(0)
        self.CI = Register.Register(0)
        self.Stopped = True
        self.Print()

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
        '''Subtract registerB from registerA and return the result as a new Register object.'''
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
        lineNumber = self.ReverseBits(self.CI.Value, 32)
        lineNumber = (lineNumber + 1)
        self.CI.Value = self.ReverseBits(lineNumber, 32)
    
    def SingleStep(self):
        '''Execute the next instruction.'''
        #
        #   First, increment CI (the program counter).
        #
        self.IncrementCI()
        #
        #   Extract the store line given by CI from memory and put it in PI.
        #
        storeLineNumber = self.ReverseBits(self.LineNumber(self.CI.Value), 5)
        self.PI = self.StoreLines.GetLine(storeLineNumber)
        #
        #   Decode the instruction.
        #
        mnemonic, lineNumber = self.DecodeInstruction(self.PI)
        # if ((mnemonic == 'STOP') or (mnemonic == 'CMP')):
        #     instruction = mnemonic
        # else:
        #     instruction = '{} {}'.format(mnemonic, lineNumber)
        # print('{:-5} - {:02}: {}'.format(instructionCount, storeLineNumber, instruction))
        #
        #   Execute the instruction.
        #
        if (mnemonic == 'JMP'):
            self.CI = Register.Register(self.StoreLines.GetLine(lineNumber).Value)
        elif (mnemonic == 'JRP'):
            self.CI = self.Add(self.CI, self.StoreLines.GetLine(lineNumber))
        elif (mnemonic == 'LDN'):
            line = self.StoreLines.GetLine(lineNumber)
            negatedValue = self.ReverseBits(line.Value, 32) * -1
            self.Accumulator.Value = self.ReverseBits(negatedValue, 32)
        elif (mnemonic == 'STO'):
            self.StoreLines.SetLine(lineNumber, Register.Register(self.Accumulator.Value))
        elif ((mnemonic == 'SUB') or (mnemonic == '---')):
            self.Accumulator = self.Sub(self.Accumulator, self.StoreLines.GetLine(lineNumber))
        elif (mnemonic == 'CMP'):
            if (self.Accumulator.Value & 0x1):
                self.IncrementCI()
        elif (mnemonic == 'STOP'):
            self.Stopped = True
        else:
            raise ValueError

#------------------------------------------------------------------------------
#
#                               Tests.
#
#------------------------------------------------------------------------------

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
    # if (cpu.Instruction(sl.GetLine(0).Value) != 0b100):
    #    raise ValueError
    # if (cpu.LineNumber(sl.GetLine(0).Value != 0b1000)):
    #    raise ValueError
    #                                  00000000001111111111222222222233
    #                                  01234567890123456789012345678901
    sl.SetLine(1, Register.Register(0b11001000000000010000000000000000))
    sl.SetLine(2, Register.Register(0b00000000000000110000000000000000))
    sl.SetLine(3, Register.Register(0b01010000000000000000000000000000))
    sl.SetLine(4, Register.Register(0b00000000000001110000000000000000))
    sl.SetLine(10, Register.Register(0b00000000000000000000000000000000))
    sl.SetLine(19, Register.Register(0b11001000000000000000000000000000))
    # cpu.RunProgram()
    print('CPU tests completed successfully.')
