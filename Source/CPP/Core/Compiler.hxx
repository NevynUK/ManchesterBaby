#ifndef __COMPILER_HXX
#define __COMPILER_HXX

#include "StoreLines.hxx"
#include "FileSystemBase.hxx"
#include <vector>

/**
 * @brief Implement a simple assembler for the SSEM.
 */
class Compiler
{
    public:
        Compiler() = delete;
        ~Compiler() = delete;

        StoreLines *Compile(FileSystemBase *, const char *);
        StoreLines *Compile(const vector<const char *> *);

    private:
        struct TokenisedLine
        {
            uint storeLineNumber;
            uint opcode;
            uint lineNumber;
        };

        vector<TokenisedLine *> *Tokenise(const vector<const char *> &);
};

#endif // __COMPILER_HXX