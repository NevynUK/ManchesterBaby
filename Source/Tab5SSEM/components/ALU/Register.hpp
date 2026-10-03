#pragma once

#include <string>
#include <iostream>

using namespace std;

/**
 * @brief Object representing a register on the SSEM.
 */
class Register
{
public:
    /**
     * @brief Construct a new Register object
     *
     * @param value Value to put in the register, the default value is 0.
     */
    explicit Register(int32_t value = 0);

    /**
     * @brief Destroy the Register object
     */
    ~Register();

    /**
     * @brief Get the value stored in the register.
     *
     * @return int32_t Value i the register.
     */
    int32_t GetValue() const noexcept;

    /**
     * @brief Set the register value.
     *
     * @param value Value to be stored in the register.
     */
    void SetValue(int32_t value) noexcept;

    /**
     * @brief Get the Line Number from the register.
     *
     * @return int Line number in the register.
     */
    uint LineNumber() const noexcept;

    /**
     * @brief Get the opcode from the register contents.
     *
     * @return uint Opcode in the register.
     */
    uint Opcode() const noexcept;

    /**
     * @brief Get the value of the current register as a binary string.
     */
    string Binary() const;

    /**
     * @brief Disassemble the register contents into a SSEM assembler instruction.
     *
     * @return string SSEM assembler instruction.
     */
    string Disassemble() const;

    /**
     * @brief Disassemble the register contents directly into a character buffer,
     *        avoiding a heap allocation.
     *
     * @param buffer  Destination buffer.
     * @param size    Size of the destination buffer in bytes.
     */
    void Disassemble(char *buffer, size_t size) const;

    /**
     * @brief Add 1 to the register value.
     */
    void Increment() noexcept;

    /**
     * @brief Add the value of another register to this register.
     *
     * @param value Value to add to this register.
     */
    void Add(Register const &value) noexcept;

    /**
     * @brief Add a value to the register.
     *
     * @param value Constant value to add to the register.
     */
    void Add(int32_t value) noexcept;

    /**
     * @brief Subtract the value of another register from this register.
     *
     * @param value Value to subtract from this register.
     */
    void Subtract(Register const &value) noexcept;

    /**
     * @brief Negate the value in the register.
     */
    void Negate() noexcept;

    /**
     * @brief Get the register value as SSEM bits.
     *
     * Note that the SSEM has the first bit at the left of the storage and
     * so we need to reverse the bits when displaying the register contents.
     *
     * @return int32_t SSEM integer representation of the register value.
     */
    int32_t ReverseBits() const noexcept;

private:
    /**
     * @brief Value held in the register.
     */
    int32_t _value;
};
