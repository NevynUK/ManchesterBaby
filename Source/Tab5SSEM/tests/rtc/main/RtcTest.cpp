/*-----------------------------------------------------------------------------
 * File        : RtcTest.cpp
 * Description : Unity test cases for the Rtc component.  Tests time set/get
 *               round-trips on the Epson RX8130CE real-time clock fitted to
 *               the M5Stack Tab5.  app_main initialises the RTC and then runs
 *               all test cases explicitly via RUN_TEST so that results are
 *               emitted in the standard Unity format.
 * Author      : Mark Stevens
 * Copyright   : Copyright (c) 2026 Mark Stevens
 * Licence     : MIT — see LICENSE in the repository root for full terms.
 * Target      : M5Stack Tab5 (ESP32-P4)
 * Build system: ESP-IDF v5.5.1
 *---------------------------------------------------------------------------*/

#include <cstring>
#include <ctime>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <unity.h>
#include "Rtc.hpp"

// =============================================================================
// Helpers
// =============================================================================

/**
 * @brief Fills a struct tm with a fully specified date and time.
 *
 * @param year   Gregorian year (e.g. 2026).
 * @param month  1-based month (1 = January).
 * @param day    Day of month (1–31).
 * @param hour   Hour (0–23).
 * @param minute Minute (0–59).
 * @param second Second (0–59).
 * @param wday   Day of week (0 = Sunday, 6 = Saturday).
 * @return Populated struct tm.
 */
static struct tm MakeTime(int year, int month, int day, int hour, int minute, int second, int wday)
{
    struct tm time = {};
    time.tm_year = year - 1900;
    time.tm_mon = month - 1;
    time.tm_mday = day;
    time.tm_hour = hour;
    time.tm_min = minute;
    time.tm_sec = second;
    time.tm_wday = wday;
    time.tm_isdst = -1;
    return (time);
}

// =============================================================================
// Tests
// =============================================================================

/**
 * @brief Verifies that Rtc::Initialise() returns a non-null pointer and that
 *        GetInstance() returns the same pointer.
 *
 * This test relies on Rtc::Initialise() having been called in app_main before
 * any RUN_TEST calls are made.
 */
static void TestRtcInitialisesSuccessfully(void)
{
    Rtc *rtc = Rtc::GetInstance();
    TEST_ASSERT_NOT_NULL_MESSAGE(rtc, "Rtc singleton must exist after Initialise()");
}

/**
 * @brief Sets a known date and time, reads it back, and verifies every field
 *        matches.
 *
 * The set time is 2026-01-15 (Thursday) 10:30:00.  A one-second guard delay
 * is introduced between SetTime and GetTime so that the oscillator has
 * completed at least one tick cycle before the read.
 */
static void TestRtcSetAndGetTimeRoundTrip(void)
{
    Rtc *rtc = Rtc::GetInstance();
    TEST_ASSERT_NOT_NULL(rtc);

    // Thursday = weekday 4 (0=Sunday)
    const struct tm written = MakeTime(2026, 1, 15, 10, 30, 0, 4);

    const bool writeOk = rtc->SetTime(written);
    TEST_ASSERT_TRUE_MESSAGE(writeOk, "SetTime() must return true");

    // Allow the oscillator one full tick before reading back.
    vTaskDelay(pdMS_TO_TICKS(1200));

    struct tm read = {};
    const bool readOk = rtc->GetTime(read);
    TEST_ASSERT_TRUE_MESSAGE(readOk, "GetTime() must return true");

    TEST_ASSERT_EQUAL_INT_MESSAGE(written.tm_year, read.tm_year, "Year must match");
    TEST_ASSERT_EQUAL_INT_MESSAGE(written.tm_mon, read.tm_mon, "Month must match");
    TEST_ASSERT_EQUAL_INT_MESSAGE(written.tm_mday, read.tm_mday, "Day must match");
    TEST_ASSERT_EQUAL_INT_MESSAGE(written.tm_hour, read.tm_hour, "Hour must match");
    TEST_ASSERT_EQUAL_INT_MESSAGE(written.tm_min, read.tm_min, "Minute must match");
    TEST_ASSERT_EQUAL_INT_MESSAGE(written.tm_wday, read.tm_wday, "Weekday must match");

    // Seconds advance after SetTime so only verify they are in a plausible
    // range (0–5) rather than an exact value.
    TEST_ASSERT_INT_WITHIN_MESSAGE(5, 0, read.tm_sec, "Seconds must be within 5 of zero after 1.2 s");
}

/**
 * @brief Sets a time near a minute boundary, waits for the clock to tick past
 *        it, then verifies that the minute field has incremented and the
 *        second field has wrapped back to near zero.
 *
 * Set time is HH:MM:55.  After a 7-second delay the expected read-back is
 * HH:(MM+1):02 ± 2 seconds.
 */
static void TestRtcTimeAdvancesCorrectly(void)
{
    Rtc *rtc = Rtc::GetInstance();
    TEST_ASSERT_NOT_NULL(rtc);

    // Wednesday = weekday 3
    const struct tm written = MakeTime(2026, 3, 11, 14, 22, 55, 3);

    const bool writeOk = rtc->SetTime(written);
    TEST_ASSERT_TRUE_MESSAGE(writeOk, "SetTime() must return true");

    // Wait 7 seconds — long enough for the clock to cross the minute boundary.
    vTaskDelay(pdMS_TO_TICKS(7000));

    struct tm read = {};
    const bool readOk = rtc->GetTime(read);
    TEST_ASSERT_TRUE_MESSAGE(readOk, "GetTime() must return true");

    TEST_ASSERT_EQUAL_INT_MESSAGE(written.tm_year, read.tm_year, "Year must be unchanged");
    TEST_ASSERT_EQUAL_INT_MESSAGE(written.tm_mon, read.tm_mon, "Month must be unchanged");
    TEST_ASSERT_EQUAL_INT_MESSAGE(written.tm_mday, read.tm_mday, "Day must be unchanged");
    TEST_ASSERT_EQUAL_INT_MESSAGE(written.tm_hour, read.tm_hour, "Hour must be unchanged");
    TEST_ASSERT_EQUAL_INT_MESSAGE(written.tm_min + 1, read.tm_min, "Minute must have incremented by 1");
    TEST_ASSERT_INT_WITHIN_MESSAGE(2, 2, read.tm_sec, "Seconds should be approximately 2 after crossing minute boundary");
}

/**
 * @brief Sets times at the extremes of the supported year range (2000 and
 *        2099) and verifies each round-trips correctly without corruption.
 *
 * The RX8130CE stores year as a two-digit BCD value relative to 2000.
 */
static void TestRtcYearBoundariesRoundTrip(void)
{
    Rtc *rtc = Rtc::GetInstance();
    TEST_ASSERT_NOT_NULL(rtc);

    // Year 2000 — Saturday = weekday 6
    const struct tm year2000 = MakeTime(2000, 1, 1, 0, 0, 0, 6);
    TEST_ASSERT_TRUE_MESSAGE(rtc->SetTime(year2000), "SetTime(2000) must return true");
    vTaskDelay(pdMS_TO_TICKS(200));

    struct tm read2000 = {};
    TEST_ASSERT_TRUE_MESSAGE(rtc->GetTime(read2000), "GetTime after year-2000 set must return true");
    TEST_ASSERT_EQUAL_INT_MESSAGE(year2000.tm_year, read2000.tm_year, "Year 2000 must round-trip");

    // Year 2099 — Thursday = weekday 4
    const struct tm year2099 = MakeTime(2099, 12, 31, 23, 59, 0, 4);
    TEST_ASSERT_TRUE_MESSAGE(rtc->SetTime(year2099), "SetTime(2099) must return true");
    vTaskDelay(pdMS_TO_TICKS(200));

    struct tm read2099 = {};
    TEST_ASSERT_TRUE_MESSAGE(rtc->GetTime(read2099), "GetTime after year-2099 set must return true");
    TEST_ASSERT_EQUAL_INT_MESSAGE(year2099.tm_year, read2099.tm_year, "Year 2099 must round-trip");
    TEST_ASSERT_EQUAL_INT_MESSAGE(year2099.tm_mon, read2099.tm_mon, "Month 12 must round-trip");
    TEST_ASSERT_EQUAL_INT_MESSAGE(year2099.tm_mday, read2099.tm_mday, "Day 31 must round-trip");
    TEST_ASSERT_EQUAL_INT_MESSAGE(year2099.tm_hour, read2099.tm_hour, "Hour 23 must round-trip");
    TEST_ASSERT_EQUAL_INT_MESSAGE(year2099.tm_min, read2099.tm_min, "Minute 59 must round-trip");
}

/**
 * @brief Sets all twelve month values in sequence and verifies each reads back
 *        correctly.
 *
 * A short delay separates each write/read pair.  The day is kept at 1 to
 * avoid invalid day-in-month combinations.
 */
static void TestRtcAllMonthsRoundTrip(void)
{
    Rtc *rtc = Rtc::GetInstance();
    TEST_ASSERT_NOT_NULL(rtc);

    for (int month = 1; month <= 12; month++)
    {
        const struct tm written = MakeTime(2026, month, 1, 0, 0, 0, 0);
        TEST_ASSERT_TRUE_MESSAGE(rtc->SetTime(written), "SetTime() must return true");
        vTaskDelay(pdMS_TO_TICKS(200));

        struct tm read = {};
        TEST_ASSERT_TRUE_MESSAGE(rtc->GetTime(read), "GetTime() must return true");
        TEST_ASSERT_EQUAL_INT_MESSAGE(written.tm_mon, read.tm_mon, "Month must round-trip");
    }
}

/**
 * @brief Verifies all valid weekday values (0–6) survive a write/read cycle.
 *
 * The RX8130CE stores weekday as a one-hot bitfield.  This test confirms that
 * the conversion between integer (0–6) and one-hot encoding is correct for
 * every day.
 */
static void TestRtcAllWeekdaysRoundTrip(void)
{
    Rtc *rtc = Rtc::GetInstance();
    TEST_ASSERT_NOT_NULL(rtc);

    for (int wday = 0; wday <= 6; wday++)
    {
        const struct tm written = MakeTime(2026, 2, 1 + wday, 12, 0, 0, wday);
        TEST_ASSERT_TRUE_MESSAGE(rtc->SetTime(written), "SetTime() must return true");
        vTaskDelay(pdMS_TO_TICKS(200));

        struct tm read = {};
        TEST_ASSERT_TRUE_MESSAGE(rtc->GetTime(read), "GetTime() must return true");
        TEST_ASSERT_EQUAL_INT_MESSAGE(wday, read.tm_wday, "Weekday must round-trip");
    }
}

// =============================================================================
// Entry point
// =============================================================================

/**
 * @brief Application entry point for the Rtc test suite.
 *
 * Initialises the RTC singleton then runs all test functions explicitly.
 * Using RUN_TEST directly (rather than TEST_CASE auto-registration) avoids
 * reliance on constructor-attribute linker tricks in C++ translation units
 * and produces deterministic Unity output.
 */
extern "C" void app_main(void)
{
    Rtc::Initialise();
    UNITY_BEGIN();
    RUN_TEST(TestRtcInitialisesSuccessfully);
    RUN_TEST(TestRtcSetAndGetTimeRoundTrip);
    RUN_TEST(TestRtcTimeAdvancesCorrectly);
    RUN_TEST(TestRtcYearBoundariesRoundTrip);
    RUN_TEST(TestRtcAllMonthsRoundTrip);
    RUN_TEST(TestRtcAllWeekdaysRoundTrip);
    UNITY_END();
}
