#include "Instructions.hxx"

#include <stdexcept>

/**
 * @brief Construct a new Instructions::Instructions object.
 * 
 * @note The _instructions object is populated with all of the instructions the SSEM supports
 *       including some synonyms.
 */
Instructions::Instructions()
{
    _instructions.push_back(Instruction("JMP", true, Instruction::JMP, "Copy the contents of store line to CI"));
    _instructions.push_back(Instruction("JPR", true, Instruction::JPR, "Add the content of the store line to CI"));
    _instructions.push_back(Instruction("JRP", false, Instruction::JPR, "Add the content of the store line to CI"));
    _instructions.push_back(Instruction("JMR", false, Instruction::JPR, "Add the content of the store line to CI"));
    _instructions.push_back(Instruction("LDN", true, Instruction::LDN, "Copy the content of the store line, negated, into the Accumulator"));
    _instructions.push_back(Instruction("STO", true, Instruction::STO, "Copy the contents of the Accumulator to the store line"));
    _instructions.push_back(Instruction("SUB", true, Instruction::SUB, "Subtract the contents of the store line from the Accumulator"));
    _instructions.push_back(Instruction("---", false, Instruction::INVALID, "Same as function number 4, SUB"));
    _instructions.push_back(Instruction("CMP", true, Instruction::CMP, "Skip the next instruction if the content of the Accumulator is negative"));
    _instructions.push_back(Instruction("SKN", false, Instruction::HALT, "Skip the next instruction if the content of the Accumulator is negative"));
    _instructions.push_back(Instruction("STOP", false, Instruction::HALT, "Light the stop light and halt the machine"));
    _instructions.push_back(Instruction("HALT", true, Instruction::HALT, "Light the stop light and halt the machine"));
    _instructions.push_back(Instruction("STP", false, Instruction::HALT, "Light the stop light and halt the machine"));
}

/**
 * @brief Destroy the Instructions::Instructions object
 */
Instructions::~Instructions()
{
    _instructions.clear();
}

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

    return(Instruction::INVALID);
}

/**
 * @brief Get the mnemonic for the given opcode.
 * 
 * @note If the opcode is not found, an invalid_argment exception is thrown.
 * 
 * @param opcode Opcode to look up.
 * @return const std::string& The preferred mnemonic for the opcode.
 */
const char *Instructions::GetMnemonic(Instruction::opcodes_e opcode) const
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
const char *Instructions::GetDescription(Instruction::opcodes_e opcode) const
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

