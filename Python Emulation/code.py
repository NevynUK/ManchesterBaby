#
#   System and external libraries.
#
import time
import board
import busio
from digitalio import DigitalInOut, Direction, Pull

#
#   Project libraries.
#
from RGBMatrixDisplayTube import RGBMatrixDisplayTube
from ManchesterBaby import ManchesterBaby
from UserInterface import UserInterface

displayTube = RGBMatrixDisplayTube()
ui = None
exec_message = None

def ProgressUpdate(lineCount):
    ui.UpdateLCD('', exec_message, 'Lines: {}'.format(lineCount))

ui = UserInterface()
time.sleep(2.0)
ui.SelectFile()

if (ui.FileName is not None):
    baby = ManchesterBaby()
    baby.Assembler('Sources/' + ui.FileName)
    baby.Print()
    exec_message = 'Exec: ' + ui.FileName
    ui.UpdateLCD('', exec_message, '')
    start = time.monotonic()
    instructionCount = baby.RunProgram(debugging = False, progress = ProgressUpdate, updateDisplayTube  = displayTube.Show)
    end = time.monotonic()
    lines_message = 'Lines: {}'.format(instructionCount)
    time_message = 'Time: {}'.format(end - start)
    ui.UpdateLCD(exec_message, lines_message, time_message)
    baby.Print()

while (True):
    pass