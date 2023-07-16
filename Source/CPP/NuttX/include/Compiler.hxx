#ifndef __COMPILER_HXX
#define __COMPILER_HXX

#include "StoreLines.hxx"
#include <vector>

/**
 * @brief Implement a simple compiler for the SSEM.
 */
class Compiler
{
    public:
        Compiler() = delete;
        ~Compiler() = delete;

        /**
         * @brief Read a file from the file system and compile the contents returning the
         *        compiled program as store lines.
         * 
         * @param filename Name of the file to be read.
         * @return StoreLines* StoreLines containing the compiled program.
         */
        static StoreLines *Compile(const char *);

        /**
         * @brief Compile the program contained in the vector of const char * pointers.
         * 
         * @param program Vector containing char * lines of text to be compiled.
         * @return StoreLines* StoreLines containing the compiled program.
         */
        static StoreLines *Compile(const vector<const char *> &);

        /**
         * @brief Is the text a comment?
         * 
         * @param line Text to be checked.
         * @return true If the text represents a comment.
         * @return false If the text is not a comment.
         */
        static bool IsComment(const char *);

        /**
         * @brief Can the text be a number (all characters are digits possibly starting with + or -).
         * 
         * @param line Text to be checked.
         * @return true If the text could be a number.
         * @return false If the text contains non-digit characters.
         */
        static bool IsNumber(const char *);


        /**
         * @brief Is the text blank?
         * 
         * @param line Text to be checked.
         * @return true If the line is blank.
         * @return false If the line is not blank.
         */
        static bool IsBlank(const char *);

        /**
         * @brief Is the text composed solely of 0s and 1s and less than 32 bits in length?
         * 
         * @param line Text to be checked.
         * @return true If the text is binary.
         * @return false If the text is not binary.
         */
        static bool IsBinary(const char *);

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

        /**
         * @brief Convert the lines of text into a number of tokenised lines.
         * 
         * Note that the elements of the line are separated by one or more spaces.
         * 
         * @param lines Vector containing the lines of text
         * @return vector<Compiler::TokenisedLine *>* Tokenised lines.
         */
        static vector<TokenisedLine *> *Tokenise(const vector<const char *> &);

        /**
         * @brief Convert the first part of the line into the store line number.
         * 
         * @param line First part of the line being compiled.
         * @return uint32_t StoreLine number for this line.
         * @throws runtime_error If the line number is invalid.
         */
        static uint32_t GetStoreLineNumber(const char *);

        /**
         * @brief Convert the text into an operand.
         * 
         * @param line Part of the line to be converted
         * @return uint32_t Opcode representing the text.
         * @throws runtime_error If the opcode is invalid or the value is out of range.
         */
        static int32_t GetOperand(const char *);

        /**
         * @brief Convert the text from binary into a number.
         * 
         * @param line Partial line to be converted.
         * @return uint32_t Number represented by the value.
         * @throws runtime_error If the value is not a binary number
         */
        static uint32_t GetBinary(const char *);

        /**
         * @brief Delete the tokens and release any memory associated with them.
         * 
         * @param tokens Vector of tokens to be deleted.
         */
        static void CleanUp(vector<Compiler::TokenisedLine *> *tokens);
};

#endif // __COMPILER_HXX