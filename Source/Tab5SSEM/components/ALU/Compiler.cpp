#include <cstdlib>
#include <stdexcept>
#include <string>
#include <vector>

#include "Instruction.hpp"
#include "Instructions.hpp"
#include "Compiler.hpp"
#include "Constants.hpp"

using namespace std;

/**
 * @brief Delete the tokens and release any memory associated with them.
 *
 * @param tokens Vector of tokens to be deleted.
 */
void Compiler::CleanUp(vector<Compiler::TokenisedLine *> *tokens)
{
    for (auto token: *tokens)
    {
        delete token;
    }
    tokens->clear();
    delete tokens;
}

/**
 * @brief Compile the program contained in the vector of strings.
 *
 * @param program Vector containing lines of text to be compiled.
 * @return StoreLines StoreLines containing the compiled program.
 */
StoreLines Compiler::Compile(const vector<string> &program)
{
    if (program.size() == 0)
    {
        throw runtime_error("Program is null");
    }

    vector<Compiler::TokenisedLine *> *tokens = Tokenise(program);
    StoreLines storeLines;

    if (tokens->size() > 0)
    {
        try
        {
            for (auto token: *tokens)
            {
                uint32_t value = 0;
                if ((token->opcode == Instruction::BIN) || (token->opcode == Instruction::NUM))
                {
                    value = token->operand;
                }
                else
                {
                    value = token->opcode << Constants::OPCODE_SHIFT;
                    value |= (token->operand & Constants::LINE_NUMBER_MASK);
                }
                storeLines[token->storeLineNumber].SetValue(value);
            }
        }
        catch (const std::exception &e)
        {
            CleanUp(tokens);
            throw e;
        }
    }
    CleanUp(tokens);

    return (storeLines);
}

/**
 * @brief Convert the lines of text into a number of tokenised lines.
 *
 * Note that the elements of the line are separated by one or more spaces.
 *
 * @param lines Vector containing the lines of text
 * @return vector<Compiler::TokenisedLine *>* Tokenised lines.
 */
vector<Compiler::TokenisedLine *> *Compiler::Tokenise(const vector<string> &lines)
{
    vector<Compiler::TokenisedLine *> *result = new vector<Compiler::TokenisedLine *>();

    try
    {
        for (const string &line: lines)
        {
            if (IsBlank(line) || IsComment(line))
            {
                continue;
            }

            // Split the line into space-delimited tokens, stopping at a comment marker
            vector<string> tokens;
            size_t position = 0;

            while (position < line.size())
            {
                while ((position < line.size()) && (line[position] == ' '))
                {
                    position++;
                }
                if (position >= line.size())
                {
                    break;
                }
                const size_t start = position;
                while ((position < line.size()) && (line[position] != ' '))
                {
                    position++;
                }
                const string token = line.substr(start, position - start);
                if (IsComment(token))
                {
                    break;
                }
                tokens.push_back(token);
            }

            if (tokens.empty())
            {
                continue;
            }

            uint32_t storeLineNumber = GetStoreLineNumber(tokens[0]);
            if (tokens.size() > 1)
            {
                Instruction::opcodes_e opcode = Instructions::Opcode(tokens[1]);
                uint32_t operand = 0;

                switch (opcode)
                {
                    case Instruction::JMP:
                    case Instruction::JPR:
                    case Instruction::LDN:
                    case Instruction::STO:
                    case Instruction::SUB:
                    case Instruction::NUM:
                        operand = GetOperand(tokens.size() > 2 ? tokens[2] : "");
                        break;
                    case Instruction::BIN:
                        operand = GetBinary(tokens.size() > 2 ? tokens[2] : "");
                        break;
                    case Instruction::UNKNOWN:
                        throw runtime_error("Unknown opcode: " + tokens[1]);
                    default:
                        if (tokens.size() > 3)
                        {
                            if (!IsComment(tokens[3]))
                            {
                                throw runtime_error("Unexpected operand");
                            }
                        }
                        break;
                }

                if (tokens.size() > 3)
                {
                    if (!IsComment(tokens[3]))
                    {
                        throw runtime_error("Unexpected text");
                    }
                }

                TokenisedLine *tokenisedLine = new TokenisedLine();
                tokenisedLine->storeLineNumber = storeLineNumber;
                tokenisedLine->opcode = opcode;
                tokenisedLine->operand = operand;
                result->push_back(tokenisedLine);
            }
        }
    }
    catch (const std::exception &e)
    {
        CleanUp(result);
        throw e;
    }

    return (result);
}

/**
 * @brief Convert the first part of the line into the store line number.
 *
 * @param line First part of the line being compiled.
 * @return uint32_t StoreLine number for this line.
 * @throws runtime_error If the line number is invalid.
 */
uint32_t Compiler::GetStoreLineNumber(const string &line)
{
    if (line.empty() || line.back() != ':')
    {
        throw runtime_error("Invalid store line number");
    }

    const string numberPart = line.substr(0, line.size() - 1);
    if (!IsNumber(numberPart))
    {
        throw runtime_error("Invalid store line number");
    }

    long number = strtol(numberPart.c_str(), nullptr, 10);
    if ((number < 0) || (static_cast<unsigned long>(number) > UINT32_MAX))
    {
        throw runtime_error("Invalid store line number");
    }

    return (static_cast<uint32_t>(number));
}

/**
 * @brief Convert the text into an operand.
 *
 * @param line Part of the line to be converted
 * @return uint32_t Opcode representing the text.
 * @throws runtime_error If the opcode is invalid or the value is out of range.
 */
int32_t Compiler::GetOperand(const string &line)
{
    if (line.empty() || !IsNumber(line))
    {
        throw runtime_error("Invalid operand");
    }

    long number = strtol(line.c_str(), nullptr, 10);
    if ((number > INT32_MAX) || (number < INT32_MIN))
    {
        throw runtime_error("Operand out of range");
    }

    return (static_cast<int32_t>(number));
}

/**
 * @brief Convert the text from binary into a number.
 *
 * @param line Partial line to be converted.
 * @return uint32_t Number represented by the value.
 * @throws runtime_error If the value is not a binary number
 */
uint32_t Compiler::GetBinary(const string &line)
{
    if (line.empty() || !IsBinary(line))
    {
        throw runtime_error("Invalid binary number");
    }

    return (static_cast<uint32_t>(strtoul(line.c_str(), nullptr, 2) & 0xffffffff));
}

/**
 * @brief Is the text a comment?
 *
 * @param line Text to be checked.
 * @return true If the text represents a comment.
 * @return false If the text is not a comment.
 */
bool Compiler::IsComment(const string &line)
{
    return (!line.empty() && ((line[0] == ';') || (line.size() >= 2 && line.compare(0, 2, "--") == 0)));
}

/**
 * @brief Can the text be a number (all characters are digits possibly starting with + or -).
 *
 * @param line Text to be checked.
 * @return true If the text could be a number.
 * @return false If the text contains non-digit characters.
 */
bool Compiler::IsNumber(const string &line)
{
    if (line.empty())
    {
        return (false);
    }

    for (size_t index = 0; index < line.size(); ++index)
    {
        if (index == 0)
        {
            if ((line[0] != '-') && (line[0] != '+') && (!isdigit(static_cast<unsigned char>(line[0]))))
            {
                return (false);
            }
        }
        else
        {
            if (!isdigit(static_cast<unsigned char>(line[index])))
            {
                return (false);
            }
        }
    }

    return (true);
}

/**
 * @brief Is the text blank?
 *
 * @param line Text to be checked.
 * @return true If the line is blank.
 * @return false If the line is not blank.
 */
bool Compiler::IsBlank(const string &line)
{
    return (line.empty());
}

/**
 * @brief Is the text composed solely of 0s and 1s and less than 32 bits in length?
 *
 * @param line Text to be checked.
 * @return true If the text is binary.
 * @return false If the text is not binary.
 */
bool Compiler::IsBinary(const string &line)
{
    if (line.empty() || line.size() > 32)
    {
        return (false);
    }

    for (const char character: line)
    {
        if ((character != '0') && (character != '1'))
        {
            return (false);
        }
    }

    return (true);
}