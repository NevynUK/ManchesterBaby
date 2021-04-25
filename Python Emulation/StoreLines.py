#!/usr/bin/env python
#
#   Class implementing the store lines for the Manchester Baby.
#
from Register import Register
from Instructions import Instructions

#
#   Arbitrary maximum number of lines in the store.  The SSEM has 32 store lines
#   but this implementation allows for a larger maximum number of lines.
#
MAX_STORE_SIZE = 1024

#
#   Implement the store in the SSEM.
#
class StoreLines:
    '''Store lines holding the program and data for the CPU.'''

#------------------------------------------------------------------------------
#
#                       Class construction.
#
#------------------------------------------------------------------------------
    def __init__(self, size = 32):
        '''Construct a new StoreLines object with the specified number of lines
        in the store.  The default number of lines is 32, the same as the SSEM.
        
        @param: size Number of Registers to create in the store lines (default is 32).
        '''
        if ((size > 0) and (size < MAX_STORE_SIZE)):
            self._storeLines = [Register() for x in range(size)]
        else:
            raise ValueError
        self._instructions = Instructions()

#------------------------------------------------------------------------------
#
#                           Properties.
#
#------------------------------------------------------------------------------
    @property
    def Length(self):
        '''Get the number of store lines in this object.
        
        @returns: Number of Registers in the store lines.
        '''
        return(len(self._storeLines))

#------------------------------------------------------------------------------
#
#                               Methods.
#
#------------------------------------------------------------------------------
    def SetLine(self, lineNumber, register):
        '''Set the value in the specified store line.
        
        @param: lineNumber Store line number to set.
        @param: register Value (of type Register) to store in the specified line.

        @raises: IndexError Indicates that the lineNumber parameter is out of range.
        '''
        if ((self._storeLines != None) and (lineNumber >= 0) and (lineNumber < len(self._storeLines))):
            self._storeLines[lineNumber] = register
        else:
            raise IndexError

    def GetLine(self, lineNumber):
        '''Get the value in the specified store line.

        @param: lineNumber Store line number to get.

        @raises: IndexError Indicates that the lineNumber parameter is out of range.
        
        @return Contents of the specified store line.
        '''
        if ((self._storeLines != None) and (lineNumber >= 0) and (lineNumber < len(self._storeLines))):
            return(self._storeLines[lineNumber])
        else:
            raise IndexError

    def Clear(self):
        '''Clear the contents of the store lines.
        
        This will set all of the store lines to a Register with the value 0.
        '''
        for lineNumber in range(len(self._storeLines)):
            self._storeLines[lineNumber] = Register.Register(0)

    def Print(self):
        '''Print the contents of the store lines along with the disassembly.'''
        print('                 00000000001111111111222222222233')
        print('                 01234567890123456789012345678901')
        for lineNumber in range(len(self._storeLines)):
            line = self.GetLine(lineNumber)
            decimal = line.Value
            if (decimal & 0x80000000):
                decimal -= 2**32
            print('{:02}: {} - {} {:16} ; {}'.format(lineNumber, line.Hex(), line.Binary(), self._instructions.Disassemble(line.Value), decimal))

#------------------------------------------------------------------------------
#
#                               Tests.
#
#------------------------------------------------------------------------------

if (__name__ == '__main__'):
    sl = StoreLines()
    if (sl.Length != 32):
        raise ValueError
    sl = StoreLines(100)
    if (sl.Length != 100):
        raise ValueError
    if (sl.GetLine(0).Value != 0):
        raise ValueError
    sl.SetLine(0, 1)
    if (sl.GetLine(0) != 1):
        raise ValueError
    try:
        sl.SetLine(-1, 0)
    except IndexError:
        pass
    try:
        sl.SetLine(500, 0)
    except IndexError:
        pass
    try:
        sl.GetLine(-1)
    except IndexError:
        pass
    try:
        sl.GetLine(500)
    except IndexError:
        pass
    sl.Clear()
    for lineNumber in range(sl.Length):
        if (sl.GetLine(lineNumber).Value != 0):
            raise ValueError
    print('StoreLines tests pass')
