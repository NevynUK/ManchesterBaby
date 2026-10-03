#!/usr/bin/env python
#
#   Class implementing the instruction set for the Manchester Baby.
#
class Instructions:
    '''Implement the methods and provide constants for the instructions that can
    be held in the SSEM.'''
    OPCODE_JMP = 0
    OPCODE_JRP = 1
    OPCODE_LDN = 2
    OPCODE_STO = 3
    OPCODE_SUB = 4
    OPCODE_UNDEFINED = 5    # Behaves in the same way as SUB.
    OPCODE_CMP = 6
    OPCODE_STOP = 7

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
        #       mnemonic
        #       English description of the purpose of the instruction.
        #
        self._instructions = [
            { 'opcode': self.OPCODE_JMP, 'instruction': { 'mnemonic': 'JMP', 'description': 'Copy the contents of store line to CI' }},
            { 'opcode': self.OPCODE_JRP, 'instruction': { 'mnemonic': 'JRP', 'description': 'Add the content of the store line to CI' }},
            { 'opcode': self.OPCODE_JRP, 'instruction': { 'mnemonic': 'JPR', 'description': 'Add the content of the store line to CI' }},
            { 'opcode': self.OPCODE_JRP, 'instruction': { 'mnemonic': 'JMR', 'description': 'Add the content of the store line to CI' }},
            { 'opcode': self.OPCODE_LDN, 'instruction': { 'mnemonic': 'LDN', 'description': 'Copy the content of the store line, negated, into the Accumulator' }},
            { 'opcode': self.OPCODE_STO, 'instruction': { 'mnemonic': 'STO', 'description': 'Copy the contents of the Accumulator to the store line' }},
            { 'opcode': self.OPCODE_SUB, 'instruction': { 'mnemonic': 'SUB', 'description': 'Subtract the contents of the store line from the Accumulator' }},
            { 'opcode': self.OPCODE_UNDEFINED, 'instruction': { 'mnemonic': '---', 'description': 'Same as function number 4, SUB' }},
            { 'opcode': self.OPCODE_CMP, 'instruction': { 'mnemonic': 'CMP', 'description': 'Skip the next instruction if the content of the Accumulator is negative' }},
            { 'opcode': self.OPCODE_CMP, 'instruction': { 'mnemonic': 'SKN', 'description': 'Skip the next instruction if the content of the Accumulator is negative' }},
            { 'opcode': self.OPCODE_STOP, 'instruction': { 'mnemonic': 'STOP', 'description': 'Light the stop light and halt the machine' }},
            { 'opcode': self.OPCODE_STOP, 'instruction': { 'mnemonic': 'HLT', 'description': 'Light the stop light and halt the machine' }},
            { 'opcode': self.OPCODE_STOP, 'instruction': { 'mnemonic': 'STP', 'description': 'Light the stop light and halt the machine' }}
            ]

#------------------------------------------------------------------------------
#
#                           Properties.
#
#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
#
#                               Opcodes.
#
#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
#
#                               Methods.
#
#------------------------------------------------------------------------------
    @staticmethod
    def Opcode(value):
        '''Extract the opcode from a register value.

        @param: value Value stored in the Register.

        @returns: Opcode element of the register value.
        '''
        return((value >> 13) & 0x7)

    def Lookup(self, name):
        '''Lookup an instruction in the list of instructions to get properties.
        
        @param: name Lookup the instruction properties by name.
        
        @returns: List of the matching instruction table entries (empty if there is no such instruction).
        '''
        i = [element for element in self._instructions if element['instruction']['mnemonic'] == name]
        return(i)

    def Mnemonic(self, opcode):
        '''Get the mnemonic for the instruction with the given opcode.

        @param: opcode Opcode to look up and be decoded.
        
        @returns: Printable mnemonic for the opcode.
        '''
        if ((opcode < 0) or (opcode > self.OPCODE_STOP)):
            raise ValueError('Invalid opcode: {}'.format(opcode))
        i = [element for element in self._instructions if element['opcode'] == opcode]
        return(i[0]['instruction']['mnemonic'])

    @staticmethod
    def LineNumber(value):
        '''Extract the line number from a register value.
        
        @param: value Register value to be decoded.

        @returns: Line number from the register value.
        '''
        return(value  & 0x1f)

    def Disassemble(self, value):
        '''Disassemble the instruction in the specified register.
        
        @param: value Register value holding the line to disassemble.

        @returns: Printable version of the register being disassembled.
        '''
        lineNumber = self.LineNumber(value)
        opcode = self.Opcode(value)
        mnemonic = self.Mnemonic(opcode)
        if ((mnemonic == 'STOP') or (mnemonic == 'CMP')):
            instruction = mnemonic
        else:
            instruction = '{} {}'.format(mnemonic, lineNumber)
        return(instruction)
