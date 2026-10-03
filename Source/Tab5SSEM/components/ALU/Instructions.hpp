#pragma once

#include <vector>
#include <string>

#include "Instruction.hpp"

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

    static void PopulateLookupTable();
    static Instruction::opcodes_e Opcode(const std::string &mnemonic);
    static const char *Mnemonic(Instruction::opcodes_e opcode);
    static const char *Description(Instruction::opcodes_e opcode);

private:
    /**
     * @brief Somewhere to hold all of the instructions.
     */
    static std::vector<Instruction> _instructions;
};
