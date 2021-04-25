#!/usr/bin/env python
#
#   Class implementing RGB Matrix display tube functionality for the Manchester Baby.
#
import board
import displayio
import framebufferio
import rgbmatrix
import time

from Register import Register
from StoreLines import StoreLines

class RGBMatrixDisplayTube:
    '''Implement a way of displaying the store lines from the Manchester Baby using a RGB matrix.'''

#------------------------------------------------------------------------------
#
#                       Class construction.
#
#------------------------------------------------------------------------------
    def __init__(self):
        '''Construct a RGB display object.'''
        displayio.release_displays()
        # This next call creates the RGB Matrix object itself. It has the given width
        # and height. bit_depth can range from 1 to 6; higher numbers allow more color
        # shades to be displayed, but increase memory usage and slow down your Python
        # code. If you just want to show primary colors plus black and white, use 1.
        # Otherwise, try 3, 4 and 5 to see which effect you like best.
        self._matrix = rgbmatrix.RGBMatrix(
            width = 32, height = 32, bit_depth = 2,
            # Pin sequence: R0, G0, B0, R1, G1, B1
            rgb_pins=[board.GP4, board.GP21, board.GP5, board.GP6, board.GP20, board.GP7],
            addr_pins=[board.GP8, board.GP19, board.GP9, board.GP18],
            clock_pin = board.GP10, latch_pin = board.GP11, output_enable_pin = board.GP12)
        # Associate the RGB matrix with a Display so that we can use displayio features
        self._display = framebufferio.FramebufferDisplay(self._matrix, auto_refresh=False)
        self._currentBuffer = 0
        self._buffers = [displayio.Bitmap(self._display.width, self._display.height, 2), displayio.Bitmap(self._display.width, self._display.height, 2)]
        self.Clear()
        self._palette = displayio.Palette(2)
        self._tg1 = displayio.TileGrid(self._buffers[0], pixel_shader = self._palette)
        self._tg2 = displayio.TileGrid(self._buffers[1], pixel_shader = self._palette)
        self._grid1 = displayio.Group(max_size = 3, scale = 1)
        self._grid1.append(self._tg1)
        self._display.show(self._grid1)
        self._grid2 = displayio.Group(max_size = 3, scale = 1)
        self._grid2.append(self._tg2)        
        self._palette[0] = 0
        self._palette[1] = 0x004000
        self._display.auto_refresh = True

#------------------------------------------------------------------------------
#
#                               Methods.
#
#------------------------------------------------------------------------------
    def Clear(self):
        '''Clear the current buffer.'''
        for index in range(self._buffers[self._currentBuffer].width * self._buffers[self._currentBuffer].height):
            self._buffers[self._currentBuffer][index] = 0

    def Show(self, storeLines):
        '''Show the store lines on the RGB matrix.

        @param: storeLines Store lines to be displayed.
        '''
        if (self._currentBuffer == 0):
            self._currentBuffer = 1
            grid = self._grid2
        else:
            self._currentBuffer = 0
            grid = self._grid1
        buffer = self._buffers[self._currentBuffer]
        for line in range(storeLines.Length):
            v = storeLines.GetLine(line).Value
            for bit in range(32):
                buffer[(line * 32) + bit] = (v >> bit) & 0x01
        self._display.show(grid)

#------------------------------------------------------------------------------
#
#                               Tests.
#
#------------------------------------------------------------------------------
if (__name__ == '__main__'):
    m = RGBMatrixDisplayTube()
    sl = StoreLines(32)
    sl.SetLine(0, Register(0xA0A0A0A0))
    sl.SetLine(1, Register(0x50505050))
    while True:
        m.Clear()
        m.Show(sl)
        time.sleep(10)

