import board
import busio
import rotaryio
import os
import time

from digitalio import DigitalInOut, Direction, Pull
import adafruit_character_lcd.character_lcd_i2c as character_lcd


class UserInterface:
    '''This object will use the hardware connected to the Raspberry Pi Pico in order to
    determine what the user wants the application to do.'''

#------------------------------------------------------------------------------
#
#                       Class construction.
#
#------------------------------------------------------------------------------
    def __init__(self):
        '''Construct a new user interface object.'''
        self.__titleLine = '  Manchester Baby\n'
        self.__i2c = busio.I2C(board.GP1, board.GP0)
        self.__lcd = character_lcd.Character_LCD_I2C(self.__i2c, 20, 4, usingPCF = True, address = 0x27)
        self.__lcd.clear()
        self.__lcd.message = self.__titleLine + '\n     Pico 2040'

        self.__runLed = DigitalInOut(board.GP27)
        self.__runLed.direction = Direction.OUTPUT
        self.__runLed.value = True

        self.__stopLed = DigitalInOut(board.GP26)
        self.__stopLed.direction = Direction.OUTPUT
        self.__stopLed.value = False

        self.__haltLed = DigitalInOut(board.GP2)
        self.__haltLed.direction = Direction.OUTPUT
        self.__haltLed.value = False

        self.__singleStepLed = DigitalInOut(board.GP28)
        self.__singleStepLed.direction = Direction.OUTPUT
        self.__singleStepLed.value = False

        self.__runStopSwitch = DigitalInOut(board.GP22)
        self.__runStopSwitch.direction = Direction.INPUT
        self.__runStopSwitch.pull = Pull.UP

        self.__singleStepSelectionSwitch = DigitalInOut(board.GP3)
        self.__singleStepSelectionSwitch.direction = Direction.INPUT
        self.__singleStepSelectionSwitch.pull = Pull.UP

        self.__singleStepSwitch = DigitalInOut(board.GP14)
        self.__singleStepSwitch.direction = Direction.INPUT
        self.__singleStepSwitch.pull = Pull.DOWN

        self.__encoderButton = DigitalInOut(board.GP15)
        self.__encoderButton.direction = Direction.INPUT
        self.__encoderButton.pull = Pull.UP

        self.__encoder = rotaryio.IncrementalEncoder(board.GP16, board.GP17)

        try:
            self.__sourceFiles = [f for f in os.listdir('Sources') if f.endswith('.ssem')]
        except:
            self.__sourceFiles = []

        self.__currentFileName = None
        self.__lastEncoderPosition = None

        self.Update()

#------------------------------------------------------------------------------
#
#                           Properties.
#
#------------------------------------------------------------------------------
    @property
    def Stop(self):
        '''Indicate if the user has put the system into the Stopped state.'''
        return(self.__stop)

    @property
    def FileName(self):
        '''Name of the file that has been selected (Sources directory name removed.'''
        return(self.__currentFileName)

    @property
    def SingleStep(self):
        self.Update()
        if ((self.__currentFileName is not None) and not(self.Stop)):
            return(True)
        else:
            return(False)

    @property
    def Filename(self):
        '''File that has been selected, note that this can be None if no file has been selected.'''
        return(self.__currentFileName)

#------------------------------------------------------------------------------
#
#                               Methods.
#
#------------------------------------------------------------------------------
    def Update(self):
        '''Update the sataus of the user interface based upon the hardware switches.'''
        self.__stop = self.__runStopSwitch.value
        self.__singleStepSelected = self.__singleStepSelectionSwitch.value
        if (self.__singleStepSelected):
            self.__singleStepLed.value = True
        else:
            self.__singleStepLed.value = False
        if (self.Stop):
            self.__runLed.value = False
            self.__stopLed.value = True
        else:
            self.__runLed.value = True
            self.__stopLed.value = False
        self.__singleStepMode = self.__singleStepSelectionSwitch.value

    def SetHaltLEDStatus(self, value):
        self.__haltLed.value = value

    def SelectFile(self):
        '''Select a source file from the .ssem files in the Sources directory.'''
        message = self.__titleLine
        if (len(self.__sourceFiles) == 0):
            message += '\nSources directory is\nempty.'
        else:
            message += 'Select file:'

        self.__lcd.clear()
        self.__lcd.message = message
        if (len(self.__sourceFiles) != 0):
            first_file = 0
            if (len(self.__sourceFiles) < 2):
                last_file = len(self.__sourceFiles)
            else:
                last_file = 2
            current_file = 0
            last_position = None
            last_current_file = None
            last_first_file = None
            encoder_pressed = False
            while (self.__encoderButton.value):
                time.sleep(0.01)
                position = self.__encoder.position
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
                            if (last_file > len(self.__sourceFiles)):
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
                        message = self.__titleLine + 'Select file:'
                        for f in self.__sourceFiles[first_file:last_file]:
                            if (current_file == file_number):
                                selector = '*'
                                self.__currentFileName = f
                            else:
                                selector = ' '
                            file_name = selector + f
                            message += '\n' + file_name[:20]
                            file_number += 1
                        self.__lcd.clear()
                        self.__lcd.message = message
                    #
                    last_current_file = current_file
                    last_first_file = first_file
                last_position = position

    def UpdateLCD(self, line1, line2, line3):
        '''Update the contents fo the LCD display.  Note that the first line will always be the title line and is a fixed message.'''
        message = self.__titleLine + line1[:20] + '\n' + line2[:20] + '\n' + line3[:20]
        self.__lcd.clear()
        self.__lcd.message = message