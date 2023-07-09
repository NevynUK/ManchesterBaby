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
 * @brief 
 * 
 * @param filename 
 * @return StoreLines& 
 */
StoreLines &Compiler::Compile(const char *filename)
{
    vector<const char *> *program = FileSystem::Contents(filename);
    return(Compile(*program));
}

/**
 * @brief 
 * 
 * @param program 
 * @return StoreLines& 
 */
StoreLines &Compiler::Compile(const vector<const char *> &program)
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
        for (auto token : *tokens)
        {
            uint32_t value = 0;
            if ((token->opcode == Instruction::BIN) || (token->opcode == Instruction::NUM))
            {
                value = token->operand;
            }
            else
            {
                value = token->opcode << 13;
                value |= (token->operand & 0x1f);
            }
            Register *r = new Register(value);
            sl[token->storeLineNumber] = *r;
        }
        for (auto token : *tokens)
        {
            delete token;
        }
        tokens->clear();
        delete tokens;
    }

    return(*storeLines);
}

/**
 * @brief 
 * 
 * @param lines 
 * @return vector<Compiler::TokenisedLine *>* 
 */
vector<Compiler::TokenisedLine *> *Compiler::Tokenise(const vector<const char *> &lines)
{
    vector<Compiler::TokenisedLine *> *result = new vector<Compiler::TokenisedLine *>();

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
    return(result);
}

/**
 * @brief 
 * 
 * @param line 
 * @return uint32_t 
 */
uint32_t Compiler::GetStoreLineNumber(const char *line)
{
    unsigned long number = 0;
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
                number = strtoul(copyOfLine, NULL, 10);
                if (number <= ((uint32_t) 0xffffffff))
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

uint32_t Compiler::GetOperand(const char *line)
{
    uint32_t number = 0;
    bool state = false;

    if ((line != NULL) && (strlen(line) > 0))
    {
        if (IsNumber(line))
        {
            number = strtoul(line, NULL, 10) & 0xffffffff;
            state = true;
        }
    }
    if (!state)
    {
        throw runtime_error("Invalid operand");
    }

    return(number);
}

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
 * @brief 
 * 
 * @param line 
 * @return true 
 * @return false 
 */
bool Compiler::IsComment(const char *line)
{
    return((line != NULL) && ((line[0] == ';') || ((strlen(line) >= 3) && (strncmp(line, "---", 3) == 0))));
}

/**
 * @brief 
 * 
 * @param line 
 * @return true 
 * @return false 
 */
bool Compiler::IsNumber(const char *line)
{
    if ((line == NULL) || (strlen(line) == 0))
    {
        return(false);
    }

    for (size_t index = 0; index < strlen(line); index++)
    {
        if (!isdigit(line[index]))
        {
            return(false);
        }
    }
    return(true);
}

/**
 * @brief 
 * 
 * @param line 
 * @return true 
 * @return false 
 */
bool Compiler::IsBlank(const char *line)
{
    return((line == NULL) || (strlen(line) == 0));
}

/**
 * @brief 
 * 
 * @param line 
 * @return true 
 * @return false 
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