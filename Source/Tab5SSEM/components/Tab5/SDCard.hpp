/*-----------------------------------------------------------------------------
 * File        : SDCard.hpp
 * Description : Singleton class (SDCard) for mounting and accessing the
 *               microSD card slot on the M5Stack Tab5.  Uses the ESP-IDF
 *               SDMMC host driver in 4-bit mode with on-chip LDO power
 *               control.
 * Author      : Mark Stevens
 * Copyright   : Copyright (c) 2026 Mark Stevens
 * Licence     : MIT — see LICENSE in the repository root for full terms.
 * Target      : M5Stack Tab5 (ESP32-P4)
 * Build system: ESP-IDF v5.5.1
 *---------------------------------------------------------------------------*/

#pragma once

#include <driver/gpio.h>
#include <driver/sdmmc_host.h>
#include <esp_ldo_regulator.h>
#include <esp_vfs_fat.h>
#include <sdmmc_cmd.h>

/**
 * @brief Singleton for the M5Stack Tab5 microSD card slot.
 *
 * Initialises the on-chip LDO (channel 4) that powers the SD card slot,
 * configures the SDMMC host in 4-bit mode on the Tab5's dedicated GPIO
 * pins, and mounts a FAT filesystem to the virtual file system at
 * MOUNT_POINT ("/sdcard").  After a successful Initialise() call the
 * standard C file I/O functions (fopen, fread, fwrite, etc.) may be used
 * with paths prefixed by "/sdcard/".
 *
 * @note Only one instance may exist at a time.  Calling Initialise() when
 *       the singleton already exists returns nullptr.
 *
 * @note If no card is inserted, Initialise() still returns a valid pointer
 *       but IsMounted() returns false.  The caller must check IsMounted()
 *       before attempting any file operations.
 */
class SDCard
{
public:
    static SDCard *GetInstance();
    static SDCard *Initialise();
    ~SDCard();
    bool IsMounted() const;
    const sdmmc_card_t *GetCard() const;

    /**
     * @brief VFS path prefix under which the SD card filesystem is mounted.
     */
    static constexpr const char *MOUNT_POINT = "/sdcard";

private:
    SDCard();

    /**
     * @brief Singleton instance pointer.
     */
    static SDCard *_instance;

    // -------------------------------------------------------------------------
    // Tab5 microSD card GPIO assignments.
    // Source: M5Stack Tab5 schematics / Espressif BSP (m5stack_tab5).
    // -------------------------------------------------------------------------

    /**
     * @brief SDMMC clock output.
     */
    static constexpr gpio_num_t SD_CLK = GPIO_NUM_43;

    /**
     * @brief SDMMC command / response line.
     */
    static constexpr gpio_num_t SD_CMD = GPIO_NUM_44;

    /**
     * @brief SDMMC data line 0 (used in both 1-bit and 4-bit modes).
     */
    static constexpr gpio_num_t SD_D0 = GPIO_NUM_39;

    /**
     * @brief SDMMC data line 1 (4-bit mode only).
     */
    static constexpr gpio_num_t SD_D1 = GPIO_NUM_40;

    /**
     * @brief SDMMC data line 2 (4-bit mode only).
     */
    static constexpr gpio_num_t SD_D2 = GPIO_NUM_41;

    /**
     * @brief SDMMC data line 3 / SPI chip-select (4-bit mode only).
     */
    static constexpr gpio_num_t SD_D3 = GPIO_NUM_42;

    /**
     * @brief On-chip LDO channel number that powers the SD card slot.
     */
    static constexpr int SD_LDO_CHANNEL = 4;

    /**
     * @brief Maximum number of files that may be open simultaneously.
     */
    static constexpr int MAX_OPEN_FILES = 5;

    /**
     * @brief FAT allocation unit size in bytes (16 KB).
     */
    static constexpr size_t ALLOCATION_UNIT_BYTES = 16 * 1024;

    /**
     * @brief SDMMC card descriptor populated by esp_vfs_fat_sdmmc_mount().
     */
    sdmmc_card_t *_card;

    /**
     * @brief Handle for the on-chip LDO channel that powers the SD slot.
     */
    esp_ldo_channel_handle_t _powerControlHandle;

    /**
     *  @brief True when the card is mounted and the filesystem is available.
     */
    bool _mounted;
};
