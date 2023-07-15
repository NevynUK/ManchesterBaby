#include <iostream>
#include "Instructions.hxx"
#include "FileSystem.hxx"

using namespace std;

extern bool TestInstructions();
extern bool TestRegister();
extern bool TestStoreLines();
extern bool TestCpu();
extern bool TestCompiler();
extern bool TestConsoleUserInterface();
extern bool TestFileSystem();

void PrintPassOrFail(const string &test_name, bool result)
{
    if (result == true)
    {
        printf("PASS:");
    }
    else
    {
        printf("FAIL:");
    }
    printf(" %s\n", test_name.c_str());
}

extern "C" bool execute_unit_tests()
{
    bool result = true;

    Instructions::PopulateLookupTable();
    
    result &= TestInstructions();
    PrintPassOrFail("Instructions", result);
    result &= TestRegister();
    PrintPassOrFail("Register", result);
    result &= TestStoreLines();
    PrintPassOrFail("StoreLines", result);
    result &= TestCpu();
    PrintPassOrFail("CPU", result);
    result &= TestFileSystem();
    PrintPassOrFail("FileSystem", result);
    result &= TestCompiler();
    PrintPassOrFail("Compiler", result);
    result &= TestConsoleUserInterface();
    PrintPassOrFail("ConsoleUserInterface", result);

    return(result);
}