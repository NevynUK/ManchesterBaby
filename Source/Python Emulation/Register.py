#!/usr/bin/env python
#
#   Register for the Manchester Baby simulation.
#
#   The register class provides methods for holding and displaying data in
#   a register in the Manchester Baby (SSEM).  This class does not interpret
#   the information in a register, it merely stores the data.
#
class Register:
    '''Register in the Manchester Baby.'''

#------------------------------------------------------------------------------
#
#                       Class construction.
#
#------------------------------------------------------------------------------
    def __init__(self, value = 0):
        '''Create a new register and set the value as specified (defaults to 0).'''
        self.Value = value

#------------------------------------------------------------------------------
#
#                           Properties.
#
#------------------------------------------------------------------------------
    @property
    def Value(self):
        '''Register value.'''
        return(self._value)

    @Value.setter
    def Value(self, value):
        '''Register value.'''
        self._value = value & 0xffffffff


#------------------------------------------------------------------------------
#
#                               Methods.
#
#------------------------------------------------------------------------------
    def Hex(self):
        '''Return a hexadecimal representation of the register value.
        
        @returns: Hexadecimal representation of the register value (this is standard twos compliment version of the value).
        '''
        return('{0:#010x}'.format(self.Value))

    def Binary(self):
        '''Return a binary representation of the register without the leading 0b prefix.

        @returns: Binary representation of the register value.  This is the reversed bit version of the register as would be displayed on the Display Tube).
        '''
        v = self.ReverseBits()
        str = '{0:#034b}'.format(v)
        return(str[2:])

    def ReverseBits(self):
        '''Reverse the bits in the specified value.  This method provides the CPU
        with the ability to translate conventional twos complement into SSEM numbers.

        SSEM numbers are twos complement numbers with the LSB and MSB reversed
        compared to conventional twos complement form.
        
        @returns: Revered bits version of the value in the register.
        '''
        result = 0
        value = self.Value
        bitCount = 32
        while (bitCount > 0):
            result <<= 1
            if (value & 1):
                result |= 1
            value >>= 1
            bitCount -= 1
        return(result)

#------------------------------------------------------------------------------
#
#                               Tests.
#
#------------------------------------------------------------------------------
if (__name__ == '__main__'):
    reg = Register()
    if (reg.Value != 0):
        raise ValueError
    reg.Value = 200
    if (reg.Value != 200):
        raise ValueError
    try:
        reg.Value = -1
    except ValueError:
        pass
    try:
        reg.Value = 0x100000000
    except ValueError:
        pass
    reg.Value = 0x1f1f
    if (reg.Hex() != '0x00001f1f'):
        raise ValueError
    if (reg.Binary() != '11111000111110000000000000000000'):
        print (reg.Binary())
        raise ValueError
    if (reg.ReverseBits() != 0xf8f80000): 
        raise ValueError
    print('Register tests completed successfully.')
