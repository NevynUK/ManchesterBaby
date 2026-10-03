#include <iostream>
#include "Instructions.hpp"

using namespace std;

extern bool TestInstructions();
extern bool TestRegister();
extern bool TestStoreLines();
extern bool TestCpu();
extern bool TestCompiler();

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
    result &= TestCompiler();
    PrintPassOrFail("Compiler", result);

    return (result);
}