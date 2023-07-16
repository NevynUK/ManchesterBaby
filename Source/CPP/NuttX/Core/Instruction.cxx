#include "Instruction.hxx"

/**
 * @brief Construct a new Instruction::Instruction object
 * 
 * @param mnemonic Mnemonic for this opcode.
 * @param preferred_mnemonic Is the the preferred mnemonic for the opcode?
 * @param opcode Opcode for the instruction.
 * @param description Description of the action of the instruction.
 */
Instruction::Instruction(const char *mnemonic, bool preferred_mnemonic,  opcodes_e opcode, const char *description) : 
    _mnemonic(mnemonic), _opcode(opcode), _description(description), _preferred_mnemonic(preferred_mnemonic)
{
}

/**
 * @brief Destroy the Instruction::Instruction object
 */
Instruction::~Instruction()
{
}

/**
 * @brief Get the mnemonic for this instruction.
 * 
 * @return const std::string& Mnemonic for this instruction.
 */
const char *Instruction::Mnemonic() const noexcept
{
    return(_mnemonic);
}

/**
 * @brief Get the opcode for this instruction.
 * 
 * @return Instruction::opcodes_e Opcode for this instruction.
 */
Instruction::opcodes_e Instruction::Opcode() noexcept
{
    return(_opcode);
}

/**
 * @brief Get the description of the action of this instruction.
 * 
 * @return const std::string& Description of this instructions action.
 */
const char *Instruction::Description() const noexcept
{
    return(_description);
}

/**
 * @brief Is this the preferred mnemonic?
 * 
 * @return true This is the preferred mnemonic.
 * @return false This is not the preferred mnemonic.
 */
bool Instruction::IsPreferredMnemonic() noexcept
{
    return(_preferred_mnemonic);
}
