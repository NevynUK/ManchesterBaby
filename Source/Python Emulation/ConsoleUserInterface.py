#!/usr/bin/env python
#
#   Class implementing a serial (console) user interface for the Manchester Baby (SSEM)
#

#------------------------------------------------------------------------------
#
#                       Class construction.
#
#------------------------------------------------------------------------------
class ConsoleUserInterface:
    '''This object will implement a console user interface for the Manchester Baby (SSEM).'''

    def __init__(self):
        pass

#------------------------------------------------------------------------------
#
#                               Methods.
#
#------------------------------------------------------------------------------
    def UpdateDisplayTube(self, storeLines):
        '''Update the display tube with the current contents od the store lines.
        
        @param: storeLines The store lines to be displayed.
        '''
        storeLines.Print()
        print()

    def UpdateProgress(self, numberOfLines):
        '''Show the number of the lines executed so far.
        
        @param: numberOfLines The number of lines that have been executed so far.
        '''
        print('Executed', numberOfLines, 'lines')

    def DisplayError(self, errorMessage):
        '''Show the error that has just occurred.
        
        @param: errorMessage Message to be displayed.
        '''
        print('Error:', errorMessage)

#------------------------------------------------------------------------------
#
#                               Tests.
#
#------------------------------------------------------------------------------
if (__name__ == '__main__'):
    ui = ConsoleUserInterface()
    ui.UpdateProgress(1000)
    ui.DisplayError('Syntax error')
    print('ConsoleUserInterface tests completed successfully.')
