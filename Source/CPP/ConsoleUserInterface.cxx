#include <stdio.h>
#include "ConsoleUserInterface.hxx"

/**
 * @brief Construct a new ConsoleUserInterface object
 */
ConsoleUserInterface::ConsoleUserInterface() : UserInterface() { }

/**
 * @brief Destroy the ConsoleUserInterface object
 */
ConsoleUserInterface::~ConsoleUserInterface() { }

/**
 * @brief Display the contents of the store lines on the console.
 * 
 * @param storeLines Store lines to be displayed.
 */
void ConsoleUserInterface::UpdateDisplayTube(StoreLines &storeLines) const
{
    printf("                 00000000001111111111222222222233\n");
    printf("                 01234567890123456789012345678901\n");
    for (uint lineNumber = 0; lineNumber < storeLines.Size(); lineNumber++)
    {
        printf("%4d: %08x - %32s %s\n", lineNumber, storeLines[lineNumber].GetValue(), 
                                        storeLines[lineNumber].Binary(), 
                                        storeLines[lineNumber].Disassemble());
    }
}

/**
 * @brief Show the number of lines executed on the console.
 * 
 * @param number_of_lines Number of line executed.
 */
void ConsoleUserInterface::UpdateProgress(uint32_t number_of_lines)
{
    printf("Executed %u lines\n", number_of_lines);
}

/**
 * @brief Present an error message to the console.
 * 
 * @param error Message to display.
 */
void ConsoleUserInterface::DisplayError(const char *error)
{
    printf("**** Error: %s\n", error);
}
