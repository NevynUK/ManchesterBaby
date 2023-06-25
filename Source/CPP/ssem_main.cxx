#include <iostream>
#include <string>
#include "Instructions.hxx"

using namespace std;

extern bool TestInstructions(Instructions &instructions);
extern bool TestRegister();
extern bool TestStoreLines();

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
    Instructions instructions;

    PrintPassOrFail("Instructions", TestInstructions(instructions));
    PrintPassOrFail("Register", TestRegister());
    PrintPassOrFail("StoreLines", TestStoreLines());

    return(0);
}