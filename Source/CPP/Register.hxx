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
        uint32_t _value;

    public:
        /**
         * @brief Construct a new Register object
         * 
         * @param value Value to put in the register, the default value is 0.
         */
        explicit Register(uint32_t value = 0);

        /**
         * @brief Destroy the Register object
         */
        ~Register();

        /**
         * @brief Get the value stored in the register.
         * 
         * @return uint32_t Value i the register.
         */
        uint32_t GetValue() const noexcept;

        /**
         * @brief Set the register value.
         * 
         * @param value Value to be stored in the register.
         */
        void SetValue(uint32_t value) noexcept;
};