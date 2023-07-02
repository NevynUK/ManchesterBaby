#include "Instructions.hxx"

#include <stdexcept>

/**
 * @brief Set up the static data in this class.
 */
std::vector<Instruction> Instructions::_instructions
{
    Instruction("JMP", true, Instruction::JMP, "Copy the contents of store line to CI"),
    Instruction("JPR", true, Instruction::JPR, "Add the content of the store line to CI"),
    Instruction("JRP", false, Instruction::JPR, "Add the content of the store line to CI"),
    Instruction("JMR", false, Instruction::JPR, "Add the content of the store line to CI"),
    Instruction("LDN", true, Instruction::LDN, "Copy the content of the store line, negated, into the Accumulator"),
    Instruction("STO", true, Instruction::STO, "Copy the contents of the Accumulator to the store line"),
    Instruction("SUB", true, Instruction::SUB, "Subtract the contents of the store line from the Accumulator"),
    Instruction("---", false, Instruction::INVALID, "Same as function number 4, SUB"),
    Instruction("CMP", true, Instruction::CMP, "Skip the next instruction if the content of the Accumulator is negative"),
    Instruction("SKN", false, Instruction::HALT, "Skip the next instruction if the content of the Accumulator is negative"),
    Instruction("STOP", false, Instruction::HALT, "Light the stop light and halt the machine"),
    Instruction("HALT", true, Instruction::HALT, "Light the stop light and halt the machine"),
    Instruction("STP", false, Instruction::HALT, "Light the stop light and halt the machine")
};

/**
 * @brief Get the opcode for the given mnemonic.
 * 
 * @note If the mnemonic is not found, an invalid_argment exception is thrown.
 * 
 * @param mnemonic Mnemonic to look up.
 * @return Instruction::opcodes_e Opcode for the specified mnemonic.
 */
Instruction::opcodes_e Instructions::GetOpcode(const std::string &mnemonic)
{
    for (auto instruction : _instructions)
    {
        if (instruction.GetMnemonic() == mnemonic)
        {
            return(instruction.GetOpcode());
        }
    }

    return(Instruction::UNKNOWN);
}

/**
 * @brief Get the mnemonic for the given opcode.
 * 
 * @note If the opcode is not found, an invalid_argment exception is thrown.
 * 
 * @param opcode Opcode to look up.
 * @return const std::string& The preferred mnemonic for the opcode.
 */
const char *Instructions::GetMnemonic(Instruction::opcodes_e opcode)
{
    for (auto instruction : _instructions)
    {
        if ((instruction.GetOpcode() == opcode) && (instruction.IsPreferredMnemonic() == true))
        {
            return(instruction.GetMnemonic());
        }
    }

   return(nullptr);
}

/**
 * @brief Description of the action of the opcode.
 * 
 * @param opcode Opcode to look up.
 * @return const std::string& Description of the preferred mnemonic for the specified opcode.
 */
const char *Instructions::GetDescription(Instruction::opcodes_e opcode)
{
    for (auto instruction : _instructions)
    {
        if ((instruction.GetOpcode() == opcode) && (instruction.IsPreferredMnemonic() == true))
        {
            return(instruction.GetDescription());
        }
    }

    return(nullptr);
}

