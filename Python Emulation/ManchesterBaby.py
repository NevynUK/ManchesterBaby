#!/usr/bin/env python
#
#   Class implementing the Manchester Baby controller for a console.
#
from Register import Register
from StoreLines import StoreLines
from CPU import CPU
from Instructions import Instructions
from ConsoleUserInterface import ConsoleUserInterface

class ManchesterBaby:
    '''
    '''
#------------------------------------------------------------------------------
#
#                       Class construction.
#
#------------------------------------------------------------------------------
    def __init__(self, userInterface = None):
        '''Construct a new ManchesterBaby (SSEM) object.
        
        @param: userInterface Class implementing the methods that will allow interaction with the user.'''
        self._instructions = Instructions()
        self._cpu = None
        if (userInterface == None):
            self._uiUpdateDisplay = None
            self._uiUpdateProgress = None
            self._uiDisplayError = None
        else:
            method = getattr(userInterface, 'UpdateDisplayTube', None)
            if (callable(method)):
                self._uiUpdateDisplay = method
            else:
                self._uiUpdateDisplay = None
            method = getattr(userInterface, 'UpdateProgress', None)
            if (callable(method)):
                self._uiUpdateProgress = method
            else:
                self._uiUpdateProgress = None
            method = getattr(userInterface, 'DisplayError', None)
            if (callable(method)):
                self._uiDisplayError = method
            else:
                self._uiDisplayError = None

#------------------------------------------------------------------------------
#
#                               Methods.
#
#------------------------------------------------------------------------------
    def PrintRegisters(self):
        '''Display the contents of the registers.'''
        print('AC: {} - {} {}'.format(self._cpu.Accumulator.Hex(), self._cpu.Accumulator.Binary(), self._cpu.Accumulator.ReverseBits()))
        print('CI: {} - {} {}'.format(self._cpu.CI.Hex(), self._cpu.CI.Binary(), self._cpu.CI.Value))
        print('PI: {} - {} {}'.format(self._cpu.PI.Hex(), self._cpu.PI.Binary(), self._instructions.Disassemble(self._cpu.PI.Value)))

    def Print(self):
        '''Print a readable version of the internal state of the CPU.'''
        print('\n--------------- SSEM Machine State ---------------\n')
        self._cpu.StoreLines.Print()
        print('')
        self.PrintRegisters()
        print('\n--------------------------------------------------------------------------------')

    def Assembler(self, fileName):
        '''Open the specified file and convert the assembler instructions into
        binary and save into the storeLines.

        On success, the store lines in the _cpu object will hold the assembled application.
        
        @param: fileName Name of the file in the Sources directory that is to be assembled and put into the store lines.
        '''
        with open(fileName, "r") as source:
            lineNumber = 0
            storeLines = StoreLines(32)
            for line in source:
                lineNumber += 1
                words = line.rstrip('\n').split()
                if (words[0] != '--'):
                    sl = int(words[0].strip(':'))
                    m = words[1].upper()
                    if (m == 'NUM'):
                        store = int(words[2])
                    elif  ((m == "BIN") or (m == "BINS")):
                        value = int(words[2], 2)
                        store = 0
                        bitCount = 32
                        while (bitCount > 0):
                            store <<= 1
                            if (value & 1):
                                store |= 1
                            value >>= 1
                            bitCount -= 1
                    else:
                        i = self._instructions.Lookup(m)
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
                    storeLines.SetLine(sl, Register(store))
            self._cpu = CPU(storeLines)

    def RunProgram(self, debugging = False):
        '''Run the program contained in the store.'''
        if (self._cpu.StoreLines == None):
            raise RuntimeError
        self._cpu.Reset()
        instructionCount = 0
        self._cpu.Stopped = False
        if (self._uiUpdateDisplay != None):
            self._uiUpdateDisplay(self._cpu.StoreLines)
        print('\nExecuting program:')
        while (self._cpu.Stopped == False):
            self._cpu.SingleStep()
            instructionCount = instructionCount + 1
            if (((instructionCount % 1000) == 0) and (self._uiUpdateProgress != None)):
                self._uiUpdateProgress(instructionCount)
            if ((self._cpu.UpdateDisplayTube) and (self._uiUpdateDisplay != None)):
                self._uiUpdateDisplay(self._cpu.StoreLines)
            # if (debugging):
            #     self.Print()
            #     command = raw_input()
            #     if (command == 'stop'): return
        return(instructionCount)

#------------------------------------------------------------------------------
#
#                               Tests.
#
#------------------------------------------------------------------------------
if (__name__ == '__main__'):
    baby = ManchesterBaby()
    baby.Assembler('Sources/hfr989.ssem')
    baby.Print()
    baby.RunProgram(debugging = False)
    baby.Print()
