/*-----------------------------------------------------------------------------
 * File        : SDCardTest.cpp
 * Description : Unity test cases for the SDCard component.  Tests SD card
 *               mounting, file write/read operations, append mode, and file
 *               deletion on the M5Stack Tab5.  app_main mounts the card and
 *               then runs all test cases explicitly via RUN_TEST so that the
 *               results are emitted in the standard Unity format expected by
 *               pytest-embedded.
 * Author      : Mark Stevens
 * Copyright   : Copyright (c) 2026 Mark Stevens
 * Licence     : MIT — see LICENSE in the repository root for full terms.
 * Target      : M5Stack Tab5 (ESP32-P4)
 * Build system: ESP-IDF v5.5.1
 *---------------------------------------------------------------------------*/

#include <cstdio>
#include <cstring>
#include <unity.h>
#include "SDCard.hpp"

/** @brief Path used by all test functions for temporary file operations. */
static constexpr const char *TEST_FILE_PATH = "/sdcard/sdcard_test.txt";

/** @brief Content written during write/read and append tests. */
static constexpr const char *TEST_WRITE_CONTENT = "Tab5 SDCard test line one";

/** @brief Additional content appended during the append test. */
static constexpr const char *TEST_APPEND_CONTENT = "Tab5 SDCard test line two";

/**
 * @brief Verifies that the SDCard singleton was successfully created and that
 *        the card is mounted with a valid card descriptor.
 *
 * This test relies on SDCard::Initialise() having been called in app_main
 * before any RUN_TEST calls are made.
 */
static void TestSDCardMountsSuccessfully(void)
{
    SDCard *sdCard = SDCard::GetInstance();
    TEST_ASSERT_NOT_NULL_MESSAGE(sdCard, "SDCard singleton must exist after Initialise()");
    TEST_ASSERT_TRUE_MESSAGE(sdCard->IsMounted(), "IsMounted() must return true");
    TEST_ASSERT_NOT_NULL_MESSAGE(sdCard->GetCard(), "GetCard() must return a valid descriptor");
}

/**
 * @brief Writes a known string to a file, reads it back immediately, and
 *        verifies the content is byte-for-byte identical.
 *
 * The test file is removed on completion to leave the card in a clean state.
 */
static void TestSDCardWriteAndReadBackMatches(void)
{
    FILE *file = fopen(TEST_FILE_PATH, "w");
    TEST_ASSERT_NOT_NULL_MESSAGE(file, "fopen for write must succeed");
    fputs(TEST_WRITE_CONTENT, file);
    fclose(file);

    file = fopen(TEST_FILE_PATH, "r");
    TEST_ASSERT_NOT_NULL_MESSAGE(file, "fopen for read must succeed");
    char buffer[64] = {};
    fgets(buffer, static_cast<int>(sizeof(buffer)), file);
    fclose(file);

    TEST_ASSERT_EQUAL_STRING_MESSAGE(TEST_WRITE_CONTENT, buffer, "Read content must match written content");
    remove(TEST_FILE_PATH);
}

/**
 * @brief Writes an initial line, opens the file in append mode to add a second
 *        line, then reads back both lines and verifies each matches.
 *
 * The test file is removed on completion.
 */
static void TestSDCardAppendModeAddsContent(void)
{
    FILE *file = fopen(TEST_FILE_PATH, "w");
    TEST_ASSERT_NOT_NULL_MESSAGE(file, "fopen for initial write must succeed");
    fprintf(file, "%s\n", TEST_WRITE_CONTENT);
    fclose(file);

    file = fopen(TEST_FILE_PATH, "a");
    TEST_ASSERT_NOT_NULL_MESSAGE(file, "fopen in append mode must succeed");
    fprintf(file, "%s\n", TEST_APPEND_CONTENT);
    fclose(file);

    file = fopen(TEST_FILE_PATH, "r");
    TEST_ASSERT_NOT_NULL_MESSAGE(file, "fopen for final read must succeed");

    char firstLine[64] = {};
    char secondLine[64] = {};
    fgets(firstLine, static_cast<int>(sizeof(firstLine)), file);
    fgets(secondLine, static_cast<int>(sizeof(secondLine)), file);
    fclose(file);

    // Strip the newlines added by fprintf before comparing against the
    // original constant strings.
    firstLine[strcspn(firstLine, "\n")] = '\0';
    secondLine[strcspn(secondLine, "\n")] = '\0';

    TEST_ASSERT_EQUAL_STRING_MESSAGE(TEST_WRITE_CONTENT, firstLine, "First line must match initial write content");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(TEST_APPEND_CONTENT, secondLine, "Second line must match appended content");
    remove(TEST_FILE_PATH);
}

/**
 * @brief Creates a file, deletes it via remove(), then verifies that a
 *        subsequent fopen() for reading returns nullptr.
 */
static void TestSDCardFileDeletionSucceeds(void)
{
    FILE *file = fopen(TEST_FILE_PATH, "w");
    TEST_ASSERT_NOT_NULL_MESSAGE(file, "fopen for setup write must succeed");
    fputs("delete me", file);
    fclose(file);

    const int result = remove(TEST_FILE_PATH);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, result, "remove() must return 0 on success");

    file = fopen(TEST_FILE_PATH, "r");
    TEST_ASSERT_NULL_MESSAGE(file, "fopen must return nullptr after file deletion");
    if (file != nullptr)
    {
        fclose(file);
    }
}

/**
 * @brief Application entry point for the SDCard test suite.
 *
 * Mounts the SD card then runs all test functions explicitly.  Using RUN_TEST
 * directly (rather than TEST_CASE auto-registration) avoids reliance on
 * constructor-attribute linker tricks in C++ translation units and produces
 * deterministic Unity output that pytest-embedded can parse reliably.
 */
extern "C" void app_main(void)
{
    SDCard::Initialise();
    UNITY_BEGIN();
    RUN_TEST(TestSDCardMountsSuccessfully);
    RUN_TEST(TestSDCardWriteAndReadBackMatches);
    RUN_TEST(TestSDCardAppendModeAddsContent);
    RUN_TEST(TestSDCardFileDeletionSucceeds);
    UNITY_END();
}
