#include "../Register.hxx"

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
        printf("Register1 is not zero");
        return(false);
    }
    register1.Increment();
    if (register1.GetValue() != 1)
    {
        printf("Register1 did not increment correctly.");
        return(false);
    }
    register1.Add(9);
    if (register1.GetValue() != 10)
    {
        printf("Register1 did not add a constant correctly.");
        return(false);
    }
    Register addValue(45);
    register1.Add(addValue);
    if (register1.GetValue() != 55)
    {
        printf("Register1 did not add another register correctly.");
        return(false);
    }
    register1.Subtract(addValue);
    if (register1.GetValue() != 10)
    {
        printf("Register1 did not subtract another register correctly.");
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