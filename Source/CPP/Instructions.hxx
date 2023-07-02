#ifndef __INSTRUCTIONS_HXX
#define __INSTRUCTIONS_HXX

#include <vector>
#include <string>

#include "Instruction.hxx"

/**
 * @brief Class to hold the instructions that the SSEM can understand.
 */
class Instructions
{
    public:
        /**
         * @brief Construct a new Instructions object
         */
        Instructions() = delete;
        /**
         * @brief Destroy the Instructions object
         */
        ~Instructions() = delete;

        /**
         * @brief Populate the lookup table.
         * 
         * Note: This is necessary as NuttX does not have the ability to initialise a vector
         *       with a list of objects in the vector initialisation.
         * 
         */
        static void PopulateLookupTable();

        /**
         * @brief Get the opcode for the given mnemonic.
         * 
         * @note If the mnemonic is not found, an invalid_argment exception is thrown.
         * 
         * @param mnemonic Mnemonic to look up.
         * @return Instruction::opcodes_e Opcode for the specified mnemonic.
         */
        static Instruction::opcodes_e GetOpcode(const std::string &mnemonic);

        /**
         * @brief Get the mnemonic for the given opcode.
         * 
         * @note If the opcode is not found, an invalid_argment exception is thrown.
         * 
         * @param opcode Opcode to look up.
         * @return const std::string& The preferred mnemonic for the opcode.
         */
        static const char *GetMnemonic(Instruction::opcodes_e opcode);

        /**
         * @brief Description of the action of the opcode.
         * 
         * @param opcode Opcode to look up.
         * @return const std::string& Description of the preferred mnemonic for the specified opcode.
         */
        static const char *GetDescription(Instruction::opcodes_e opcode);

    private:
        /**
         * @brief Somewhere to hold all of the instructions.
         */
        static std::vector<Instruction> _instructions;
};

#endif // __INSTRUCTIONS_HXX