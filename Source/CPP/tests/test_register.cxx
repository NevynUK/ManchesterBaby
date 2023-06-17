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
        return(false);
    }

    if (register2.GetValue() != 0x12345678)
    {
        return(false);
    }

    register1.SetValue(0x87654321);
    if (register1.GetValue() != 0x87654321)
    {
        return(false);
    }
    
    return(true);
}