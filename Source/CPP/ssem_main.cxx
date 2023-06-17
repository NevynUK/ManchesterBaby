#include <iostream>
#include "Instructions.hxx"

using namespace std;

extern bool TestInstructions(Instructions &instructions);
extern bool TestRegister();

void PrintPassOrFail(const string &test_name, bool result)
{
    if (result == true)
    {
        cout << "PASS:";
    }
    else
    {
        cout << "FAIL:";
    }
    cout << " " << test_name << endl;
}

int main()
{
    Instructions instructions;

    PrintPassOrFail("Instructions", TestInstructions(instructions));
    PrintPassOrFail("Register", TestRegister());
    return(0);
}