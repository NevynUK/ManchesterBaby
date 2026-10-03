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
#                           Special methods.
#
#------------------------------------------------------------------------------
    def __eq__(self, other):
        '''Registers are equal if they hold the same value.'''
        if (not isinstance(other, Register)):
            return(NotImplemented)
        return(self.Value == other.Value)

    # Registers are mutable so they cannot be hashed.
    __hash__ = None

    def __repr__(self):
        return('Register({})'.format(self.Hex()))


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
        return('{0:032b}'.format(self.ReverseBits()))

    def ReverseBits(self):
        '''Reverse the bits in the specified value.  This method provides the CPU
        with the ability to translate conventional twos complement into SSEM numbers.

        SSEM numbers are twos complement numbers with the LSB and MSB reversed
        compared to conventional twos complement form.
        
        @returns: Reversed bits version of the value in the register.
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
