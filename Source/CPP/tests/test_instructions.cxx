#include <iostream>
#include <stdexcept>

#include "../Instructions.hxx"

using namespace std;

/**
 * @brief Test that a valid mnemonic generates the expected opcode.
 * 
 * @param mnemonic Mnemonic to look up.
 * @param expected_opcode Opcode that we expect to be returned.
 * @return true Expected opcode returned.
 * @return false Invalid opcode or an exception was generated.
 */
bool TestMnemonic(const string &mnemonic, int8_t expected_opcode)
{
    bool result = true;

    int8_t code = Instructions::GetOpcode(mnemonic);
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
 * @param expected_mnemonic Mnemonic returned (this should be the preferred mnemonic).
 * @param opcode Opcode to lookup
 * @return true Expected mnemonic returned.
 * @return false Invalid mnemonic or an exception was generated.
 */
bool TestOpcode(const string &expected_mnemonic, Instruction::opcodes_e opcode)
{
    bool result = true;

    const char *mn = Instructions::GetMnemonic(opcode);
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
 * @return true When all of the tests pass.
 * @return false Fail if any of the tests fail.
 */
bool TestInstructions()
{
    bool result = true;

    //
    //  Make sure the mnemonics are all understood.
    //
    result &= TestMnemonic("JMP", Instruction::JMP);
    result &= TestMnemonic("JPR", Instruction::JPR);
    result &= TestMnemonic("JMR", Instruction::JPR);
    result &= TestMnemonic("JRP", Instruction::JPR);
    result &= TestMnemonic("LDN", Instruction::LDN);
    result &= TestMnemonic("STO", Instruction::STO);
    result &= TestMnemonic("SUB", Instruction::SUB);
    result &= TestMnemonic("---", Instruction::INVALID);
    result &= TestMnemonic("CMP", Instruction::CMP);
    result &= TestMnemonic("SKN", Instruction::HALT);
    result &= TestMnemonic("STOP", Instruction::HALT);
    result &= TestMnemonic("HALT", Instruction::HALT);
    result &= TestMnemonic("STP", Instruction::HALT);
    //
    //  Now test for an invalid mnemonic.
    //
    result &= (Instructions::GetOpcode("InvalidMnemonic") == Instruction::UNKNOWN);
    
    //
    //  Now get the preferred mnemonics for the opcodes.
    //
    result &= TestOpcode("JMP", Instruction::JMP);
    result &= TestOpcode("JPR", Instruction::JPR);
    result &= TestOpcode("LDN", Instruction::LDN);
    result &= TestOpcode("STO", Instruction::STO);
    result &= TestOpcode("SUB", Instruction::SUB);
    result &= TestOpcode("CMP", Instruction::CMP);
    result &= TestOpcode("HALT", Instruction::HALT);
    //
    // Now test for invalid opcode, we should get an exception.
    //
    result &= (Instructions::GetMnemonic((Instruction::opcodes_e) 0x08) == nullptr);

    return(result);
}

