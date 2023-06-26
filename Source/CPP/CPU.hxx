#include "Instructions.hxx"
#include "StoreLines.hxx"

class Cpu
{
    public:
        Cpu() = delete;
        explicit Cpu(StoreLines &storeLines);
        ~Cpu();

        void Reset();
        bool SingleStep();
        void Stop();
        uint32_t LineNumber(Register const &) const noexcept;
        uint32_t Opcode(Register const &) const noexcept;
        bool IsStopped() const noexcept;
        Register const &PI() const noexcept;
        Register const &CI() const noexcept;
        Register const &Accumulator() const noexcept;

    private:
        Register _pi;
        Register _ci;
        Register _accumulator;
        StoreLines &_storeLines;
        bool _stopped;
};