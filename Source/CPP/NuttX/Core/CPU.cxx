#include "CPU.hxx"

/**
 * @brief Construct a new Cpu::Cpu object
 * 
 * @param storeLines Store lines containing the code to be run.
 */
Cpu::Cpu(StoreLines &storeLines) : _pi(0), _ci(0),  _accumulator(0), _storeLines(storeLines), _stopped(true)
{
}

/**
 * @brief Destroy the Cpu::Cpu object
 */
Cpu::~Cpu()
{
}

/**
 * @brief Is the CPU in the stopped state.
 * 
 * @return true if the CPU is stopped.
 * @return false if the CPU is running.
 */
bool Cpu::IsStopped() const noexcept
{
    return(_stopped);
}

/**
 * @brief Get the PI register.
 * 
 * @return Register const& the current value in the PI register.
 */
Register const &Cpu::PI() const noexcept
{
    return(_pi);
}

/**
 * @brief Get the CI register.
 * 
 * @return Register const& the current value in the CI register.
 */
Register const &Cpu::CI() const noexcept
{
    return(_ci);
}

/**
 * @brief Get the accumulator register.
 * 
 * @return Register const& the current value in the accumulator register.
 */
Register const &Cpu::Accumulator() const noexcept
{
    return(_accumulator);
}

/**
 * @brief Reset the CPU.
 */
void Cpu::Reset()
{
    _accumulator.SetValue(0);
    _ci.SetValue(0);
    _pi.SetValue(0);
    _stopped = false;
}

/**
 * @brief Execute a single instruction from the store lines.
 * 
 * @return true if the instruction was executed correctly.
 * @return false if there is an error.
 */
bool Cpu::SingleStep()
{
    if (_stopped)
    {
        return(false);
    }
    //
    //  First step is to increment the CI register.
    //
    uint32_t ci = _ci.GetValue();
    if (ci == 0xffffffff)
    {
        ci = 0;
    }
    else
    {
        ci++;
    }
    _ci.SetValue(ci);

    //
    //  Get the store line from the store lines and decode the instruction.
    //
    uint32_t lineNumber = _ci.LineNumber();
    if (lineNumber >= _storeLines.Size())
    {
        _stopped = true;
        return(false);
    }
    _pi = _storeLines[lineNumber];
    uint32_t opcode = _pi.Opcode();
    lineNumber = _pi.LineNumber();

    //
    //  Finally, we can execute the instruction.
    //
    switch (opcode)
    {
        case Instruction::JMP:
            _ci.SetValue(_storeLines[lineNumber].GetValue());
            break;
        case Instruction::JPR:
            _ci.Add(_storeLines[lineNumber]);
            break;
        case Instruction::LDN:
            _accumulator.SetValue(_storeLines[lineNumber].GetValue());
            _accumulator.Negate();
            break;
        case Instruction::STO:
            _storeLines[lineNumber].SetValue(_accumulator.GetValue());
            break;
        case Instruction::SUB:
            _accumulator.Subtract(_storeLines[lineNumber]);
            break;
        case Instruction::CMP:
            if (_accumulator.GetValue() & 0x80000000)
            {
                _ci.Increment();
            }
            break;
        case Instruction::HALT:
            _stopped = true;
            break;
    }

    return(true);
}