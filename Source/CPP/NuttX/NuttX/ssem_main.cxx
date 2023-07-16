#include <iostream>
#include <time.h>

#include "Instructions.hxx"
#include "FileSystem.hxx"
#include "Compiler.hxx"
#include "StoreLines.hxx"
#include "CPU.hxx"
#include "ConsoleUserInterface.hxx"

using namespace std;

/**
 * @brief External reference to the method that will run the unit tests.
 */
extern "C" int execute_unit_tests();

/**
 * @brief Main program loop.
 */
extern "C" int main(int argc, char *argv[])
{
#ifdef UNIT_TESTS
    return(execute_unit_tests() ? 0 : -1);
#else
    Instructions::PopulateLookupTable();
    StoreLines *storeLines = Compiler::Compile("hfr989.ssem");
    ConsoleUserInterface consoleUserInterface;
    consoleUserInterface.UpdateDisplayTube(*storeLines);

    Cpu *cpu = new Cpu(*storeLines);
    cpu->Reset();
    uint instructionCount = 0;

    struct timespec spec, spec2;
    clock_gettime(CLOCK_REALTIME, &spec);

    while (!cpu->IsStopped())
    {
        cpu->SingleStep();
        instructionCount++;
    }

    clock_gettime(CLOCK_REALTIME, &spec2);

    printf("\n\n\nProgram execution complete.\n");
    consoleUserInterface.UpdateDisplayTube(*storeLines);
    printf("Executed %u instructions in %d nanoseconds.\n", instructionCount, spec2.tv_nsec - spec.tv_nsec);

    delete storeLines;
    delete cpu;
#endif

    return(0);
}