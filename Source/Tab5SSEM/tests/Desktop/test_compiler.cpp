#include <string>
#include <vector>

#include "Compiler.hpp"

using namespace std;

#define FILE_TO_READ "hfr989.ssem"

static const string goodApplication[] = {"; A comment",  "--- Another comment", "01:   LDN 18", "02:   LDN 18 ; A comment", "03:   SUB 20",   "04:   CMP", "05:   JRP 21", "06:   JRP 21 --- Another comment", "07:   STO 24",
                                         "16:   NUM 20", "08:   STOP",          "18:   NUM 0",  "19:   BIN 0101",           "20:   NUM -989", ""};

static const string invalidLineNumber[] = {"01 LDN 18"};

static const string invalidOpcode[] = {"01:   LDM 18"};

static const string invalidOperand1[] = {"01:   LDN 18 19"};

static const string invalidOperand2[] = {"01:   LDN"};

vector<string> CreateProgram(const string *lines, uint size)
{
    vector<string> program;

    for (uint index = 0; index < size; index++)
    {
        program.push_back(lines[index]);
    }

    return (program);
}

bool CheckSupportingMethods()
{
    if (Compiler::IsComment(""))
    {
        printf("IsComment checking empty string\n");
        return (false);
    }
    if (!Compiler::IsComment("; A comment"))
    {
        printf("Checking '; A comment'\n");
        return (false);
    }
    if (!Compiler::IsComment("--- Another comment"))
    {
        printf("Checking '--- Another comment'\n");
        return (false);
    }
    if (Compiler::IsComment("01:   LDN 18"))
    {
        printf("Checking '01:   LDN 18'\n");
        return (false);
    }
    if (Compiler::IsComment("--"))
    {
        printf("Checking '--'\n");
        return (false);
    }
    //
    if (Compiler::IsNumber(""))
    {
        printf("IsNumber checking empty string\n");
        return (false);
    }
    //
    if (!Compiler::IsBlank(""))
    {
        printf("Checking ''\n");
        return (false);
    }
    if (Compiler::IsBlank(" "))
    {
        printf("Checking ' '\n");
        return (false);
    }
    //
    if (Compiler::IsBinary(""))
    {
        printf("IsBinary checking empty string\n");
        return (false);
    }
    if (!Compiler::IsBinary("010101"))
    {
        printf("Checking '010101'\n");
        return (false);
    }
    if (!Compiler::IsBinary("01010101010101010101010101010101")) // 32 bits
    {
        printf("Checking '01010101010101010101010101010101'\n");
        return (false);
    }
    if (Compiler::IsBinary("010101010101010101010101010101010")) // 33 bits
    {
        printf("Checking '010101010101010101010101010101010'\n");
        return (false);
    }
    if (Compiler::IsBinary("ABCD"))
    {
        printf("IsBinary checking 'ABCD'\n");
        return (false);
    }
    return (true);
}

bool TestCompiler()
{
    bool result = true;

    if (!CheckSupportingMethods())
    {
        return (false);
    }

    try
    {
        vector<string> program = CreateProgram(goodApplication, sizeof(goodApplication) / sizeof(string));
        StoreLines storeLines = Compiler::Compile(program);
        result &= true;
    }
    catch (const std::exception &e)
    {
        printf("Creating a valid program.\n");
        result = false;
    }

    try
    {
        vector<string> program = CreateProgram(invalidLineNumber, sizeof(invalidLineNumber) / sizeof(string));
        StoreLines storeLines = Compiler::Compile(program);
        printf("Creating a program with an invalid line number.\n");
        result = false;
    }
    catch (const std::exception &e)
    {
        result &= true;
    }

    try
    {
        vector<string> program = CreateProgram(invalidOpcode, sizeof(invalidOpcode) / sizeof(string));
        StoreLines storeLines = Compiler::Compile(program);
        printf("Creating a program with an invalid opcode.\n");
        result = false;
    }
    catch (const std::exception &e)
    {
        result &= true;
    }

    try
    {
        vector<string> program = CreateProgram(invalidOperand1, sizeof(invalidOperand1) / sizeof(string));
        StoreLines storeLines = Compiler::Compile(program);
        printf("Creating a program with an invalid operand 1.\n");
        result = false;
    }
    catch (const std::exception &e)
    {
        result &= true;
    }

    try
    {
        vector<string> program = CreateProgram(invalidOperand2, sizeof(invalidOperand2) / sizeof(string));
        StoreLines storeLines = Compiler::Compile(program);
        printf("Creating a program with an invalid operand 2.\n");
        result = false;
    }
    catch (const std::exception &e)
    {
        result &= true;
    }

    return (result);
}