#ifndef __INSTRUCTION_HXX
#define __INSTRUCTION_HXX

#include <stdlib.h>

class Instruction
{
    public:
        enum opcodes_e : uint8_t
        {
            JMP = 0x00,
            JPR = 0x01,
            LDN = 0x02,
            STO = 0x03,
            SUB = 0x04,
            INVALID = 0x05,
            CMP = 0x06,
            HALT = 0x07,
            //
            //  Not really opcodes but special tokens used in the compiler.
            //
            NUM = 0x08,
            BIN = 0x09,
            UNKNOWN = 0xff
        };        

    private:
        const char *_mnemonic;
        opcodes_e _opcode;
        const char *_description;
        bool _preferred_mnemonic;

    public:
        /**
         * @brief Default constructor - deleted.
         */
        Instruction() = delete;

        /**
         * @brief Construct a new Instruction::Instruction object
         * 
         * @param mnemonic Mnemonic for this opcode.
         * @param preferred_mnemonic Is the the preferred mnemonic for the opcode/
         * @param opcode Opcode for the instruction.
         * @param description Description of the action of the instruction.
         */
        Instruction(const char *mnemonic, bool preferred_mnemonic, opcodes_e opcode, const char *description);

        /**
         * @brief Destroy the Instruction::Instruction object
         */
        ~Instruction();

        /**
         * @brief Get the mnemonic for this instruction.
         * 
         * @return const std::string& Mnemonic for this instruction.
         */
        const char *Mnemonic() const noexcept;

        /**
         * @brief Get the opcode for this instruction.
         * 
         * @return Instruction::opcodes_e Opcode for this instruction.
         */
        Instruction::opcodes_e Opcode() noexcept;

        /**
         * @brief Get the description of the action of this instruction.
         * 
         * @return const std::string& Description of this instructions action.
         */
        const char *Description() const noexcept;

        /**
         * @brief Is this the preferred mnemonic?
         * 
         * @return true This is the preferred mnemonic.
         * @return false This is not the preferred mnemonic.
         */
        bool IsPreferredMnemonic() noexcept;
};

#endif // __INSTRUCTION_HXX