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

extern "C" int main()
{
    Instructions::PopulateLookupTable();
    
    PrintPassOrFail("Instructions", TestInstructions());
    PrintPassOrFail("Register", TestRegister());
    PrintPassOrFail("StoreLines", TestStoreLines());
    PrintPassOrFail("CPU", TestCpu());
    PrintPassOrFail("FileSystem", TestFileSystem());
    PrintPassOrFail("Compiler", TestCompiler());
    PrintPassOrFail("ConsoleUserInterface", TestConsoleUserInterface());

    return(0);
}