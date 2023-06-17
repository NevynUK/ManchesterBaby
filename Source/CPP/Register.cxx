#include "Register.hxx"

/**
 * @brief Construct a new Register object
 * 
 * @param value Value to put in the register, the default value is 0.
 */
Register::Register(uint32_t value) : _value(value)
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
 * @return uint32_t Value i the register.
 */
uint32_t Register::GetValue() const noexcept
{
    return(_value);
}

/**
 * @brief Set the register value.
 * 
 * @param value Value to be stored in the register.
 */
void Register::SetValue(uint32_t value) noexcept
{
    _value = value;
}