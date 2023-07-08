#include "Register.hxx"

/**
 * @brief Test the register class.
 * 
 * @return true Register tests have passed.
 * @return false Register tests have failed.
 */
bool TestRegister()
{
    Register register1;
    Register register2(0x12345678);

    if (register1.GetValue() != 0)
    {
        printf("Register1 is not zero.\n");
        return(false);
    }
    register1.Increment();
    if (register1.GetValue() != 1)
    {
        printf("Register1 did not increment correctly.\n");
        return(false);
    }
    register1.Add(9);
    if (register1.GetValue() != 10)
    {
        printf("Register1 did not add a constant correctly.\n");
        return(false);
    }
    Register addValue(45);
    register1.Add(addValue);
    if (register1.GetValue() != 55)
    {
        printf("Register1 did not add another register correctly.\n");
        return(false);
    }
    register1.Subtract(addValue);
    if (register1.GetValue() != 10)
    {
        printf("Register1 did not subtract another register correctly.\n");
        return(false);
    }

    register1.Negate();
    if (register1.GetValue() != -10)
    {
        printf("Register1 did not negate correctly.\n");
        return(false);
    }

    register1.Negate();    
    if (register1.GetValue() != 10)
    {
        printf("Register1 did not revert back to 10.\n");
        return(false);
    }

    char *binary = register1.Binary();
    if (strcmp(binary, "01010000000000000000000000000000") != 0)
    {
        printf("Register1 did not convert to binary correctly, result '%s'.\n", binary);
        return(false);
    }
    delete[] binary;

    if (register1.LineNumber() != 10)
    {
        printf("Register1 did not return the correct line number.\n");
        return(false);
    }

    if (register2.GetValue() != 0x12345678)
    {
        return(false);
    }

    register1.SetValue(0x87654321);
    if (((uint32_t) register1.GetValue()) != 0x87654321)
    {
        return(false);
    }

    return(true);
}