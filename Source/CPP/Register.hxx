#include <iostream>

/**
 * @brief Object representing a register on the SSEM.
 */
class Register
{
    private:
        /**
         * @brief Value held in the register.
         */
        int32_t _value;

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
};