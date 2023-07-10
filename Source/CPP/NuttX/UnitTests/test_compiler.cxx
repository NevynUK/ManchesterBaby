#include "Compiler.hxx"
#include "ConsoleUserInterface.hxx"

#define FILE_TO_READ        "hfr989.ssem"

static const char *goodApplication[] = 
{
    "; A comment",
    "--- Another comment",
    "01:   LDN 18",
    "02:   LDN 18 ; A comment",
    "03:   SUB 20",
    "04:   CMP",
    "05:   JRP 21",
    "06:   JRP 21 --- Another comment",
    "07:   STO 24",
    "16:   NUM 20",
    "08:   STOP",
    "18:   NUM 0",
    "19:   BIN 0101"
};

static const char *invalidLineNumber[] = 
{
    "01 LDN 18"
};

static const char *invalidOpcode[] = 
{
    "01:   LDM 18"
};

static const char *invalidOperand1[] = 
{
    "01:   LDN 18 19"
};

static const char *invalidOperand2[] = 
{
    "01:   LDN"
};

vector<const char *> *CreateProgram(const char **lines)
{
    vector<const char *> *program = new vector<const char *>();

    for (uint index = 0; index < sizeof(goodApplication) / sizeof(char *); index++)
    {
        program->push_back(lines[index]);
    }

    return(program);
}

bool CheckSupportingMethods()
{
    if (Compiler::IsComment(NULL))
    {
        printf("IsComment checking NULL\n");
        return(false);
    }
    if (!Compiler::IsComment("; A comment"))
    {
        printf("Checking '; A comment'\n");
        return(false);
    }
    if (!Compiler::IsComment("--- Another comment"))
    {
        printf("Checking '--- Another comment'\n");
        return(false);
    }
    if (Compiler::IsComment("01:   LDN 18"))
    {
        printf("Checking '01:   LDN 18'\n");
        return(false);
    }
    if (Compiler::IsComment("--"))
    {
        printf("Checking '---'\n");
        return(false);
    }
    if (Compiler::IsComment(NULL))
    {
        printf("IsComment checking NULL\n");
        return(false);
    }
    //
    if (Compiler::IsNumber(NULL))
    {
        printf("IsNumber checking NULL\n");
        return(false);
    }
    //
    if (!Compiler::IsBlank(NULL))
    {
        printf("IsBlank checking NULL\n");
        return(false);
    }
    if (!Compiler::IsBlank(""))
    {
        printf("Checking ''\n");
        return(false);
    }
    if (Compiler::IsBlank(" "))
    {
        printf("Checking ' '\n");
        return(false);
    }
    //
    if (Compiler::IsBinary(NULL))
    {
        printf("IsBinary checking NULL\n");
        return(false);
    }
    if (!Compiler::IsBinary("010101"))
    {
        printf("Checking '010101'\n");
        return(false);
    }
    if (!Compiler::IsBinary("01010101010101010101010101010101")) // 32 bits
    {
        printf("Checking '01010101010101010101010101010101'\n");
        return(false);
    }
    if (Compiler::IsBinary("010101010101010101010101010101010")) // 33 bits
    {
        printf("Checking '010101010101010101010101010101010'\n");
        return(false);
    }
    if (Compiler::IsBinary("ABCD"))
    {
        printf("IsBinary checking 'ABCD'\n");
        return(false);
    }
    return(true);
}

bool TestCompiler()
{
    bool result = true;

    if (!CheckSupportingMethods())
    {
        return(false);
    }

    vector<const char *> *program;
    StoreLines *storeLines;
    try
    {
        program = CreateProgram(goodApplication);
        storeLines = Compiler::Compile(*program);

        program->clear();
        delete program;
        result &= true;
    }
    catch(const std::exception& e)
    {
        printf("Creating a valid program.\n");
        result = false;
    }
    
    try
    {
        program = CreateProgram(invalidLineNumber);
        storeLines = Compiler::Compile(*program);
        program->clear();
        delete program;
        printf("Creating a program with an invalid line number.\n");
        result = false;
    }
    catch(const std::exception& e)
    {
        result &= true;
    }  

    try
    {
        program = CreateProgram(invalidOpcode);
        storeLines = Compiler::Compile(*program);
        program->clear();
        delete program;
        printf("Creating a program with an invalid opcode.\n");
        result = false;
    }
    catch(const std::exception& e)
    {
        result &= true;
    }
    
    try
    {
        program = CreateProgram(invalidOperand1);
        storeLines = Compiler::Compile(*program);
        program->clear();
        delete program;
        printf("Creating a program with an invalid operand 1.\n");
        result = false;
    }
    catch(const std::exception& e)
    {
        result &= true;
    }
    
    try
    {
        program = CreateProgram(invalidOperand2);
        storeLines = Compiler::Compile(*program);
        program->clear();
        delete program;
        printf("Creating a program with an invalid operand 2.\n");
        result = false;
    }
    catch(const std::exception& e)
    {
        result &= true;
    }
    
   return(result);
}