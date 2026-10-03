#include <inttypes.h>

#include "Instructions.hpp"
#include "Register.hpp"
#include "Constants.hpp"

using namespace std;

/**
 * @brief Construct a new Register object
 *
 * @param value Value to put in the register, the default value is 0.
 */
Register::Register(int32_t value) : _value(value)
{
}

/**
 * @brief Destroy the Register object
 */
Register::~Register()
{
}

/**
 * @brief Get the value stored in the register.
 *
 * @return int32_t Value i the register.
 */
int32_t Register::GetValue() const noexcept
{
    return (_value);
}

/**
 * @brief Set the register value.
 *
 * @param value Value to be stored in the register.
 */
void Register::SetValue(int32_t value) noexcept
{
    _value = value;
}

/**
 * @brief Increment this register by 1.
 */
void Register::Increment() noexcept
{
    Add(1);
}

/**
 * @brief Add the specified register value to this register value.
 *
 * @param value Register holding the value to add.
 */
void Register::Add(Register const &value) noexcept
{
    Add(value.GetValue());
}

/**
 * @brief Add the specified constant to this register.
 *
 * @param value Constant value to add.
 */
void Register::Add(int32_t value) noexcept
{
    _value += value;
}

/**
 * @brief Subtract the specified register from this register.
 *
 * @param value Register to subtract from this register.
 */
void Register::Subtract(Register const &value) noexcept
{
    _value -= value.GetValue();
}

/**
 * @brief Negate the value held in this register.
 */
void Register::Negate() noexcept
{
    _value *= -1;
}

/**
 * @brief Get the value of the current register as a binary string.
 *
 * @return string String containing the binary representation of the register value.
 */
string Register::Binary() const
{
    string binary(32, '0');
    int32_t mask = 1;
    int32_t value = ReverseBits();

    for (int bitcount = 31; bitcount >= 0; --bitcount)
    {
        binary[bitcount] = (value & mask) ? '1' : '0';
        mask <<= 1;
    }

    return (binary);
}

/**
 * @brief Reverse the bits in the register.
 *
 * @return int32_t _value with the bits reversed.
 */
int32_t Register::ReverseBits() const noexcept
{
    int32_t result = 0;
    int32_t value = _value;

    for (int index = 0; index < 32; index++)
    {
        result <<= 1;
        if (value & 1)
        {
            result |= 1;
        }
        value >>= 1;
    }

    return (result);
}

/**
 * @brief Extract the line number from the store line.
 *
 * @param storeLine Store line to extract the line number from.
 * @return uint32_t Line number component of the register.
 */
uint Register::LineNumber() const noexcept
{
    return (_value & Constants::LINE_NUMBER_MASK);
}

/**
 * @brief Extract the opcode from the register.
 *
 * @param storeLine to extract the opcode from.
 * @return uint32_t opcode stored in the store line.
 */
uint Register::Opcode() const noexcept
{
    return ((_value >> Constants::OPCODE_SHIFT) & Constants::OPCODE_MASK);
}

/**
 * @brief Disassemble the register contents directly into a character buffer,
 *        avoiding a heap allocation.
 *
 * @param buffer  Destination buffer.
 * @param size    Size of the destination buffer in bytes.
 */
void Register::Disassemble(char *buffer, size_t size) const
{
    const uint lineNumber = LineNumber();
    const Instruction::opcodes_e opcode = static_cast<Instruction::opcodes_e>(Opcode());

    if ((opcode == Instruction::CMP) || (opcode == Instruction::HALT))
    {
        snprintf(buffer, size, "%s", Instructions::Mnemonic(opcode));
    }
    else
    {
        snprintf(buffer, size, "%s %" PRIu32, Instructions::Mnemonic(opcode), (uint32_t) lineNumber);
    }
}

/**
 * @brief Disassemble the register contents into a SSEM assembler instruction.
 *
 * @return string Disassembled SSEM assembler instruction.
 */
string Register::Disassemble() const
{
    char buffer[Constants::LINE_LENGTH];
    Disassemble(buffer, sizeof(buffer));
    return (string(buffer));
}