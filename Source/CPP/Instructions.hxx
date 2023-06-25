#include <vector>
#include <string>

#include "Instruction.hxx"

/**
 * @brief Class to hold the instructions that the SSEM can understand.
 */
class Instructions
{
    private:
        /**
         * @brief Somewhere to hold all of the instructions.
         */
        std::vector<Instruction> _instructions;

    public:
        /**
         * @brief Construct a new Instructions object
         */
        Instructions();
        /**
         * @brief Destroy the Instructions object
         */
        ~Instructions();

        /**
         * @brief Get the opcode for the given mnemonic.
         * 
         * @note If the mnemonic is not found, an invalid_argment exception is thrown.
         * 
         * @param mnemonic Mnemonic to look up.
         * @return Instruction::opcodes_e Opcode for the specified mnemonic.
         */
        Instruction::opcodes_e GetOpcode(const std::string &mnemonic);

        /**
         * @brief Get the mnemonic for the given opcode.
         * 
         * @note If the opcode is not found, an invalid_argment exception is thrown.
         * 
         * @param opcode Opcode to look up.
         * @return const std::string& The preferred mnemonic for the opcode.
         */
        const char *GetMnemonic(Instruction::opcodes_e opcode) const;

        /**
         * @brief Description of the action of the opcode.
         * 
         * @param opcode Opcode to look up.
         * @return const std::string& Description of the preferred mnemonic for the specified opcode.
         */
        const char *GetDescription(Instruction::opcodes_e opcode) const;
};