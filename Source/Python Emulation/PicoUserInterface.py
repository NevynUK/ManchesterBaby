#!/usr/bin/env python
#
#   Class implementing Raspberry Pi Pico user interface for the Manchester Baby
#
import board
import busio
import rotaryio
import os
import time

from digitalio import DigitalInOut, Direction, Pull
import adafruit_character_lcd.character_lcd_i2c as character_lcd


class PicoUserInterface:
    '''This object will use the hardware connected to the Raspberry Pi Pico in order to
    determine what the user wants the application to do.
    '''

#------------------------------------------------------------------------------
#
#                       Class construction.
#
#------------------------------------------------------------------------------
    def __init__(self):
        '''Construct a new Raspberry Pi Pico user interface object.'''
        self._titleLine = '  Manchester Baby\n'
        self._i2c = busio.I2C(board.GP1, board.GP0)
        self._lcd = character_lcd.Character_LCD_I2C(self._i2c, 20, 4, usingPCF = True, address = 0x27)
        self._lcd.clear()
        self._lcd.message = self._titleLine + '\n     Pico 2040'

        self._runLed = DigitalInOut(board.GP27)
        self._runLed.direction = Direction.OUTPUT
        self._runLed.value = True

        self._stopLed = DigitalInOut(board.GP26)
        self._stopLed.direction = Direction.OUTPUT
        self._stopLed.value = False

        self._haltLed = DigitalInOut(board.GP2)
        self._haltLed.direction = Direction.OUTPUT
        self._haltLed.value = False

        self._singleStepLed = DigitalInOut(board.GP28)
        self._singleStepLed.direction = Direction.OUTPUT
        self._singleStepLed.value = False

        self._runStopSwitch = DigitalInOut(board.GP22)
        self._runStopSwitch.direction = Direction.INPUT
        self._runStopSwitch.pull = Pull.UP

        self._singleStepSelectionSwitch = DigitalInOut(board.GP3)
        self._singleStepSelectionSwitch.direction = Direction.INPUT
        self._singleStepSelectionSwitch.pull = Pull.UP

        self._singleStepSwitch = DigitalInOut(board.GP14)
        self._singleStepSwitch.direction = Direction.INPUT
        self._singleStepSwitch.pull = Pull.DOWN

        self._encoderButton = DigitalInOut(board.GP15)
        self._encoderButton.direction = Direction.INPUT
        self._encoderButton.pull = Pull.UP

        self._encoder = rotaryio.IncrementalEncoder(board.GP16, board.GP17)

        try:
            self._sourceFiles = [f for f in os.listdir('Sources') if f.endswith('.ssem')]
        except:
            self._sourceFiles = []

        self._currentFileName = None
        self._lastEncoderPosition = None

        self.Update()

#------------------------------------------------------------------------------
#
#                           Properties.
#
#------------------------------------------------------------------------------
    @property
    def Stop(self):
        '''Indicate if the user has put the system into the Stopped state.'''
        return(self._stop)

    @property
    def FileName(self):
        '''Name of the file that has been selected (Sources directory name removed.'''
        return(self._currentFileName)

    @property
    def SingleStep(self):
        '''Execute the next instruction.'''
        self.Update()
        if ((self._currentFileName is not None) and not(self.Stop)):
            return(True)
        else:
            return(False)

    @property
    def Filename(self):
        '''File that has been selected, note that this can be None if no file has been selected.'''
        return(self._currentFileName)

#------------------------------------------------------------------------------
#
#                               Methods.
#
#------------------------------------------------------------------------------
    def UpdateDisplayTube(self, storeLines):
        '''Update the display tube with the current contents od the store lines.
        
        @param: storeLines The store lines to be displayed.
        '''
        pass

    def UpdateProgress(self, numberOfLines):
        '''Show the number of the lines executed so far.
        
        @param: numberOfLines The number of lines that have been executed so far.
        '''
        pass

    def DisplayError(self, errorMessage):
        '''Show the error that has just occurred.
        
        @param: errorMessage Message to be displayed.
        '''
        pass

    def Update(self):
        '''Update the status of the user interface based upon the hardware switches.'''
        self._stop = self._runStopSwitch.value
        self._singleStepSelected = self._singleStepSelectionSwitch.value
        if (self._singleStepSelected):
            self._singleStepLed.value = True
        else:
            self._singleStepLed.value = False
        if (self.Stop):
            self._runLed.value = False
            self._stopLed.value = True
        else:
            self._runLed.value = True
            self._stopLed.value = False
        self._singleStepMode = self._singleStepSelectionSwitch.value

    def SetHaltLEDStatus(self, value):
        '''Turn the HALT light on or off.

        @param: value Turn the light on if True, off if False.
        '''
        self._haltLed.value = value

    def SelectFile(self):
        '''Select a source file from the .ssem files in the Sources directory.
        
        On exit, the _fileName variable will hold the name of the selected file.
        '''
        message = self._titleLine
        if (len(self._sourceFiles) == 0):
            message += '\nSources directory is\nempty.'
        else:
            message += 'Select file:'

        self._lcd.clear()
        self._lcd.message = message
        if (len(self._sourceFiles) != 0):
            first_file = 0
            if (len(self._sourceFiles) < 2):
                last_file = len(self._sourceFiles)
            else:
                last_file = 2
            current_file = 0
            last_position = None
            last_current_file = None
            last_first_file = None
            encoder_pressed = False
            while (self._encoderButton.value):
                time.sleep(0.01)
                #
                #   Work out what we should be displaying on the LCD as we only have two lines
                #   of 20 characters each for the file names.
                #
                position = self._encoder.position
                if ((last_position is None) or (position != last_position)):
                    if (last_position is not None):
                        if (position > last_position):
                            first_file += 1
                            last_file += 1
                            current_file += 1
                        else:
                            first_file -= 1
                            last_file -= 1
                            current_file -= 1
                        if (first_file < 0):
                            first_file = 0
                            last_file = 2
                        else:
                            if (last_file > len(self._sourceFiles)):
                                last_file -= 1
                                first_file -= 1
                        #
                        if (current_file >= last_file):
                            current_file = last_file - 1
                        else:
                            if (current_file < 0):
                                current_file = 0
                    #
                    #   Now we can display stuff.
                    #
                    if ((last_first_file != first_file) or (last_current_file != current_file)):
                        file_number = first_file
                        message = self._titleLine + 'Select file:'
                        for f in self._sourceFiles[first_file:last_file]:
                            if (current_file == file_number):
                                selector = '*'
                                self._currentFileName = f
                            else:
                                selector = ' '
                            file_name = selector + f
                            message += '\n' + file_name[:20]
                            file_number += 1
                        self._lcd.clear()
                        self._lcd.message = message
                    #
                    last_current_file = current_file
                    last_first_file = first_file
                last_position = position

    def UpdateLCD(self, line1, line2, line3):
        '''Update the contents fo the LCD display.  Note that the first line will always be the title line and is a fixed message.
        
        @param: line1 Text to display on line 2 of the LCD display.
        @param: line2 Text to display on line 3 of the LCD display
        @param: line3 Text to display on line 4 of the LCD display
        '''
        message = self._titleLine + line1[:20] + '\n' + line2[:20] + '\n' + line3[:20]
        self._lcd.clear()
        self._lcd.message = message