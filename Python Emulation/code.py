import time
import board
import busio
from digitalio import DigitalInOut, Direction, Pull
import adafruit_character_lcd.character_lcd_spi as character_lcd

from RGBMatrixDisplayTube import RGBMatrixDisplayTube

from ManchesterBaby import ManchesterBaby

def ProgressUpdate(lineCount):
    message ='  Manchester Baby\n\nExec: hfr989.asm\nLines: {}'.format(lineCount)
    lcd.clear()
    lcd.message = message

lcd_columns = 20
lcd_rows = 4

clk = board.GP2
data = board.GP3
latch = board.GP1

spi = busio.SPI(clk, MOSI = data, MISO = None)
latch = DigitalInOut(latch)
lcd = character_lcd.Character_LCD_SPI(spi, latch, lcd_columns, lcd_rows)

lcd.message = '  Manchester Baby\n\n     Pico 2040'

runLed = DigitalInOut(board.GP27)
runLed.direction = Direction.OUTPUT
runLed.value = True

stopLed = DigitalInOut(board.GP26)
stopLed.direction = Direction.OUTPUT
stopLed.value = False

displayTube = RGBMatrixDisplayTube()
baby = ManchesterBaby()
baby.Assembler('Samples/hfr989.asm')
baby.Print()
start = time.monotonic()
stopLed.value = True
runLed.value = False
message ='  Manchester Baby\n\nExec: hfr989.asm'
lcd.clear()
lcd.message = message
instructionCount = baby.RunProgram(debugging = False, progress = ProgressUpdate, updateDisplayTube  = displayTube.Show)
runLed.value = True
stopLed.value = False
end = time.monotonic()

message ='  Manchester Baby\nExec: hfr989.asm\nLines: {}\nTime: {} sec'.format(instructionCount, end - start)
lcd.clear()
lcd.message = message

baby.Print()

while (True):
    pass
