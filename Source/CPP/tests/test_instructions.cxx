#include <iostream>
#include <stdexcept>

#include "../Instructions.hxx"

using namespace std;

/**
 * @brief Test that a valid mnemonic generates the expected opcode.
 * 
 * @param instructions Object holding the instruction definitions.
 * @param mnemonic Mnemonic to look up.
 * @param expected_opcode Opcode that we expect to be returned.
 * @return true Expected opcode returned.
 * @return false Invalid opcode or an exception was generated.
 */
bool TestMnemonic(Instructions &instructions, const string &mnemonic, int8_t expected_opcode)
{
    bool result = true;

    int8_t code = instructions.GetOpcode(mnemonic);
    if (code != expected_opcode)
    {
        cout << "Invalid opcode for " << mnemonic << ", expected " << expected_opcode << ", received " << code << endl;
        result = false;
    }
    return(result);
}

/**
 * @brief Test that we can translate an opcode into the preferred mnemonic for the instruction.
 * 
 * @param instructions Object holding the instruction definitions.
 * @param expected_mnemonic Mnemonic returned (this should be the preferred mnemonic).
 * @param opcode Opcode to lookup
 * @return true Expected mnemonic returned.
 * @return false Invalid mnemonic or an exception was generated.
 */
bool TestOpcode(const Instructions &instructions, const string &expected_mnemonic, Instruction::opcodes_e opcode)
{
    bool result = true;

    const char *mn = instructions.GetMnemonic(opcode);
    if (mn != expected_mnemonic)
    {
        cout << "Invalid mnemonic for " << opcode << ", expected " << expected_mnemonic << ", received " << mn << endl;
        result = false;
    }
    return(result);
}

/**
 * @brief Test the Instructions object.
 * 
 * @param instructions Object holding the instruction definitions.
 * @return true When all of the tests pass.
 * @return false Fail if any of the tests fail.
 */
bool TestInstructions(Instructions &instructions)
{
    bool result = true;

    //
    //  Make sure the mnemonics are all understood.
    //
    result &= TestMnemonic(instructions, "JMP", Instruction::JMP);
    result &= TestMnemonic(instructions, "JPR", Instruction::JPR);
    result &= TestMnemonic(instructions, "JMR", Instruction::JPR);
    result &= TestMnemonic(instructions, "JRP", Instruction::JPR);
    result &= TestMnemonic(instructions, "LDN", Instruction::LDN);
    result &= TestMnemonic(instructions, "STO", Instruction::STO);
    result &= TestMnemonic(instructions, "SUB", Instruction::SUB);
    result &= TestMnemonic(instructions, "---", Instruction::INVALID);
    result &= TestMnemonic(instructions, "CMP", Instruction::CMP);
    result &= TestMnemonic(instructions, "SKN", Instruction::HALT);
    result &= TestMnemonic(instructions, "STOP", Instruction::HALT);
    result &= TestMnemonic(instructions, "HALT", Instruction::HALT);
    result &= TestMnemonic(instructions, "STP", Instruction::HALT);
    //
    //  Now test for an invalid mnemonic.
    //
    result &= (instructions.GetOpcode("InvalidMnemonic") == Instruction::INVALID);
    
    //
    //  Now get the preferred mnemonics for the opcodes.
    //
    result &= TestOpcode(instructions, "JMP", Instruction::JMP);
    result &= TestOpcode(instructions, "JPR", Instruction::JPR);
    result &= TestOpcode(instructions, "LDN", Instruction::LDN);
    result &= TestOpcode(instructions, "STO", Instruction::STO);
    result &= TestOpcode(instructions, "SUB", Instruction::SUB);
    result &= TestOpcode(instructions, "CMP", Instruction::CMP);
    result &= TestOpcode(instructions, "HALT", Instruction::HALT);
    //
    // Now test for invalid opcode, we should get an exception.
    //
    result &= (instructions.GetMnemonic((Instruction::opcodes_e) 0x08) == nullptr);

    return(result);
}

