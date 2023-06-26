#include "Register.hxx"

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
    return(_value);
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