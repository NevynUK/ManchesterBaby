#pragma once

#include <stdint.h>
#include <stdlib.h>

class Instruction
{
public:
    enum opcodes_e : uint8_t {
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

    Instruction(const char *mnemonic, bool preferred_mnemonic, opcodes_e opcode, const char *description);
    ~Instruction();
    const char *Mnemonic() const noexcept;
    Instruction::opcodes_e Opcode() noexcept;
    const char *Description() const noexcept;
    bool IsPreferredMnemonic() noexcept;
};
