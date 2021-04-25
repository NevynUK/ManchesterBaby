import board
import displayio
import framebufferio
import rgbmatrix
import time

from Register import Register
from StoreLines import StoreLines

class RGBMatrixDisplayTube:
    '''Implement a way of displaying the store lines from the Manchester Baby using a RGB matrix.'''
    def __init__(self):
        displayio.release_displays()
        # This next call creates the RGB Matrix object itself. It has the given width
        # and height. bit_depth can range from 1 to 6; higher numbers allow more color
        # shades to be displayed, but increase memory usage and slow down your Python
        # code. If you just want to show primary colors plus black and white, use 1.
        # Otherwise, try 3, 4 and 5 to see which effect you like best.
        self.__matrix = rgbmatrix.RGBMatrix(
            width = 32, height = 32, bit_depth = 2,
            # Pin sequence: R0, G0, B0, R1, G1, B1
            rgb_pins=[board.GP4, board.GP21, board.GP5, board.GP6, board.GP20, board.GP7],
            addr_pins=[board.GP8, board.GP19, board.GP9, board.GP18],
            clock_pin = board.GP10, latch_pin = board.GP11, output_enable_pin = board.GP12)
        # Associate the RGB matrix with a Display so that we can use displayio features
        self.__display = framebufferio.FramebufferDisplay(self.__matrix, auto_refresh=False)
        self.__currentBuffer = 0
        self.__buffers = [displayio.Bitmap(self.__display.width, self.__display.height, 2), displayio.Bitmap(self.__display.width, self.__display.height, 2)]
        self.Clear()
        self.__palette = displayio.Palette(2)
        self.__tg1 = displayio.TileGrid(self.__buffers[0], pixel_shader = self.__palette)
        self.__tg2 = displayio.TileGrid(self.__buffers[1], pixel_shader = self.__palette)
        self.__grid1 = displayio.Group(max_size = 3, scale = 1)
        self.__grid1.append(self.__tg1)
        self.__display.show(self.__grid1)
        self.__grid2 = displayio.Group(max_size = 3, scale = 1)
        self.__grid2.append(self.__tg2)        
        self.__palette[0] = 0
        self.__palette[1] = 0x004000
        self.__display.auto_refresh = True

    def Clear(self):
        '''Clear the current buffer.'''
        for index in range(self.__buffers[self.__currentBuffer].width * self.__buffers[self.__currentBuffer].height):
            self.__buffers[self.__currentBuffer][index] = 0

    def Show(self, storeLines):
        if (self.__currentBuffer == 0):
            self.__currentBuffer = 1
            grid = self.__grid2
        else:
            self.__currentBuffer = 0
            grid = self.__grid1
        buffer = self.__buffers[self.__currentBuffer]
        for line in range(storeLines.Length):
            v = storeLines.GetLine(line).Value
            for bit in range(32):
                buffer[(line * 32) + bit] = (v >> bit) & 0x01
        self.__display.show(grid)

if (__name__ == '__main__'):
    m = RGBMatrixDisplayTube()
    sl = StoreLines(32)
    sl.SetLine(0, Register(0xA0A0A0A0))
    sl.SetLine(1, Register(0x50505050))
    while True:
        m.Clear()
        m.Show(sl)
        time.sleep(10)

