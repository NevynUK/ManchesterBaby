#include <iostream>
#include <string>
#include "Instructions.hxx"

using namespace std;

extern bool TestInstructions();
extern bool TestRegister();
extern bool TestStoreLines();
extern bool TestCpu();
extern bool TestAssembler();
extern bool TestConsoleUserInterface();

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
    PrintPassOrFail("Instructions", TestInstructions());
    PrintPassOrFail("Register", TestRegister());
    PrintPassOrFail("StoreLines", TestStoreLines());
    PrintPassOrFail("CPU", TestCpu());
    PrintPassOrFail("Assembler", TestAssembler());
    PrintPassOrFail("ConsoleUserInterface", TestConsoleUserInterface());

    return(0);
}