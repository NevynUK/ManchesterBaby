#include "Register.hpp"

/**
 * @brief Test the register class.
 *
 * @return true Register tests have passed.
 * @return false Register tests have failed.
 */
bool TestRegister()
{
    Register register1;
    Register register2(0x12345678);

    if (register1.GetValue() != 0)
    {
        printf("Register1 is not zero.\n");
        return (false);
    }
    register1.Increment();
    if (register1.GetValue() != 1)
    {
        printf("Register1 did not increment correctly.\n");
        return (false);
    }
    register1.Add(9);
    if (register1.GetValue() != 10)
    {
        printf("Register1 did not add a constant correctly.\n");
        return (false);
    }
    Register addValue(45);
    register1.Add(addValue);
    if (register1.GetValue() != 55)
    {
        printf("Register1 did not add another register correctly.\n");
        return (false);
    }
    register1.Subtract(addValue);
    if (register1.GetValue() != 10)
    {
        printf("Register1 did not subtract another register correctly.\n");
        return (false);
    }

    register1.Negate();
    if (register1.GetValue() != -10)
    {
        printf("Register1 did not negate correctly.\n");
        return (false);
    }

    register1.Negate();
    if (register1.GetValue() != 10)
    {
        printf("Register1 did not revert back to 10.\n");
        return (false);
    }

    string binary = register1.Binary();
    if (binary != "01010000000000000000000000000000")
    {
        printf("Register1 did not convert to binary correctly, result '%s'.\n", binary.c_str());
        return (false);
    }

    if (register1.LineNumber() != 10)
    {
        printf("Register1 did not return the correct line number.\n");
        return (false);
    }

    if (register2.GetValue() != 0x12345678)
    {
        return (false);
    }

    register1.SetValue(0x87654321);
    if (((uint32_t) register1.GetValue()) != 0x87654321)
    {
        return (false);
    }

    // Additional ALU arithmetic tests: positive, negative, zero
    Register regPos(5);
    regPos.Add(3);
    if (regPos.GetValue() != 8)
    {
        printf("ALU Add failed for positive values.\n");
        return (false);
    }
    regPos.Subtract(Register(2));
    if (regPos.GetValue() != 6)
    {
        printf("ALU Subtract failed for positive values.\n");
        return (false);
    }
    regPos.Negate();
    if (regPos.GetValue() != -6)
    {
        printf("ALU Negate failed for positive value.\n");
        return (false);
    }

    Register regNeg(-7);
    regNeg.Add(-3);
    if (regNeg.GetValue() != -10)
    {
        printf("ALU Add failed for negative values.\n");
        return (false);
    }
    regNeg.Subtract(Register(5));
    if (regNeg.GetValue() != -15)
    {
        printf("ALU Subtract failed for negative values.\n");
        return (false);
    }
    regNeg.Negate();
    if (regNeg.GetValue() != 15)
    {
        printf("ALU Negate failed for negative value.\n");
        return (false);
    }

    Register regZero(0);
    regZero.Add(0);
    if (regZero.GetValue() != 0)
    {
        printf("ALU Add failed for zero.\n");
        return (false);
    }
    regZero.Subtract(Register(0));
    if (regZero.GetValue() != 0)
    {
        printf("ALU Subtract failed for zero.\n");
        return (false);
    }
    regZero.Negate();
    if (regZero.GetValue() != 0)
    {
        printf("ALU Negate failed for zero.\n");
        return (false);
    }

    // ALU bitwise operation tests
    Register regBit(0x80000001); // 1000...0001
    int32_t reversed = regBit.ReverseBits();
    if (reversed != 0x80000001)
    {
        printf("ALU ReverseBits failed for 0x80000001.\n");
        return (false);
    }
    Register regBit2(0xF0F0F0F0);
    int32_t reversed2 = regBit2.ReverseBits();
    if (reversed2 != 0x0F0F0F0F)
    {
        printf("ALU ReverseBits failed for 0xF0F0F0F0.\n");
        return (false);
    }
    string binStr = regBit2.Binary();
    if (binStr != "00001111000011110000111100001111")
    {
        printf("ALU Binary failed for 0xF0F0F0F0, got '%s'.\n", binStr.c_str());
        return (false);
    }
    Register regZeroBit(0);
    if (regZeroBit.ReverseBits() != 0)
    {
        printf("ALU ReverseBits failed for zero.\n");
        return (false);
    }
    if (regZeroBit.Binary() != string(32, '0'))
    {
        printf("ALU Binary failed for zero.\n");
        return (false);
    }
    return (true);
}