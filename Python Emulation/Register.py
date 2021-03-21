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
    def __init__(self, value = 0):
        '''Create a new register and set the value to 0.'''
        self.Value = value

    def __GetValue(self):
        '''Get the register value.'''
        return(self.__value)

    def __SetValue(self, value):
        '''Set the segister to the new value, note that the value must be positive
        and less than 0x100000000'''
        if ((value < 0) or (value > 0xffffffff)):
            raise ValueError
        self.__value = value

    Value = property(__GetValue, __SetValue, None, None)

    def Hex(self):
        '''Return a hexadecimal representation of the register value.'''
        return('{0:#010x}'.format(self.Value))

    def Binary(self):
        '''Return a binary representation of the register without the leadng 0b prefix.'''
        return('{0:#034b}'.format(self.Value)[2:])

#
#   Tests for the Register class.
#
if (__name__ == '__main__'):
    reg = Register()
    if (reg.Value != 0): raise ValueError
    reg.Value = 200
    if (reg.Value != 200): raise ValueError
    try:
        reg.Value = -1
    except ValueError:
        pass
    try:
        reg.Value = 0x100000000
    except ValueError:
        pass
    reg.Value = 0x1f1f
    if (reg.Hex() != '0x00001f1f'): raise ValueError
    if (reg.Binary() != '00000000000000000001111100011111'): raise ValueError
    print('Register tests completed successfully.')
