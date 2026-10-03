/*-----------------------------------------------------------------------------
 * File        : Rtc.cpp
 * Description : Implementation of the Rtc singleton for the Epson RX8130CE
 *               real-time clock on the M5Stack Tab5.  The chip is accessed
 *               over I2C using the ESP-IDF v5 new i2c_master driver.  On
 *               first power-up the startup sequence detects an oscillation
 *               stop (VLF flag) and issues a software reset as required by
 *               the datasheet.
 * Author      : Mark Stevens
 * Copyright   : Copyright (c) 2026 Mark Stevens
 * Licence     : MIT — see LICENSE in the repository root for full terms.
 * Target      : M5Stack Tab5 (ESP32-P4)
 * Build system: ESP-IDF v5.5.1
 *---------------------------------------------------------------------------*/

#include "Rtc.hpp"

#include <cstring>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/** @brief Logging tag used for all ESP-IDF log output from this module. */
static constexpr const char *LOG_TAG = "Rtc";

/** @brief I2C transaction timeout in milliseconds. */
static constexpr int I2C_TIMEOUT_MS = 100;

/** @brief Startup wait time in milliseconds (datasheet §8.3: ≥30 ms). */
static constexpr int STARTUP_WAIT_MS = 35;

/** @brief Software reset completion wait time (datasheet: ≥125 ms). */
static constexpr int RESET_WAIT_MS = 130;

/** @brief Control Register 0 bit: stop the timekeeping oscillator. */
static constexpr uint8_t CTRL0_STOP = 0x40;

/** @brief Control Register 0 bit: alarm interrupt enable. */
static constexpr uint8_t CTRL0_AIE = 0x08;

/** @brief Control Register 0 bit: timer interrupt enable. */
static constexpr uint8_t CTRL0_TIE = 0x10;

/** @brief Extension Register bit: timer enable. */
static constexpr uint8_t EXT_TE = 0x10;

/** @brief Extension Register bit: WADA (0=week alarm, 1=day alarm). */
static constexpr uint8_t EXT_WADA = 0x08;

/** @brief Extension Register mask: timer clock source bits [2:0]. */
static constexpr uint8_t EXT_TSEL_MASK = 0x07;

/** @brief Alarm register bit 7: disable this alarm channel. */
static constexpr uint8_t ALARM_DISABLE_BIT = 0x80;

/** @brief Offset from year-2000 in tm_year (years-since-1900) scale. */
static constexpr int TM_YEAR_OFFSET_2000 = 100;

/** @brief Initialise the static singleton pointer to null. */
Rtc *Rtc::_instance = nullptr;

// =============================================================================
// Public static interface
// =============================================================================

/**
 * @brief Returns the existing singleton instance.
 *
 * @return Pointer to the singleton, or nullptr if Initialise() has not
 *         yet been called.
 */
Rtc *Rtc::GetInstance()
{
    return (_instance);
}

/**
 * @brief Creates and initialises the singleton.
 *
 * Opens an I2C master bus on the Tab5 RTC I2C pins, verifies the chip
 * is present, checks the VLF flag, and performs a software reset if
 * the oscillator was stopped.
 *
 * @return Pointer to the newly created singleton, or nullptr if the
 *         singleton already exists or initialisation fails.
 */
Rtc *Rtc::Initialise()
{
    if (_instance != nullptr)
    {
        return (nullptr);
    }

    Rtc *candidate = new Rtc();
    if (!candidate->_initialised)
    {
        delete candidate;
        return (nullptr);
    }

    _instance = candidate;
    return (_instance);
}

// =============================================================================
// Constructor / Destructor
// =============================================================================

/**
 * @brief Private constructor — use Initialise() to create the singleton.
 *
 * Opens the I2C master bus and adds the RX8130CE device.  Throws no
 * exceptions; check GetInstance() != nullptr for success.
 */
Rtc::Rtc() : _busHandle(nullptr), _deviceHandle(nullptr), _initialised(false), _busOwned(false)
{
    // Attempt to reuse the I2C_NUM_1 bus that M5GFX creates during display.init().
    // IDF v5.x rejects a second i2c_new_master_bus() call on the same port number,
    // so we retrieve the existing handle when available.
    esp_err_t result = i2c_master_get_bus_handle(I2C_NUM_1, &_busHandle);
    if (result != ESP_OK)
    {
        // Bus not yet claimed — open it ourselves.
        i2c_master_bus_config_t busConfig = {};
        busConfig.i2c_port = I2C_NUM_1;
        busConfig.sda_io_num = RTC_SDA_PIN;
        busConfig.scl_io_num = RTC_SCL_PIN;
        busConfig.clk_source = I2C_CLK_SRC_DEFAULT;
        busConfig.glitch_ignore_cnt = 7;
        busConfig.flags.enable_internal_pullup = true;

        result = i2c_new_master_bus(&busConfig, &_busHandle);
        if (result != ESP_OK)
        {
            ESP_LOGE(LOG_TAG, "Failed to create I2C master bus: %s", esp_err_to_name(result));
            return;
        }
        _busOwned = true;
    }

    // Add the RX8130CE device to the bus.
    i2c_device_config_t deviceConfig = {};
    deviceConfig.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    deviceConfig.device_address = I2C_ADDRESS;
    deviceConfig.scl_speed_hz = I2C_FREQUENCY_HZ;

    result = i2c_master_bus_add_device(_busHandle, &deviceConfig, &_deviceHandle);
    if (result != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Failed to add RX8130CE device to I2C bus: %s", esp_err_to_name(result));
        i2c_del_master_bus(_busHandle);
        _busHandle = nullptr;
        return;
    }

    _initialised = PerformStartup();
}

/**
 * @brief Destructor.
 *
 * Stops any active alarm or timer interrupt, releases the I2C device
 * handle, deletes the I2C master bus, and resets the singleton pointer
 * so that Initialise() may be called again.
 */
Rtc::~Rtc()
{
    if (_deviceHandle != nullptr)
    {
        // Disable all active interrupts before releasing the hardware.
        EnableAlarmInterrupt(false);
        StopWakeupTimer();

        i2c_master_bus_rm_device(_deviceHandle);
        _deviceHandle = nullptr;
    }

    if (_busHandle != nullptr)
    {
        if (_busOwned)
        {
            i2c_del_master_bus(_busHandle);
        }
        _busHandle = nullptr;
    }

    _instance = nullptr;
}

// =============================================================================
// Time
// =============================================================================

/**
 * @brief Reads the current time from the RTC into a standard tm struct.
 *
 * Year is stored as years since 1900 (matching struct tm convention).
 * Month is 0-based (0 = January).
 *
 * @param[out] time  Struct to fill with the current date and time.
 * @return true on success; false if the I2C transaction failed.
 */
bool Rtc::GetTime(struct tm &time) const
{
    uint8_t buffer[7] = {};
    if (!ReadRegisters(REG_SEC, buffer, sizeof(buffer)))
    {
        return (false);
    }

    time.tm_sec = static_cast<int>(BcdToDec(buffer[0] & 0x7F));
    time.tm_min = static_cast<int>(BcdToDec(buffer[1] & 0x7F));
    time.tm_hour = static_cast<int>(BcdToDec(buffer[2] & 0x3F));

    // Weekday register is one-hot: bit 0 = Sunday, bit 6 = Saturday.
    uint8_t weekdayBit = buffer[3] & 0x7F;
    int weekday = 0;
    while (weekdayBit > 1)
    {
        weekdayBit >>= 1;
        weekday++;
    }
    time.tm_wday = weekday;

    time.tm_mday = static_cast<int>(BcdToDec(buffer[4] & 0x3F));
    time.tm_mon = static_cast<int>(BcdToDec(buffer[5] & 0x1F)) - 1;
    time.tm_year = static_cast<int>(BcdToDec(buffer[6])) + TM_YEAR_OFFSET_2000;
    time.tm_isdst = -1;

    return (true);
}

/**
 * @brief Sets the RTC time from a standard tm struct.
 *
 * Stops the timekeeping oscillator during the write and restarts it
 * immediately after, per the datasheet requirement.
 *
 * @param time  Date and time to write.  Year is years-since-1900;
 *              month is 0-based.
 * @return true on success; false if the I2C transaction failed.
 */
bool Rtc::SetTime(const struct tm &time)
{
    if (!SetStop(true))
    {
        return (false);
    }

    uint8_t buffer[7] = {};
    buffer[0] = DecToBcd(static_cast<uint8_t>(time.tm_sec));
    buffer[1] = DecToBcd(static_cast<uint8_t>(time.tm_min));
    buffer[2] = DecToBcd(static_cast<uint8_t>(time.tm_hour));
    buffer[3] = static_cast<uint8_t>(1u << time.tm_wday); // One-hot weekday
    buffer[4] = DecToBcd(static_cast<uint8_t>(time.tm_mday));
    buffer[5] = DecToBcd(static_cast<uint8_t>(time.tm_mon + 1));
    buffer[6] = DecToBcd(static_cast<uint8_t>(time.tm_year - TM_YEAR_OFFSET_2000));

    // Build the multi-byte write: register address followed by 7 data bytes.
    uint8_t packet[8] = {};
    packet[0] = REG_SEC;
    memcpy(&packet[1], buffer, sizeof(buffer));

    esp_err_t result = i2c_master_transmit(_deviceHandle, packet, sizeof(packet), I2C_TIMEOUT_MS);

    // Restart the oscillator regardless of whether the write succeeded.
    bool restartOk = SetStop(false);

    return (result == ESP_OK && restartOk);
}

// =============================================================================
// Alarm
// =============================================================================

/**
 * @brief Configures the alarm registers.
 *
 * Writes the alarm minute (0x17), hour (0x18), and day/week (0x19)
 * registers and sets the WADA bit in the Extension Register.  Does NOT
 * automatically enable the alarm interrupt — call EnableAlarmInterrupt()
 * separately.
 *
 * @param config  Alarm match configuration.
 * @return true on success.
 */
bool Rtc::SetAlarm(const AlarmConfig &config)
{
    // Update the WADA bit in the Extension Register.
    uint8_t extensionReg = 0;
    if (!ReadRegisters(REG_EXTENSION, &extensionReg, 1))
    {
        return (false);
    }

    if (config.matchWeekday)
    {
        extensionReg &= ~EXT_WADA;
    }
    else
    {
        extensionReg |= EXT_WADA;
    }

    if (!WriteRegister(REG_EXTENSION, extensionReg))
    {
        return (false);
    }

    // Build the three alarm register bytes.  Bit 7 = 0 enables matching for
    // that field; bit 7 = 1 disables it.
    uint8_t alarmMinute = DecToBcd(config.minute);
    uint8_t alarmHour = DecToBcd(config.hour);
    uint8_t alarmDay;

    if (config.matchWeekday)
    {
        alarmDay = static_cast<uint8_t>(1u << config.day); // One-hot encoding
    }
    else
    {
        alarmDay = DecToBcd(config.day);
    }

    if (!config.matchMinute)
    {
        alarmMinute |= ALARM_DISABLE_BIT;
    }
    if (!config.matchHour)
    {
        alarmHour |= ALARM_DISABLE_BIT;
    }
    if (!config.matchDay)
    {
        alarmDay |= ALARM_DISABLE_BIT;
    }

    uint8_t packet[4] = {REG_ALARM_MIN, alarmMinute, alarmHour, alarmDay};

    esp_err_t result = i2c_master_transmit(_deviceHandle, packet, sizeof(packet), I2C_TIMEOUT_MS);
    return (result == ESP_OK);
}

/**
 * @brief Reads the current alarm register settings.
 *
 * @param[out] config  Filled with the current alarm configuration.
 * @return true on success.
 */
bool Rtc::GetAlarm(AlarmConfig &config) const
{
    uint8_t alarmBuffer[3] = {};
    if (!ReadRegisters(REG_ALARM_MIN, alarmBuffer, sizeof(alarmBuffer)))
    {
        return (false);
    }

    uint8_t extensionReg = 0;
    if (!ReadRegisters(REG_EXTENSION, &extensionReg, 1))
    {
        return (false);
    }

    config.matchMinute = (alarmBuffer[0] & ALARM_DISABLE_BIT) == 0;
    config.matchHour = (alarmBuffer[1] & ALARM_DISABLE_BIT) == 0;
    config.matchDay = (alarmBuffer[2] & ALARM_DISABLE_BIT) == 0;
    config.matchWeekday = (extensionReg & EXT_WADA) == 0;

    config.minute = BcdToDec(alarmBuffer[0] & 0x7F);
    config.hour = BcdToDec(alarmBuffer[1] & 0x7F);

    if (config.matchWeekday)
    {
        // Decode one-hot weekday back to 0–6.
        uint8_t weekdayBit = alarmBuffer[2] & 0x7F;
        int weekday = 0;
        while (weekdayBit > 1)
        {
            weekdayBit >>= 1;
            weekday++;
        }
        config.day = static_cast<uint8_t>(weekday);
    }
    else
    {
        config.day = BcdToDec(alarmBuffer[2] & 0x3F);
    }

    return (true);
}

/**
 * @brief Enables or disables the alarm interrupt (AIE bit in Control 0).
 *
 * When enabled, the RX8130CE asserts /IRQ LOW when the alarm time is
 * reached.  Clear the AF flag via ClearFlags() to deassert /IRQ.
 *
 * @param enable  true to enable; false to disable.
 * @return true on success.
 */
bool Rtc::EnableAlarmInterrupt(bool enable)
{
    uint8_t control0 = 0;
    if (!ReadRegisters(REG_CONTROL0, &control0, 1))
    {
        return (false);
    }

    if (enable)
    {
        control0 |= CTRL0_AIE;
    }
    else
    {
        control0 &= ~CTRL0_AIE;
    }

    return (WriteRegister(REG_CONTROL0, control0));
}

// =============================================================================
// Wakeup timer
// =============================================================================

/**
 * @brief Configures and starts the countdown wakeup timer.
 *
 * Writes the 16-bit countdown value to registers 0x1A–0x1B, sets the
 * clock source, enables the timer (TE bit), and optionally enables the
 * timer interrupt (TIE bit).
 *
 * @param countdownValue    Initial countdown value (1–65535).  The time
 *                          until interrupt = countdownValue / clockFrequency.
 * @param clockSource       Clock source that determines tick frequency.
 * @param enableInterrupt   true to assert /IRQ when the counter reaches zero.
 * @return true on success.
 */
bool Rtc::StartWakeupTimer(uint16_t countdownValue, TimerClockSource clockSource, bool enableInterrupt)
{
    // Stop the timer before modifying its registers.
    uint8_t extensionReg = 0;
    if (!ReadRegisters(REG_EXTENSION, &extensionReg, 1))
    {
        return (false);
    }

    extensionReg &= ~EXT_TE;
    if (!WriteRegister(REG_EXTENSION, extensionReg))
    {
        return (false);
    }

    // Write the 16-bit countdown value (little-endian: L byte then H byte).
    const uint8_t timerLow = static_cast<uint8_t>(countdownValue & 0xFF);
    const uint8_t timerHigh = static_cast<uint8_t>((countdownValue >> 8) & 0xFF);
    uint8_t timerPacket[3] = {REG_TIMER_LOW, timerLow, timerHigh};

    esp_err_t result = i2c_master_transmit(_deviceHandle, timerPacket, sizeof(timerPacket), I2C_TIMEOUT_MS);
    if (result != ESP_OK)
    {
        return (false);
    }

    // Set the clock source and re-enable the timer.
    extensionReg &= ~EXT_TSEL_MASK;
    extensionReg |= (static_cast<uint8_t>(clockSource) & EXT_TSEL_MASK);
    extensionReg |= EXT_TE;

    if (!WriteRegister(REG_EXTENSION, extensionReg))
    {
        return (false);
    }

    // Configure the timer interrupt bit in Control Register 0.
    uint8_t control0 = 0;
    if (!ReadRegisters(REG_CONTROL0, &control0, 1))
    {
        return (false);
    }

    if (enableInterrupt)
    {
        control0 |= CTRL0_TIE;
    }
    else
    {
        control0 &= ~CTRL0_TIE;
    }

    return (WriteRegister(REG_CONTROL0, control0));
}

/**
 * @brief Stops the wakeup timer and disables its interrupt.
 *
 * Clears the TE and TIE bits without modifying the countdown value.
 *
 * @return true on success.
 */
bool Rtc::StopWakeupTimer()
{
    uint8_t extensionReg = 0;
    if (!ReadRegisters(REG_EXTENSION, &extensionReg, 1))
    {
        return (false);
    }

    extensionReg &= ~EXT_TE;
    if (!WriteRegister(REG_EXTENSION, extensionReg))
    {
        return (false);
    }

    uint8_t control0 = 0;
    if (!ReadRegisters(REG_CONTROL0, &control0, 1))
    {
        return (false);
    }

    control0 &= ~CTRL0_TIE;
    return (WriteRegister(REG_CONTROL0, control0));
}

// =============================================================================
// Status flags
// =============================================================================

/**
 * @brief Reads the current status flags from Flag Register 0x1D.
 *
 * @return Bitmask of StatusFlags constants; 0 if the read fails.
 */
Rtc::StatusFlags Rtc::GetFlags() const
{
    uint8_t flagReg = 0;
    ReadRegisters(REG_FLAG, &flagReg, 1);
    return (flagReg);
}

/**
 * @brief Clears the specified status flags in Flag Register 0x1D.
 *
 * Performs a read-modify-write so that only the requested bits are
 * cleared.  Must be called after handling an interrupt to deassert /IRQ.
 *
 * @param flags  Bitmask of StatusFlags constants to clear.
 * @return true on success.
 */
bool Rtc::ClearFlags(StatusFlags flags)
{
    uint8_t flagReg = 0;
    if (!ReadRegisters(REG_FLAG, &flagReg, 1))
    {
        return (false);
    }

    flagReg &= ~flags;
    return (WriteRegister(REG_FLAG, flagReg));
}

// =============================================================================
// Private helpers
// =============================================================================

/**
 * @brief Writes one byte to an RX8130CE register.
 *
 * @param registerAddress  Target register address.
 * @param value            Byte to write.
 * @return true on I2C success.
 */
bool Rtc::WriteRegister(uint8_t registerAddress, uint8_t value) const
{
    uint8_t packet[2] = {registerAddress, value};
    esp_err_t result = i2c_master_transmit(_deviceHandle, packet, sizeof(packet), I2C_TIMEOUT_MS);
    return (result == ESP_OK);
}

/**
 * @brief Reads one or more consecutive bytes from the RX8130CE.
 *
 * @param registerAddress  First register address to read.
 * @param buffer           Destination buffer.
 * @param length           Number of bytes to read.
 * @return true on I2C success.
 */
bool Rtc::ReadRegisters(uint8_t registerAddress, uint8_t *buffer, size_t length) const
{
    esp_err_t result = i2c_master_transmit_receive(_deviceHandle, &registerAddress, 1, buffer, length, I2C_TIMEOUT_MS);
    return (result == ESP_OK);
}

/**
 * @brief Converts a BCD-encoded byte to a decimal integer.
 *
 * @param bcd  BCD byte.
 * @return Decimal value.
 */
uint8_t Rtc::BcdToDec(uint8_t bcd)
{
    return (static_cast<uint8_t>(((bcd >> 4) * 10) + (bcd & 0x0F)));
}

/**
 * @brief Converts a decimal integer (0–99) to BCD encoding.
 *
 * @param decimal  Value to convert.
 * @return BCD byte.
 */
uint8_t Rtc::DecToBcd(uint8_t decimal)
{
    return (static_cast<uint8_t>(((decimal / 10) << 4) | (decimal % 10)));
}

/**
 * @brief Sets or clears the STOP bit in Control Register 0.
 *
 * Stopping the oscillator is required before writing time registers.
 *
 * @param stop  true to stop; false to restart.
 * @return true on I2C success.
 */
bool Rtc::SetStop(bool stop)
{
    uint8_t control0 = 0;
    if (!ReadRegisters(REG_CONTROL0, &control0, 1))
    {
        return (false);
    }

    if (stop)
    {
        control0 |= CTRL0_STOP;
    }
    else
    {
        control0 &= ~CTRL0_STOP;
    }

    return (WriteRegister(REG_CONTROL0, control0));
}

/**
 * @brief Performs the power-on initialisation sequence from the datasheet.
 *
 * Waits 30 ms, performs a dummy-read, checks the VLF flag, and issues
 * a software reset if necessary.
 *
 * @return true if the chip is operational after initialisation.
 */
bool Rtc::PerformStartup()
{
    // §8.3: Wait at least 30 ms after power-on before the first access.
    vTaskDelay(pdMS_TO_TICKS(STARTUP_WAIT_MS));

    // Dummy read — ignore ACK/NACK (chip may not yet be ready).
    uint8_t dummy = 0;
    i2c_master_transmit_receive(_deviceHandle, &dummy, 1, &dummy, 1, 1);

    // Read the Flag Register to check the VLF (oscillation-stop) flag.
    uint8_t flagReg = 0;
    if (!ReadRegisters(REG_FLAG, &flagReg, 1))
    {
        ESP_LOGE(LOG_TAG, "Could not read Flag Register — RX8130CE not found on I2C bus");
        return (false);
    }

    if (flagReg & FLAG_VLF)
    {
        ESP_LOGW(LOG_TAG, "VLF flag set — oscillator was stopped.  Performing software reset.");

        // Software reset sequence from the RX8130CE datasheet / application note.
        WriteRegister(REG_CONTROL0, 0x00);
        WriteRegister(REG_CONTROL0, CTRL0_STOP);
        WriteRegister(0x50, 0x6C); // Undocumented internal reset register
        WriteRegister(0x53, 0x01); // Undocumented internal reset register
        WriteRegister(0x66, 0x03); // Undocumented internal reset register
        WriteRegister(0x6B, 0x02); // Undocumented internal reset register
        WriteRegister(0x6B, 0x01); // Undocumented internal reset register

        vTaskDelay(pdMS_TO_TICKS(RESET_WAIT_MS));

        // Clear the VLF flag and restart the oscillator.
        uint8_t clearedFlag = 0;
        if (!ReadRegisters(REG_FLAG, &clearedFlag, 1))
        {
            return (false);
        }
        clearedFlag &= ~FLAG_VLF;
        WriteRegister(REG_FLAG, clearedFlag);
        SetStop(false);

        ESP_LOGW(LOG_TAG, "Software reset complete — please set the time.");
    }

    ESP_LOGI(LOG_TAG, "RX8130CE initialised successfully");
    return (true);
}
