/*-----------------------------------------------------------------------------
 * File        : SDCard.cpp
 * Description : Implementation of the SDCard singleton.  Configures the
 *               ESP32-P4 on-chip LDO (channel 4), mounts the microSD card
 *               via SDMMC 4-bit mode on the Tab5 dedicated GPIO pins, and
 *               registers a FAT filesystem at "/sdcard".
 * Author      : Mark Stevens
 * Copyright   : Copyright (c) 2026 Mark Stevens
 * Licence     : MIT — see LICENSE in the repository root for full terms.
 * Target      : M5Stack Tab5 (ESP32-P4)
 * Build system: ESP-IDF v5.5.1
 *---------------------------------------------------------------------------*/

#include "SDCard.hpp"

#include <cstring>
#include <inttypes.h>
#include <esp_log.h>
#include <esp_ldo_regulator.h>

/** @brief Logging tag used for all ESP-IDF log output from this module. */
static constexpr const char *LOG_TAG = "SDCard";

/** @brief Initialise the static singleton pointer to null. */
SDCard *SDCard::_instance = nullptr;

// -----------------------------------------------------------------------------
// Public static interface
// -----------------------------------------------------------------------------

/**
 * @brief Returns the existing singleton instance.
 *
 * @return Pointer to the singleton, or nullptr if Initialise() has not
 *         yet been called.
 */
SDCard *SDCard::GetInstance()
{
    return (_instance);
}

/**
 * @brief Creates and initialises the singleton.
 *
 * Acquires the on-chip LDO (channel 4), mounts the SD card via the
 * SDMMC host in 4-bit mode, and registers the FAT filesystem at
 * MOUNT_POINT.  If no card is present, or if the mount fails, the
 * singleton is still created but IsMounted() returns false.
 *
 * @return Pointer to the newly created singleton, or nullptr if the
 *         singleton already exists.
 */
SDCard *SDCard::Initialise()
{
    if (_instance != nullptr)
    {
        return (nullptr);
    }

    _instance = new SDCard();
    return (_instance);
}

// -----------------------------------------------------------------------------
// Constructor / Destructor
// -----------------------------------------------------------------------------

/**
 * @brief Private constructor — use Initialise() to create the singleton.
 *
 * Acquires the on-chip LDO, configures the SDMMC host and slot, and
 * attempts to mount the FAT filesystem.  Sets _mounted accordingly.
 */
SDCard::SDCard() : _card(nullptr), _powerControlHandle(nullptr), _mounted(false)
{
    // Acquire the on-chip LDO that powers the SD card slot (channel 4 on Tab5).
    // The LDO output voltage for the SD card slot is 3.3 V.
    esp_ldo_channel_config_t ldoConfig = {};
    ldoConfig.chan_id = SD_LDO_CHANNEL;
    ldoConfig.voltage_mv = 3300;

    esp_err_t result = esp_ldo_acquire_channel(&ldoConfig, &_powerControlHandle);
    if (result != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Failed to acquire on-chip LDO channel %" PRId32 ": %s", (int32_t) SD_LDO_CHANNEL, esp_err_to_name(result));
        return;
    }

    // Configure the SDMMC host for 4-bit mode at high speed (40 MHz).
    // pwr_ctrl_handle is left as nullptr because the LDO is managed manually
    // above; the channel stays enabled for the lifetime of the singleton.
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.slot = SDMMC_HOST_SLOT_0;
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;

    // Configure the SDMMC slot with the Tab5 GPIO pin assignments.
    sdmmc_slot_config_t slot;
    memset(&slot, 0, sizeof(slot));
    slot.clk = SD_CLK;
    slot.cmd = SD_CMD;
    slot.d0 = SD_D0;
    slot.d1 = SD_D1;
    slot.d2 = SD_D2;
    slot.d3 = SD_D3;
    slot.cd = SDMMC_SLOT_NO_CD;
    slot.wp = SDMMC_SLOT_NO_WP;
    slot.width = 4;
    slot.flags = 0;

    // Configure the FAT filesystem mount options.
    esp_vfs_fat_sdmmc_mount_config_t mountConfig = {};
    mountConfig.format_if_mount_failed = false;
    mountConfig.max_files = MAX_OPEN_FILES;
    mountConfig.allocation_unit_size = ALLOCATION_UNIT_BYTES;

    result = esp_vfs_fat_sdmmc_mount(MOUNT_POINT, &host, &slot, &mountConfig, &_card);
    if (result != ESP_OK)
    {
        if (result == ESP_FAIL)
        {
            ESP_LOGE(
                LOG_TAG, "Failed to mount FAT filesystem — card may be "
                         "unformatted or corrupted.");
        }
        else
        {
            ESP_LOGE(
                LOG_TAG,
                "Failed to initialise SD card (%s). "
                "Check that a card is inserted.",
                esp_err_to_name(result));
        }

        // Release the LDO now that it will not be needed.
        esp_ldo_release_channel(_powerControlHandle);
        _powerControlHandle = nullptr;
        return;
    }

    _mounted = true;

    const uint64_t totalBytes = static_cast<uint64_t>(_card->csd.capacity) * static_cast<uint64_t>(_card->csd.sector_size);
    const double sizeGb = static_cast<double>(totalBytes) / (1024.0 * 1024.0 * 1024.0);

    ESP_LOGI(LOG_TAG, "SD card mounted at %s - %s  %.2f GB", MOUNT_POINT, _card->cid.name, sizeGb);
}

/**
 * @brief Destructor.
 *
 * Unmounts the FAT filesystem, releases the SDMMC host, and deletes
 * the on-chip LDO power controller.  Resets the singleton pointer so
 * that Initialise() may be called again.
 */
SDCard::~SDCard()
{
    if (_mounted)
    {
        esp_vfs_fat_sdcard_unmount(MOUNT_POINT, _card);
        _card = nullptr;
        _mounted = false;
    }

    if (_powerControlHandle != nullptr)
    {
        esp_ldo_release_channel(_powerControlHandle);
        _powerControlHandle = nullptr;
    }

    _instance = nullptr;
}

// -----------------------------------------------------------------------------
// Public accessors
// -----------------------------------------------------------------------------

/**
 * @brief Returns whether the card was successfully mounted.
 *
 * @return true if the card is mounted and file I/O is available.
 */
bool SDCard::IsMounted() const
{
    return (_mounted);
}

/**
 * @brief Returns a pointer to the raw SDMMC card descriptor.
 *
 * The descriptor contains card identification data (name, revision,
 * serial number) and capacity information.
 *
 * @return Pointer to sdmmc_card_t, or nullptr if the card is not mounted.
 */
const sdmmc_card_t *SDCard::GetCard() const
{
    return (_card);
}
