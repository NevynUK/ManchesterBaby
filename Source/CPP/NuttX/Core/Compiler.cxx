#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include <vector>

#include "Instruction.hxx"
#include "Instructions.hxx"
#include "Compiler.hxx"
#include "FileSystem.hxx"
#include "Constants.hxx"

using namespace std;

/**
 * @brief Delete the tokens and release any memory associated with them.
 * 
 * @param tokens Vector of tokens to be deleted.
 */
void Compiler::CleanUp(vector<Compiler::TokenisedLine *> *tokens)
{
    for (auto token : *tokens)
    {
        delete token;
    }
    tokens->clear();
    delete tokens;
}

/**
 * @brief Read a file from the file system and compile the contents returning the
 *        compiled program as store lines.
 * 
 * @param filename Name of the file to be read.
 * @return StoreLines* StoreLines containing the compiled program.
 */
StoreLines *Compiler::Compile(const char *filename)
{
    vector<const char *> *program = FileSystem::Contents(filename);
    return(Compile(*program));
}

/**
 * @brief Compile the program contained in the vector of const char * pointers.
 * 
 * @param program Vector containing char * lines of text to be compiled.
 * @return StoreLines* StoreLines containing the compiled program.
 */
StoreLines *Compiler::Compile(const vector<const char *> &program)
{
    if (program.size() == 0)
    {
        throw runtime_error("Program is null");
    }

    vector<Compiler::TokenisedLine *> *tokens = Tokenise(program);
    StoreLines *storeLines = new StoreLines();
    StoreLines &sl = *storeLines;
    if (tokens->size() > 0)
    {
        try
        {
            for (auto token : *tokens)
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
                sl[token->storeLineNumber].SetValue(value);
            }
        }
        catch(const std::exception& e)
        {
            CleanUp(tokens);
            delete storeLines;
            throw e;
        }
    }
    CleanUp(tokens);

    return(storeLines);
}

/**
 * @brief Convert the lines of text into a number of tokenised lines.
 * 
 * Note that the elements of the line are separated by one or more spaces.
 * 
 * @param lines Vector containing the lines of text
 * @return vector<Compiler::TokenisedLine *>* Tokenised lines.
 */
vector<Compiler::TokenisedLine *> *Compiler::Tokenise(const vector<const char *> &lines)
{
    vector<Compiler::TokenisedLine *> *result = new vector<Compiler::TokenisedLine *>();

    try
    {
        for (auto line : lines)
        {
            char copyOfLine[Constants::LINE_LENGTH];
            strcpy(copyOfLine, line);
            char *text = strtok(copyOfLine, " ");
            while (text != NULL)
            {
                if (IsBlank(line) || IsComment(line))
                {
                    break;
                }
                uint32_t storeLineNumber = GetStoreLineNumber(text);
                text = strtok(NULL, " ");
                Instruction::opcodes_e opcode = Instructions::Opcode(text);
                text = strtok(NULL, " ");
                uint32_t operand = 0;
                switch (opcode)
                {
                    case Instruction::JMP:
                    case Instruction::JPR:
                    case Instruction::LDN:
                    case Instruction::STO:
                    case Instruction::SUB:
                    case Instruction::NUM: 
                        operand = GetOperand(text);
                        break;
                    case Instruction::BIN:
                        operand = GetBinary(text);
                        break;
                    default:
                        if (!(IsBlank(text) || IsComment(text)))
                        {
                            throw runtime_error("Unexpected operand");
                        }
                        break;
                }
                text = strtok(NULL, " ");
                if (!(IsBlank(text) || IsComment(text)))
                {
                    throw runtime_error("Unexpected text");
                }
                text = NULL;
                TokenisedLine *tokenisedLine = new TokenisedLine();
                tokenisedLine->storeLineNumber = storeLineNumber;
                tokenisedLine->opcode = opcode;
                tokenisedLine->operand = operand;
                result->push_back(tokenisedLine);
            }
        }
    }
    catch(const std::exception& e)
    {
        CleanUp(result);
        throw e;
    }

    return(result);
}

/**
 * @brief Convert the first part of the line into the store line number.
 * 
 * @param line First part of the line being compiled.
 * @return uint32_t StoreLine number for this line.
 * @throws runtime_error If the line number is invalid.
 */
uint32_t Compiler::GetStoreLineNumber(const char *line)
{
    long number = 0;
    uint32_t result = 0;
    bool state = false;

    if ((line != NULL) && (strlen(line) > 0))
    {
        char *copyOfLine = strdup(line);
        if (copyOfLine[strlen(copyOfLine) - 1] == ':')
        {
            copyOfLine[strlen(line) - 1] = '\0';
            if (IsNumber(copyOfLine))
            {
                number = strtol(copyOfLine, NULL, 10);
                if ((number >= 0) && (number <= UINT32_MAX))
                {
                    result = number;
                    state = true;
                }            
            }
        }
        free(copyOfLine);
    }
    if (!state)
    {
        throw runtime_error("Invalid store line number");
    }

    return(result);
}

/**
 * @brief Convert the text into an operand.
 * 
 * @param line Part of the line to be converted
 * @return uint32_t Opcode representing the text.
 * @throws runtime_error If the opcode is invalid or the value is out of range.
 */
int32_t Compiler::GetOperand(const char *line)
{
    long number = 0;
    bool state = false;

    if ((line != NULL) && (strlen(line) > 0))
    {
        if (IsNumber(line))
        {
            number = strtol(line, NULL, 10);
            if ((number > INT32_MAX) || (number < INT32_MIN))
            {
                throw runtime_error("Operand out of range");
            }
            state = true;
        }
    }
    if (!state)
    {
        throw runtime_error("Invalid operand");
    }

    return((int32_t) number);
}

/**
 * @brief Convert the text from binary into a number.
 * 
 * @param line Partial line to be converted.
 * @return uint32_t Number represented by the value.
 * @throws runtime_error If the value is not a binary number
 */
uint32_t Compiler::GetBinary(const char *line)
{
    uint32_t number = 0;
    bool state = false;

    if ((line != NULL) && (strlen(line) > 0))
    {
        if (IsBinary(line))
        {
            number = strtoul(line, NULL, 2) & 0xffffffff;
            state = true;
        }
    }
    if (!state)
    {
        throw runtime_error("Invalid binary number");
    }

    return(number);
}

/**
 * @brief Is the text a comment?
 * 
 * @param line Text to be checked.
 * @return true If the text represents a comment.
 * @return false If the text is not a comment.
 */
bool Compiler::IsComment(const char *line)
{
    return((line != NULL) && ((line[0] == ';') || ((strlen(line) >= 3) && (strncmp(line, "---", 3) == 0))));
}

/**
 * @brief Can the text be a number (all characters are digits possibly starting with + or -).
 * 
 * @param line Text to be checked.
 * @return true If the text could be a number.
 * @return false If the text contains non-digit characters.
 */
bool Compiler::IsNumber(const char *line)
{
    if ((line == NULL) || (strlen(line) == 0))
    {
        return(false);
    }

    for (size_t index = 0; index < strlen(line); index++)
    {
        if (index == 0)
        {
            if ((line[0] != '-') && (line[0] != '+') && (!isdigit(line[0])))
            {
                return(false);
            }
        }
        else
        {
            if (!isdigit(line[index]))
            {
                return(false);
            }
        }
    }
    return(true);
}

/**
 * @brief Is the text blank?
 * 
 * @param line Text to be checked.
 * @return true If the line is blank.
 * @return false If the line is not blank.
 */
bool Compiler::IsBlank(const char *line)
{
    return((line == NULL) || (strlen(line) == 0));
}

/**
 * @brief Is the text composed solely of 0s and 1s and less than 32 bits in length?
 * 
 * @param line Text to be checked.
 * @return true If the text is binary.
 * @return false If the text is not binary.
 */
bool Compiler::IsBinary(const char *line)
{
    if ((line == NULL) || (strlen(line) == 0) || (strlen(line) > 32))
    {
        return(false);
    }
    for (size_t index = 0; index < strlen(line); index++)
    {
        if ((line[index] != '0') && (line[index] != '1'))
        {
            return(false);
        }
    }
    return(true);
}