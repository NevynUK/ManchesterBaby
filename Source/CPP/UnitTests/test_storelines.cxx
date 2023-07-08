#include "StoreLines.hxx"

/**
 * @brief Test the creation of a StoreLines object.
 * 
 * @return true If the tests pass.
 * @return false If the tests fail.
 */
bool TestStoreLineCreation()
{
    bool result = true;
    StoreLines storeLines;

    //
    //  Create a store lines object with the default size of 32.
    //
    if (storeLines.Size() != 32)
    {
        result = false;
    }
    //
    //  Check one of the values.
    //
    if (storeLines[0].GetValue() != Register().GetValue())
    {
        result = false;
    }
    //
    //  Now with a non-default size.
    //
    StoreLines storeLines2(64);
    if (storeLines2.Size() != 64)
    {
        result = false;
    }

    return(result);
}

/**
 * @brief Test accessing elements in the store lines.
 * 
 * @return true If the tests pass.
 * @return false If the tests fail.
 */
bool TestStoreLinesAccess()
{
    bool result = true;
    StoreLines storeLines;

    //
    //  Check that we can set the value of a store line.
    //
    storeLines[0].SetValue(1);
    if (storeLines[0].GetValue() != 1)
    {
        result = false;
    }
    //
    //  Clearing the store lines should reset line 0 to 0 and leave the size at the default of 32.
    //
    storeLines.Clear();
    if (storeLines[0].GetValue() != 0)
    {
        result = false;
    }
    if (storeLines.Size() != 32)
    {
        result = false;
    }
    //
    //  Check that we can't access a store line that is out of range.
    //
    try
    {
        storeLines[32].SetValue(1);
        result = false;
    }
    catch (const std::invalid_argument &e)
    {
        //  This is what we expect.
    }
    catch (...)
    {
        result = false;
    }

    return(result);
}

/**
 * @brief Test the StoreLines object.
 * 
 * @return true StoreLines tests pass.
 * @return false StoreLines tests fail.
 */
bool TestStoreLines()
{
    bool result = true;

    result &= TestStoreLineCreation();
    result &= TestStoreLinesAccess();

    return(result);
}