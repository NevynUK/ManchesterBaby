#include <stdio.h>
#include "StoreLines.hxx"
#include "ConsoleUserInterface.hxx"

bool TestConsoleUserInterface()
{
    ConsoleUserInterface consoleUserInterface;
    StoreLines storeLines(2);

    printf("\n");
    consoleUserInterface.UpdateDisplayTube(storeLines);

    printf("\n");
    storeLines[0] = Register(10);
    consoleUserInterface.UpdateDisplayTube(storeLines);

    printf("\n");
    storeLines[0].SetValue(0b0100000000001010);     // LDN 10
    storeLines[1].SetValue(0b1000000000001011);     // SUB 11
    consoleUserInterface.UpdateDisplayTube(storeLines);

    printf("\n");
    consoleUserInterface.UpdateProgress(10);

    printf("\n");
    consoleUserInterface.DisplayError("This is an error");

    return(true);
}
 