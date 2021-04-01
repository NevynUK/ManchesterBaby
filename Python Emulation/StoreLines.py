#!/usr/bin/env python
#
#   Class implementing the store lines for the Manchester Baby.
#
import Register

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
        in the store.  The default number of lines is 32, the same as the SSEM.'''
        if ((size > 0) and (size < MAX_STORE_SIZE)):
            self.__storeLines = [Register.Register() for x in range(size)]
        else:
            raise ValueError

#------------------------------------------------------------------------------
#
#                           Properties.
#
#------------------------------------------------------------------------------
    def __GetLength(self):
        '''Get the number of store lines in this object.'''
        return(len(self.__storeLines))

    Length = property(__GetLength, None, None, None)

#------------------------------------------------------------------------------
#
#                               Methods.
#
#------------------------------------------------------------------------------
    def SetLine(self, lineNumber, register):
        '''Set the value in the specified store line.'''
        if ((self.__storeLines != None) and (lineNumber >= 0) and (lineNumber < len(self.__storeLines))):
            self.__storeLines[lineNumber] = register
        else:
            raise IndexError

    def GetLine(self, lineNumber):
        '''Get the value in the specified store line'''
        if ((self.__storeLines != None) and (lineNumber >= 0) and (lineNumber < len(self.__storeLines))):
            return(self.__storeLines[lineNumber])
        else:
            raise IndexError

    def Clear(self):
        '''Clear the contents of the store lines.
        
        This will set all of the store lines to 0.'''
        for lineNumber in range(len(self.__storeLines)):
            self.__storeLines[lineNumber] = Register.Register(0)

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
