#ifndef __STORE_LINES_HXX
#define __STORE_LINES_HXX

#include <iostream>
#include <vector>
#include <stdexcept>

#include "Register.hxx"

using namespace std;

/**
 * @brief Object that will hold the contents of the store lines.
 */
class StoreLines
{
    private:
        /**
         * @brief Vector of store lines.
         */
         vector<Register> _lines;

    public:
        /**
         * @brief Construct a new StoreLines object
         * 
         * @param size Number of lines in this object (defaults to 32).
         */
        explicit StoreLines(uint size = 32);

        /**
         * @brief Destroy the StoreLines object
         */
        ~StoreLines();

        /**
         * @brief Get a reference to a store line.
         * 
         * @note If the index is out of range, an invalid_argment exception is thrown.

        * @param index Store line being requested.
        * @return Register& Reference to the store line requested
        */
        Register &operator[](uint index);

        /**
         * @brief Get the number of the store lines.
         * 
         * @return uint Number of store lines.
         */
        uint Size() const { return(_lines.size()); }

        /**
         * @brief Clear all of the store lines without resizing (so sets all to Register(s))
         */
        void Clear();
};

#endif // __STORE_LINES_HXX