/*-----------------------------------------------------------------------------
 * File        : Utility.hpp
 * Description : General-purpose utility functions for the SSEM emulator,
 *               including numeric formatting helpers.
 * Author      : Mark Stevens
 * Copyright   : Copyright (c) 2026 Mark Stevens
 * Licence     : MIT — see LICENSE in the repository root for full terms.
 * Target      : M5Stack Tab5 (ESP32-P4)
 * Build system: ESP-IDF v5.5.1
 *---------------------------------------------------------------------------*/

#pragma once

#include <cstdint>
#include <string>

using namespace std;

/**
 * @brief General-purpose utility functions for the SSEM emulator.
 *
 * All methods are static; the class is not intended to be instantiated.
 */
class Utility
{
public:
    /**
     * @brief Formats an unsigned 32-bit integer as a decimal string with
     *        comma thousands separators (e.g. 1234567 becomes "1,234,567").
     *
     * @param value  The value to format.
     * @return string  The formatted string.
     */
    static string FormatWithCommas(uint32_t value);
};
