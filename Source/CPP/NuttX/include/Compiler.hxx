#ifndef __COMPILER_HXX
#define __COMPILER_HXX

#include "StoreLines.hxx"
#include <vector>

/**
 * @brief Implement a simple assembler for the SSEM.
 */
class Compiler
{
    public:
        Compiler() = delete;
        ~Compiler() = delete;

        static StoreLines *Compile(const char *);
        static StoreLines *Compile(const vector<const char *> &);

        static bool IsComment(const char *);
        static bool IsNumber(const char *);
        static bool IsBlank(const char *);
        static bool IsBinary(const char *);

    private:
        enum class Token { Comment, StoreLineNumber, Instruction, LineNumber, Unknown };

        struct TokenisedLine
        {
            uint32_t storeLineNumber;
            uint32_t opcode;
            uint32_t operand;
        };

        static vector<TokenisedLine *> *Tokenise(const vector<const char *> &);
        static uint32_t GetStoreLineNumber(const char *);
        static int32_t GetOperand(const char *);
        static uint32_t GetBinary(const char *);
        static void CleanUp(vector<Compiler::TokenisedLine *> *tokens);
};

#endif // __COMPILER_HXX