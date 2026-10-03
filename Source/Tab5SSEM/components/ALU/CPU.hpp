#pragma once

#include "Instructions.hpp"
#include "StoreLines.hpp"

class Cpu
{
public:
    /**
     * @brief Default constructor is not used in tis application.
     */
    Cpu() = delete;
    explicit Cpu(StoreLines &storeLines);
    ~Cpu();
    void Reset();
    bool SingleStep();
    bool IsStopped() const noexcept;
    Register const &PI() const noexcept;
    Register const &CI() const noexcept;
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
