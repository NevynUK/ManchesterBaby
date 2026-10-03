#include <cerrno>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "Instructions.hpp"
#include "Compiler.hpp"
#include "StoreLines.hpp"
#include "CPU.hpp"

using namespace std;

/**
 * @brief External reference to the method that will run the unit tests.
 */
extern "C" int execute_unit_tests();

/**
 * @brief Expected results from hfr989.ssem.
 */
vector<int32_t> hfr989RunResults = {0, 16402, 16403, 32788, 49152, 8213, 32790, 24600, 16406, 32791, 24596, 16404, 24598, 16408, 49152, 25, 18, 57344, 0, -989, 42, -3, -42, 1, 0, 16, 0, 0, 0, 0, 0, 0};

/**
 * @brief Expected results from 3Minutes.ssem.
 */
vector<int32_t> threeMinutesRunResults = {0,          -1679623105, 948305920,  948043776,   -1199505408, -1744434128, -104726528, -662634496, 8585216,  1505770556, -1610481664, -635371520, 996540416, 983698480, -1574567936, -2147352576,
                                          -606011392, 1002847292,  1004666880, -2147090432, 2076246016,  35851312,    63373312,   37748736,   35651584, 64575,      1610612736,  -1,         0,         575,       2078473279,  190840833};

/**
 * @brief Expected results from Add.ssem.
 */
vector<int32_t> addRunResults = {0, 16404, 32789, 24598, 16406, 57344, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 5, -15, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/**
 * @brief Expected results from HCF1.ssem.
 */
vector<int32_t> hcf1RunResults = {0, 16408, 24602, 16410, 24603, 16407, 32795, 57344, 8212, 32794, 24601, 16409, 57344, 57344, 16410, 32789, 24603, 16411, 24602, 22, -3, 1, 4, -35, 34, 0, -34, 34, 0, 0, 0, 0};

/**
 * @brief Expected results from HCF2.ssem.
 */
vector<int32_t> hcf2RunResults = {0, 16414, 24605, 16415, 24607, 16415, 24606, 16413, 32798, 57344, 8219, 32799, 24607, 32796, 57344, 0, 57344, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -3, 2, -314159265, 271828183, -271828183};

/**
 * @brief Expected results from Parabola.ssem.
 */
vector<int32_t> parabolaRunResults = {0, 50348061, 75530269, 24605, 138461184, 57344, 16413, 270557213, 16413, 32797, 24606, 537935902, 24605, 16392, 32796, 24606, 1074282526, 24584, 32795, 24588, 16412, 32794, -2147196900, 16412, 24604, 31, 1, -8192, 8, 0, -16413, 0};

/**
 * @brief Expected results from Primes.ssem.
 */
vector<int32_t> prinesRunResults = {24, 16405, 24597, 16405, 32783, 24597, 16399, 24598, 16406, 24598, 16406, 32783, 24598, 32789, 49152, -1, 16405, 24599, 16407, 32790, 0, 2, 2, 0, 7, 49152, 8192, 24599, 16407, 32790, 49152, 20};

/**
 * @brief Expected results from TuringLongDivision.ssem.
 */
vector<int32_t> turingLongDivisionRunResults = {19, 16415, 24607, 16415, 32798, 57344, 0, 16415, 24607, 16412, 32796, 24604, 16415, 32799, 24607, 16412, 24604, 57344, 26, 57344, 24607, 16413, 32796, 32796, 24604, 27, 2, 11, 0, 4, 20, -36};

/**
 * @brief Display the contents of the store lines on the console.
 *
 * @param storeLines Store lines to be displayed.
 */
void UpdateDisplayTube(StoreLines &storeLines)
{
    printf("                   00000000001111111111222222222233\n");
    printf("                   01234567890123456789012345678901\n");
    for (uint lineNumber = 0; lineNumber < storeLines.Size(); lineNumber++)
    {
        const string binary = storeLines[lineNumber].Binary();
        const string disassembled = storeLines[lineNumber].Disassemble();
        printf("%4u: 0x%08x - %32s %-16s ; %d\n", lineNumber, static_cast<unsigned int>(storeLines[lineNumber].ReverseBits()), binary.c_str(), disassembled.c_str(), static_cast<int>(storeLines[lineNumber].GetValue()));
    }
}

/**
 * @brief Read the contents of the specified file, one line per entry.
 *
 * Opens the file at the given full path for reading.  Each line is stripped of
 * its trailing newline and carriage-return characters before being appended
 * to the result vector.  Blank lines and lines that could not be read are
 * skipped.
 *
 * @param fullPath  Full file system path to the file (e.g. "/sdcard/Add.ssem").
 * @return vector<string>  File contents with one entry per line.
 *         The vector is empty if the file could not be opened.
 */
vector<string> ReadFile(const string &fullPath)
{
    vector<string> lines;

    auto fileDeleter = [](FILE *f) {
        if (f != nullptr)
            fclose(f);
    };
    string fileName = "../../SSEMPrograms/" + fullPath;
    unique_ptr<FILE, decltype(fileDeleter)> file(fopen(fileName.c_str(), "r"), fileDeleter);
    if (file == nullptr)
    {
        fprintf(stderr, "Failed to open file %s: %s\n", fileName.c_str(), strerror(errno));
        return (lines);
    }

    // printf("Reading file %s:\n", fileName.c_str());

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file.get()) != nullptr)
    {
        string line = buffer;

        while (!line.empty() && ((line.back() == '\n') || (line.back() == '\r')))
        {
            line.pop_back();
        }

        if (!line.empty())
        {
            // printf("    %s\n", line.c_str());
            lines.push_back(std::move(line));
        }
    }

    return (lines);
}

void RunProgram(const string &fileName, const vector<int32_t> &expectedResults, uint32_t expectedInstructionCount)
{
    auto fileContents = ReadFile(fileName);
    auto storeLines = Compiler::Compile(fileContents);

    Cpu cpu(storeLines);
    cpu.Reset();

    uint instructionCount = 0;
    while (!cpu.IsStopped())
    {
        cpu.SingleStep();
        instructionCount++;
    }

    // printf("\n\n\nProgram execution complete.\n");
    // UpdateDisplayTube(storeLines);
    // printf("Executed %u instructions.\n", instructionCount);

    if (instructionCount != expectedInstructionCount)
    {
        fprintf(stderr, "Instruction count mismatch: expected %u, actual %u\n", expectedInstructionCount, instructionCount);
        exit(1);
    }

    for (uint lineNumber = 0; lineNumber < storeLines.Size(); lineNumber++)
    {
        if (storeLines[lineNumber].GetValue() != expectedResults[lineNumber])
        {
            fprintf(stderr, "Mismatch at line %u: expected %d, actual %d\n", lineNumber, expectedResults[lineNumber], storeLines[lineNumber].GetValue());
            exit(1);
        }
    }
}

/**
 * @brief Main program loop.
 */
int main(int __attribute__((unused)) argc, char *__attribute__((unused)) argv[])
{
#ifdef UNIT_TESTS
    return (execute_unit_tests() ? 0 : -1);
#else
    Instructions::PopulateLookupTable();
    RunProgram("hfr989.ssem", hfr989RunResults, 21387);
    RunProgram("3Minutes.ssem", threeMinutesRunResults, 1);
    // Clock needs looking at.
    RunProgram("Add.ssem", addRunResults, 5);
    RunProgram("HCF1.ssem", hcf1RunResults, 7);
    RunProgram("HCF2.ssem", hcf2RunResults, 9);
    // IntDivision needs looking at.
    // Nightmare needs looking at.
    RunProgram("Parabola.ssem", parabolaRunResults, 341);
    RunProgram("Primes.ssem", prinesRunResults, 15);
    RunProgram("TuringLongDivision.ssem", turingLongDivisionRunResults, 5);
#endif

    printf("All tests passed.\n");
    return (0);
}