#pragma once

#include "StoreLines.hpp"
#include <string>
#include <vector>

/**
 * @brief Implement a simple compiler for the SSEM.
 */
class Compiler
{
public:
    Compiler() = delete;
    ~Compiler() = delete;

    static StoreLines Compile(const vector<string> &);
    static bool IsComment(const std::string &line);
    static bool IsNumber(const std::string &line);
    static bool IsBlank(const std::string &line);
    static bool IsBinary(const std::string &line);

private:
    /**
     * @brief Structure to hold the tokenised line.
     */
    struct TokenisedLine
    {
        /**
         * @brief Store line number that this line will be stored in.
         */
        uint32_t storeLineNumber;

        /**
         * @brief Opcode for this line.
         */
        uint32_t opcode;

        /**
         * @brief Operand to be used by the opcode.
         */
        uint32_t operand;
    };

    static vector<TokenisedLine *> *Tokenise(const vector<string> &);
    static uint32_t GetStoreLineNumber(const std::string &line);
    static int32_t GetOperand(const std::string &line);
    static uint32_t GetBinary(const std::string &line);
    static void CleanUp(vector<Compiler::TokenisedLine *> *tokens);
};
