#ifndef __CPU_HXX
#define __CPU_HXX

#include "Instructions.hxx"
#include "StoreLines.hxx"

class Cpu
{
    public:
        /**
         * @brief Default constructor is not used in tis application.
         */
        Cpu() = delete;
        /**
         * @brief Construct a new Cpu::Cpu object
         * 
         * @param storeLines Store lines containing the code to be run.
         */
        explicit Cpu(StoreLines &storeLines);

        /**
         * @brief Destroy the Cpu::Cpu object
         */
        ~Cpu();

        /**
         * @brief Reset the CPU.
         */
        void Reset();

        /**
         * @brief Execute a single instruction from the store lines.
         * 
         * @return true if the instruction was executed correctly.
         * @return false if there is an error.
         */
        bool SingleStep();

        /**
         * @brief Is the CPU in the stopped state.
         * 
         * @return true if the CPU is stopped.
         * @return false if the CPU is running.
         */
        bool IsStopped() const noexcept;

        /**
         * @brief Get the PI register.
         * 
         * @return Register const& the current value in the PI register.
         */
        Register const &PI() const noexcept;

        /**
         * @brief Get the CI register.
         * 
         * @return Register const& the current value in the CI register.
         */
        Register const &CI() const noexcept;

        /**
         * @brief Get the accumulator register.
         * 
         * @return Register const& the current value in the accumulator register.
         */
        Register const &Accumulator() const noexcept;

    private:
        /**
         * @brief Storage for the PI register.
         */
        Register _pi;

        /**
         * @brief Storage for the CI register.
         */
        Register _ci;

        /**
         * @brief Storage for the accumulator register.
         */
        Register _accumulator;

        /**
         * @brief Storelines containing the code to be executed.
         */
        StoreLines &_storeLines;

        /**
         * @brief Storage for the Stopped flag.
         */
        bool _stopped;
};

#endif // __CPU_HXX