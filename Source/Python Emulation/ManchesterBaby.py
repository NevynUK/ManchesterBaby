#!/usr/bin/env python
#
#   Class implementing the Manchester Baby controller for a console.
#
from Register import Register
from StoreLines import StoreLines
from CPU import CPU
from Instructions import Instructions

class ManchesterBaby:
    '''Controller for the Manchester Baby (SSEM): assembles programs, runs them and reports progress.'''
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
        self._uiUpdateDisplay = self._GetCallback(userInterface, 'UpdateDisplayTube')
        self._uiUpdateProgress = self._GetCallback(userInterface, 'UpdateProgress')
        self._uiDisplayError = self._GetCallback(userInterface, 'DisplayError')

#------------------------------------------------------------------------------
#
#                               Methods.
#
#------------------------------------------------------------------------------
    @staticmethod
    def _GetCallback(userInterface, name):
        '''Get the named method from the user interface, or None if it does not provide it.'''
        method = getattr(userInterface, name, None)
        if (callable(method)):
            return(method)
        return(None)

    def _RequireCpu(self):
        '''Make sure that a program has been assembled.

        @raises: RuntimeError Indicates that there is no program in the store lines.
        '''
        if ((self._cpu is None) or (self._cpu.StoreLines is None)):
            raise RuntimeError('No program has been assembled')

    def PrintRegisters(self):
        '''Display the contents of the registers.'''
        self._RequireCpu()
        print('AC: {} - {} {}'.format(self._cpu.Accumulator.Hex(), self._cpu.Accumulator.Binary(), self._cpu.Accumulator.ReverseBits()))
        print('CI: {} - {} {}'.format(self._cpu.CI.Hex(), self._cpu.CI.Binary(), self._cpu.CI.Value))
        print('PI: {} - {} {}'.format(self._cpu.PI.Hex(), self._cpu.PI.Binary(), self._instructions.Disassemble(self._cpu.PI.Value)))

    def Print(self):
        '''Print a readable version of the internal state of the CPU.'''
        self._RequireCpu()
        print('\n--------------- SSEM Machine State ---------------\n')
        self._cpu.StoreLines.Print()
        print('')
        self.PrintRegisters()
        print('\n--------------------------------------------------------------------------------')

    def Assembler(self, fileName, storeSize = 32):
        '''Open the specified file and convert the assembler instructions into
        binary and save into the storeLines.

        On success, the store lines in the _cpu object will hold the assembled application.

        @param: fileName Name of the file that is to be assembled and put into the store lines.
        @param: storeSize Number of store lines to create (default is 32, the same as the SSEM).

        @raises: ValueError Indicates a line that cannot be assembled (the message includes the line number).
        '''
        with open(fileName, "r") as source:
            lineNumber = 0
            storeLines = StoreLines(storeSize)
            for line in source:
                lineNumber += 1
                words = line.rstrip('\n').split()
                if ((len(words) == 0) or (words[0] == '--')):
                    continue
                try:
                    sl = int(words[0].strip(':'))
                    m = words[1].upper()
                    if (m == 'NUM'):
                        store = int(words[2])
                    elif ((m == 'BIN') or (m == 'BINS')):
                        store = Register(int(words[2], 2)).ReverseBits()
                    else:
                        i = self._instructions.Lookup(m)
                        if (not i):
                            raise ValueError('unknown instruction ' + words[1])
                        opcode = i[0]['opcode']
                        if (m in ['STOP', 'HLT', 'STP', 'CMP', 'SKN']):
                            ln = 0
                        else:
                            ln = int(words[2])
                        store = ln | (opcode << 13)
                    storeLines.SetLine(sl, Register(store))
                except (ValueError, IndexError) as error:
                    raise ValueError('Cannot process line {}: {} ({})'.format(lineNumber, line.rstrip('\n'), error))
            self._cpu = CPU(storeLines)

    def RunProgram(self, progress = None, updateDisplayTube = None, maxInstructions = None):
        '''Run the program contained in the store.

        @param: progress Optional callable taking the number of instructions executed so far, called every 1000 instructions.
        @param: updateDisplayTube Optional callable taking the store lines, called whenever the store changes.
        @param: maxInstructions Optional limit on the number of instructions executed.

        @raises: RuntimeError Indicates that maxInstructions was reached before the program stopped.

        @returns: Number of instructions executed.
        '''
        self._RequireCpu()
        if (progress is None):
            progress = self._uiUpdateProgress
        if (updateDisplayTube is None):
            updateDisplayTube = self._uiUpdateDisplay
        self._cpu.Reset()
        instructionCount = 0
        if (updateDisplayTube is not None):
            updateDisplayTube(self._cpu.StoreLines)
        try:
            while (not self._cpu.Stopped):
                if ((maxInstructions is not None) and (instructionCount >= maxInstructions)):
                    raise RuntimeError('Instruction limit of {} reached'.format(maxInstructions))
                self._cpu.SingleStep()
                instructionCount = instructionCount + 1
                if (((instructionCount % 1000) == 0) and (progress is not None)):
                    progress(instructionCount)
                if ((self._cpu.UpdateDisplayTube) and (updateDisplayTube is not None)):
                    updateDisplayTube(self._cpu.StoreLines)
        except (RuntimeError, ValueError, IndexError) as error:
            if (self._uiDisplayError is not None):
                self._uiDisplayError(str(error))
            raise
        return(instructionCount)

#------------------------------------------------------------------------------
#
#                               Tests.
#
#------------------------------------------------------------------------------
if (__name__ == '__main__'):
    import os
    import time
    baby = ManchesterBaby()
    baby.Assembler(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'Sources', 'hfr989.ssem'))
    baby.Print()
    start = time.monotonic()
    print('\nExecuting program:')
    instructionCount = baby.RunProgram()
    end = time.monotonic()
    baby.Print()
    print('\nExecuted', instructionCount, 'statements in', end-start, 'seconds')
