#include "StoreLines.hxx"

/**
 * @brief Construct a new StoreLines object
 * 
 * @param size Number of lines in this object.
 */
StoreLines::StoreLines(uint size)
{
    _lines.resize(size);
    for (auto &line : _lines)
    {
        line = Register();
    }
}

/**
 * @brief Destroy the StoreLines object
 */
StoreLines::~StoreLines()
{
    _lines.clear();
}

/**
 * @brief Get a reference to a store line.
 * 
 * @note If the index is out of range, an invalid_argment exception is thrown.

 * @param index Store line being requested.
 * @return Register& Reference to the store line requested
 */
Register &StoreLines::operator[](uint index)
{
    if (index >= _lines.size())
    {
        throw(std::invalid_argument("Index out of range"));
    }

    return(_lines[index]);
}

/**
 * @brief Clear all of the store lines without resizing (so sets all to 0).
 */
void StoreLines::Clear()
{
    for (auto &line : _lines)
    {
        line.SetValue(0);
    }
}