/*-----------------------------------------------------------------------------
 * File        : Rtc.hpp
 * Description : Singleton class (Rtc) for the Epson RX8130CE real-time clock
 *               on the M5Stack Tab5.  Provides time get/set, three alarm
 *               channels (minute, hour, day/week), and wakeup-timer interrupt
 *               support via the chip's /IRQ output and a configurable GPIO.
 * Author      : Mark Stevens
 * Copyright   : Copyright (c) 2026 Mark Stevens
 * Licence     : MIT — see LICENSE in the repository root for full terms.
 * Target      : M5Stack Tab5 (ESP32-P4)
 * Build system: ESP-IDF v5.5.1
 *---------------------------------------------------------------------------*/

#pragma once

#include <cstdint>
#include <ctime>
#include <driver/gpio.h>
#include <driver/i2c_master.h>

/**
 * @brief Singleton driver for the Epson RX8130CE RTC on the M5Stack Tab5.
 *
 * The RX8130CE communicates over I2C at address 0x32.  This class exposes:
 *
 *  - Time read/write using the standard C `struct tm`.
 *  - Day/date alarm with independent enable bits for minute, hour, and day.
 *  - Wakeup-timer countdown interrupt at selectable clock sources.
 *  - Interrupt flag inspection and clearing.
 *
 * Interrupt wakeup from deep-sleep is routed via the PMS150G-U06 companion
 * MCU from the RX8130CE /IRQ pin.  The caller is responsible for configuring
 * the ESP32-P4 deep-sleep wakeup source via esp_sleep_enable_ext0_wakeup()
 * after the alarm or timer has been configured through this class.
 *
 * @note Only one instance may exist at a time.  Calling Initialise() when the
 *       singleton already exists returns nullptr.
 *
 * @note The first call to Initialise() performs a dummy-read, checks the
 *       VLF (oscillation-stop) flag, and issues a software reset if the
 *       oscillator was stopped.  The application must set the correct time
 *       after any power-on reset or software reset.
 */
class Rtc
{
public:
    // =========================================================================
    // Types
    // =========================================================================

    /**
     * @brief Alarm match fields.
     *
     * Each field is independently enabled or disabled.  Unset fields are
     * ignored by the hardware.  The WADA bit in the Extension Register selects
     * whether the Day field matches the day-of-month or day-of-week.
     */
    struct AlarmConfig
    {
        /** @brief Enable matching on the minute field. */
        bool matchMinute = false;

        /** @brief Enable matching on the hour field. */
        bool matchHour = false;

        /** @brief Enable matching on the day field (day-of-month or weekday). */
        bool matchDay = false;

        /** @brief Minute value to match (0–59). */
        uint8_t minute = 0;

        /** @brief Hour value to match (0–23). */
        uint8_t hour = 0;

        /**
         * @brief Day value to match.
         *
         * When matchWeekday is false: day-of-month 1–31.
         * When matchWeekday is true: weekday 0 (Sunday) – 6 (Saturday).
         */
        uint8_t day = 0;

        /**
         * @brief Select day-of-week matching instead of day-of-month.
         *
         * Sets the WADA bit in the Extension Register.  When true the day
         * register stores a weekday bitmask (bit 0 = Sunday … bit 6 = Saturday).
         */
        bool matchWeekday = false;
    };

    /**
     * @brief Wakeup-timer clock source selection (TSEL bits).
     */
    enum class TimerClockSource : uint8_t {
        /** @brief 4096 Hz — smallest period, highest resolution. */
        Hz4096 = 0x00,

        /** @brief 64 Hz. */
        Hz64 = 0x01,

        /** @brief 1 Hz — one second per count. */
        Hz1 = 0x02,

        /** @brief 1/60 Hz — one minute per count. */
        Per60Seconds = 0x03,

        /** @brief 1/3600 Hz — one hour per count (≈0.000278 Hz). */
        Per3600Seconds = 0x04,
    };

    /**
     * @brief Status flags from Flag Register (0x1D).
     *
     * Bits are ORed together; use the individual flag constants to test.
     */
    using StatusFlags = uint8_t;

    /** @brief VBLF — VBAT low-voltage detection. */
    static constexpr StatusFlags FLAG_VBAT_LOW = 0x80;

    /** @brief UF — time-update interrupt fired. */
    static constexpr StatusFlags FLAG_UPDATE = 0x20;

    /** @brief TF — wakeup-timer interrupt fired. */
    static constexpr StatusFlags FLAG_TIMER = 0x10;

    /** @brief AF — alarm interrupt fired. */
    static constexpr StatusFlags FLAG_ALARM = 0x08;

    /** @brief RSF — voltage-drop detected (/RST asserted). */
    static constexpr StatusFlags FLAG_RESET = 0x04;

    /** @brief VLF — oscillation-stop detected. */
    static constexpr StatusFlags FLAG_VLF = 0x02;

    /** @brief VBFF — VBAT fully charged. */
    static constexpr StatusFlags FLAG_VBAT_FULL = 0x01;

    // =========================================================================
    // Singleton interface
    // =========================================================================

    static Rtc *GetInstance();
    static Rtc *Initialise();
    ~Rtc();

    // =========================================================================
    // Time
    // =========================================================================

    bool GetTime(struct tm &time) const;
    bool SetTime(const struct tm &time);

    // =========================================================================
    // Alarm
    // =========================================================================

    bool SetAlarm(const AlarmConfig &config);
    bool GetAlarm(AlarmConfig &config) const;
    bool EnableAlarmInterrupt(bool enable);

    // =========================================================================
    // Wakeup timer
    // =========================================================================

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
    bool StartWakeupTimer(uint16_t countdownValue, TimerClockSource clockSource, bool enableInterrupt);
    bool StopWakeupTimer();

    // =========================================================================
    // Status flags
    // =========================================================================

    StatusFlags GetFlags() const;
    bool ClearFlags(StatusFlags flags);

private:
    // =========================================================================
    // Hardware constants
    // =========================================================================

    /** @brief RX8130CE 7-bit I2C address. */
    static constexpr uint8_t I2C_ADDRESS = 0x32;

    /** @brief I2C clock frequency for RTC communication. */
    static constexpr uint32_t I2C_FREQUENCY_HZ = 400000;

    /** @brief I2C SDA pin — shared internal bus, GPIO 31. */
    static constexpr gpio_num_t RTC_SDA_PIN = GPIO_NUM_31;

    /** @brief I2C SCL pin — shared internal bus, GPIO 32. */
    static constexpr gpio_num_t RTC_SCL_PIN = GPIO_NUM_32;

    // =========================================================================
    // Register addresses
    // =========================================================================

    /** @brief First time register (seconds); 7 consecutive bytes through 0x16. */
    static constexpr uint8_t REG_SEC = 0x10;

    /** @brief Alarm minute register. */
    static constexpr uint8_t REG_ALARM_MIN = 0x17;

    /** @brief Wakeup timer low byte. */
    static constexpr uint8_t REG_TIMER_LOW = 0x1A;

    /** @brief Extension Register (FSEL, USEL, TE, WADA, TSEL). */
    static constexpr uint8_t REG_EXTENSION = 0x1C;

    /** @brief Flag Register (VBLF, UF, TF, AF, RSF, VLF, VBFF). */
    static constexpr uint8_t REG_FLAG = 0x1D;

    /** @brief Control Register 0 (TEST, STOP, UIE, TIE, AIE, TSTP, TBKON, TBKE). */
    static constexpr uint8_t REG_CONTROL0 = 0x1E;

    /** @brief Control Register 1 (SMPTSEL, CHGEN, INIEN, RSVSEL, BFVSEL). */
    static constexpr uint8_t REG_CONTROL1 = 0x1F;

    /** @brief Digital offset register. */
    static constexpr uint8_t REG_OFFSET = 0x30;

    // =========================================================================
    // Constructor and helpers
    // =========================================================================

    Rtc();
    bool WriteRegister(uint8_t registerAddress, uint8_t value) const;
    bool ReadRegisters(uint8_t registerAddress, uint8_t *buffer, size_t length) const;
    static uint8_t BcdToDec(uint8_t bcd);
    static uint8_t DecToBcd(uint8_t decimal);
    bool SetStop(bool stop);
    bool PerformStartup();

    // =========================================================================
    // Members
    // =========================================================================

    /** @brief The single instance of this class. */
    static Rtc *_instance;

    /** @brief I2C master bus handle. */
    i2c_master_bus_handle_t _busHandle;

    /** @brief I2C device handle for the RX8130CE. */
    i2c_master_dev_handle_t _deviceHandle;

    /** @brief True once the chip has been successfully initialised. */
    bool _initialised;

    /** @brief True if this instance created the I2C bus and must delete it on destruction. */
    bool _busOwned;
};
