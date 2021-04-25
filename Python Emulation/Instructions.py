import Register

class Instructions:
    '''Implement the methods and provide constants for the instructions that can
    be held in the SSEM.'''
#------------------------------------------------------------------------------
#
#                       Class construction.
#
#------------------------------------------------------------------------------
    def __init__(self):
        '''Constructor'''

        #
        #   Dictionary containing the instruction set, the associated 'mnemonic' and the 'description' of the instruction.
        #
        #   Instructions are stored in a list with each entry being a dictionary item.  The dictionary item contains the
        #   SSEM opcode for instruction along with information about the instruction:
        #
        #   { opcode: code, instruction: details }
        #
        #   The instruction detail is a dictionary item containing the following items:
        #       Conventional twos complement form of the opcode.
        #       mnemonic
        #       English description of the purpose of the instruction.
        #
        self.__instructions = [
            { 'opcode': 0, 'instruction': { 'twoComplementOpCode': self.OPCODE_JMP, 'mnemonic': 'JMP', 'description': 'Copy the contents of store line to CI' }},
            { 'opcode': 1, 'instruction': { 'twoComplementOpCode': self.OPCODE_JRP, 'mnemonic': 'JPR', 'description': 'Add the content of the store line to CI' }},
            { 'opcode': 1, 'instruction': { 'twoComplementOpCode': self.OPCODE_JRP, 'mnemonic': 'JRP', 'description': 'Add the content of the store line to CI' }},
            { 'opcode': 1, 'instruction': { 'twoComplementOpCode': self.OPCODE_JRP, 'mnemonic': 'JMR', 'description': 'Add the content of the store line to CI' }},
            { 'opcode': 2, 'instruction': { 'twoComplementOpCode': self.OPCODE_LDN, 'mnemonic': 'LDN', 'description': 'Copy the content of the store line, negated, into the Accumulator' }},
            { 'opcode': 3, 'instruction': { 'twoComplementOpCode': self.OPCODE_STO, 'mnemonic': 'STO', 'description': 'Copy the contents of the Accumulator to the store line' }},
            { 'opcode': 4, 'instruction': { 'twoComplementOpCode': self.OPCODE_SUB, 'mnemonic': 'SUB', 'description': 'Subtract the contents of the store line from the Accumulator' }},
            { 'opcode': 5, 'instruction': { 'twoComplementOpCode': self.OPCODE_UNDEFINED, 'mnemonic': '---', 'description': 'Same as function number 4, SUB' }},
            { 'opcode': 6, 'instruction': { 'twoComplementOpCode': self.OPCODE_CMP, 'mnemonic': 'CMP', 'description': 'Skip the next instruction if the content of the Accumulator is negative' }},
            { 'opcode': 6, 'instruction': { 'twoComplementOpCode': self.OPCODE_CMP, 'mnemonic': 'SKN', 'description': 'Skip the next instruction if the content of the Accumulator is negative' }},
            { 'opcode': 7, 'instruction': { 'twoComplementOpCode': self.OPCODE_STOP, 'mnemonic': 'STOP', 'description': 'Light the stop light and halt the machine' }},
            { 'opcode': 7, 'instruction': { 'twoComplementOpCode': self.OPCODE_STOP, 'mnemonic': 'HLT', 'description': 'Light the stop light and halt the machine' }},
            { 'opcode': 7, 'instruction': { 'twoComplementOpCode': self.OPCODE_STOP, 'mnemonic': 'STP', 'description': 'Light the stop light and halt the machine' }}
            ]

#------------------------------------------------------------------------------
#
#                           Properties.
#
#------------------------------------------------------------------------------
    @property
    def OPCODE_JMP(self):
        '''JMP opcode.'''
        return(0)

    @property
    def OPCODE_JRP(self):
        '''JRP opcode.'''
        return(1)

    @property
    def OPCODE_LDN(self):
        '''LDN opcode.'''
        return(2)

    @property
    def OPCODE_STO(self):
        '''STO opcode.'''
        return(3)

    @property
    def OPCODE_SUB(self):
        '''SUB opcode.'''
        return(4)

    @property
    def OPCODE_CMP(self):
        '''CMP opcode.'''
        return(6)

    @property
    def OPCODE_STOP(self):
        '''STOP opcode.'''
        return(7)

    @property
    def OPCODE_UNDEFINED(self):
        '''Undefined opcode.'''
        return(5)

#------------------------------------------------------------------------------
#
#                               Methods.
#
#------------------------------------------------------------------------------
    def Opcode(self, value):
        '''Extract the opcode from a register value.'''
        return((value >> 13) & 0x7)

    def Lookup(self, name):
        '''Lookup an instruction in the list of instructions to get properties.'''
        i = [element for element in self.__instructions if element['instruction']['mnemonic'] == name]
        return(i)

    def Mnemonic(self, value):
        '''Get the mnemonic for the instruction with the given opcode.'''
        if ((value < 0) or (value >= len(self.__instructions))):
            raise ValueError
        i = [element for element in self.__instructions if element['instruction']['twoComplementOpCode'] == value]
        return(i[0]['instruction']['mnemonic'])

    def LineNumber(self, value):
        '''Extract the line number from a register value.'''
        return(value  & 0x1f)

#------------------------------------------------------------------------------
#
#                               Tests.
#
#------------------------------------------------------------------------------
if (__name__ == '__main__'):
    instructions = Instructions()
    register = 0x0000000f
    if (instructions.Opcode(register) != instructions.OPCODE_JMP):
        raise ValueError
    if (instructions.LineNumber(register != 0xf)):
        raise ValueError
    if (instructions.Mnemonic(0) != 'JMP'):
        raise ValueError
    if (instructions.Mnemonic(7) != 'STOP'):
        raise ValueError
    try:
        instructions.Mnemonic(-1)
    except ValueError:
        pass
    try:
        instructions.Mnemonic(8)
    except ValueError:
        pass

    print('Instructions tests pass')
