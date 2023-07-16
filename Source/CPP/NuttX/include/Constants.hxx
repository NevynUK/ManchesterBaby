#ifndef __CONSTANTS_HXX
#define __CONSTANTS_HXX

#include <stdlib.h>

/**
 * @brief Place to hold the constants used in the SSEM.
 */
class Constants
{
    public:
        /**
         * @brief Maximum amount of space to allocate for a line of text in a source file or to output to the console.
         */
        static const unsigned LINE_LENGTH = 128;

        /**
         * @brief Mask for the line number part of a store line.
         */
        static const unsigned LINE_NUMBER_MASK = 0x1f;

        /**
         * @brief Number of bits to shit the opcode to put it in the right place in a store line.
         */
        static const unsigned OPCODE_SHIFT = 13;

        /**
         * @brief Mask bits for the opcode part of a store line.
         */
        static const unsigned OPCODE_MASK = 0x7;
};

#endif // __CONSTANTS_HXX