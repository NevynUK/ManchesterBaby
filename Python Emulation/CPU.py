#!/usr/bin/env python
#
#   CPU for the Manchester Baby (SSEM).
#
import Register
import StoreLines
import Instructions

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
        self.PI = Register.Register(0)
        self.StoreLines = storeLines
        self.Reset()
        self.__instructions = Instructions.Instructions()
        self.UpdateDisplayTube = False

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
        '''Present instruction register (PI).'''
        return(self.__pi)

    def __SetPI(self, pi):
        '''Present instruction register (PI).'''
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

    #
    #   This property allows for the optimisation of the display tube updates.
    #   We only really need to update the display tube in two case:
    #
    #   * When the application is first loaded into the store lines
    #   * When we save a value into the store lines.
    #
    #   It is assumed that the diplay update is performed by the class controlling
    #   the stepping operation so we set this property when we put a new value
    #   into the store lines.
    #
    def __SetUpdateDisplayTube(self, updateDisplayLines):
        '''Indicate if we should update the display lines.'''
        self.__updateDisplayLines = updateDisplayLines

    def __GetUpdateDisplayTube(self):
        '''Indicate if we should update the display lines.'''
        return(self.__updateDisplayLines)

    UpdateDisplayTube = property(__GetUpdateDisplayTube, __SetUpdateDisplayTube, None, None)

#------------------------------------------------------------------------------
#
#                               Methods.
#
#------------------------------------------------------------------------------
    def Reset(self):
        '''Reset the CPU so that it is ready to execute the program in the store lines.'''
        self.Accumulator = Register.Register(0)
        self.CI = Register.Register(0)
        self.Stopped = False

    def SingleStep(self):
        '''Execute the next instruction.'''
        if (self.Stopped == True):
            raise RuntimeError
        #
        #   First, increment CI (the program counter).
        #
        self.CI.Value = (self.CI.Value + 1) & 0xffffffff
        #
        #   Extract the store line given by CI from memory and put it in PI.
        #
        storeLineNumber = self.__instructions.LineNumber(self.CI.Value)        
        self.PI = self.StoreLines.GetLine(storeLineNumber)
        #
        #   Decode the instruction.
        #
        opcode = self.__instructions.Opcode(self.PI.Value)
        lineNumber = self.__instructions.LineNumber(self.PI.Value)
        #
        #   Execute the instruction.
        #
        self.UpdateDisplayTube = False
        if (opcode == self.__instructions.OPCODE_JMP):
            self.CI = Register.Register(self.StoreLines.GetLine(lineNumber).Value)
        elif (opcode == self.__instructions.OPCODE_JRP):
            self.CI.Value = (self.CI.Value + self.StoreLines.GetLine(lineNumber).Value) & 0xffffffff
        elif (opcode == self.__instructions.OPCODE_LDN):
            self.Accumulator.Value = self.StoreLines.GetLine(lineNumber).Value * -1
        elif (opcode == self.__instructions.OPCODE_STO):
            self.StoreLines.SetLine(lineNumber, Register.Register(self.Accumulator.Value))
            self.UpdateDisplayTube = True
        elif ((opcode == self.__instructions.OPCODE_SUB) or (opcode == self.__instructions.OPCODE_UNDEFINED)):
            self.Accumulator.Value = (self.Accumulator.Value - self.StoreLines.GetLine(lineNumber).Value) & 0xffffffff
        elif (opcode == self.__instructions.OPCODE_CMP):
            if (self.Accumulator.Value & 0x80000000):
                self.CI.Value = (self.CI.Value + 1) & 0xffffffff
        elif (opcode == self.__instructions.OPCODE_STOP):
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
    # LDN 10
    sl.SetLine(1, Register.Register(0b0100000000001010))
    # SUB 11
    sl.SetLine(2, Register.Register(0b1000000000001011))
    # STO 12
    sl.SetLine(3, Register.Register(0b0110000000001100))
    # CMP (SKN)
    sl.SetLine(4, Register.Register(0b1100000000000000))
    # JMP 12 (should be store line 1 to make the next instruction executed store line 2)
    sl.SetLine(5, Register.Register(0b0000000000001100))
    # JRP 11 (Add 9 to CI, currently 6)
    sl.SetLine(6, Register.Register(0b0010000000001011))
    # STOP
    sl.SetLine(16, Register.Register(0b1110000000000000))
    # Number 10 (negated so that load will get the positive number).
    sl.SetLine(10, Register.Register(0xfffffff6))
    # Number 9
    sl.SetLine(11, Register.Register(9))
    # Number 0
    sl.SetLine(12, Register.Register(1))
    #
    #   Now execute the program one step at a time.
    #
    cpu.Reset()
    #
    #   LDN 10
    #
    cpu.SingleStep()
    if (cpu.CI.Value != 1):
        raise ValueError
    if (cpu.Accumulator.Value != 10):
        raise ValueError
    #
    #   SUB 11
    #
    cpu.SingleStep()
    if (cpu.CI.Value != 2):
        raise ValueError
    if (cpu.Accumulator.Value != 1):
        raise ValueError
    #
    #   STO 12
    #
    cpu.SingleStep()
    if (cpu.Accumulator.Value != 1):
        raise ValueError
    if (cpu.StoreLines.GetLine(12).Value != 1):
        raise ValueError
    #
    #   CMP
    #
    cpu.SingleStep()
    if (cpu.CI.Value != 4):
        raise ValueError
    #
    #   JMP 12, SUB 11, STO 12, CMP
    #
    cpu.SingleStep()
    cpu.SingleStep()
    cpu.SingleStep()
    cpu.SingleStep()
    #
    #   JRP 11
    #
    cpu.SingleStep()
    if (cpu.CI.Value != 15):
        raise ValueError
    #
    #   STOP
    #
    cpu.SingleStep()
    if (cpu.Stopped != True):
        raise ValueError
    #
    cpu.Stopped = True
    try:
        cpu.SingleStep()
    except RuntimeError:
        pass
    
    print('CPU tests completed successfully.')
