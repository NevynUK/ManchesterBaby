/*-----------------------------------------------------------------------------
 * File        : Utility.cpp
 * Description : General-purpose utility functions for the SSEM emulator,
 *               including numeric formatting helpers.
 * Author      : Mark Stevens
 * Copyright   : Copyright (c) 2026 Mark Stevens
 * Licence     : MIT — see LICENSE in the repository root for full terms.
 * Target      : M5Stack Tab5 (ESP32-P4)
 * Build system: ESP-IDF v5.5.1
 *---------------------------------------------------------------------------*/

#include "Utility.hpp"
#include <cstdio>
#include <cstring>
#include <inttypes.h>

using namespace std;

/**
 * @brief Formats an unsigned 32-bit integer as a decimal string with
 *        comma thousands separators (e.g. 1234567 becomes "1,234,567").
 *
 * @param value  The value to format.
 * @return string  The formatted string.
 */
string Utility::FormatWithCommas(uint32_t value)
{
    char raw[11];
    snprintf(raw, sizeof(raw), "%" PRIu32, value);

    const int rawLength = static_cast<int>(strlen(raw));
    string result;
    result.reserve(static_cast<size_t>(rawLength + (rawLength - 1) / 3));

    for (int i = 0; i < rawLength; ++i)
    {
        if (i > 0 && ((rawLength - i) % 3) == 0)
        {
            result += ',';
        }
        result += raw[i];
    }

    return (result);
}
