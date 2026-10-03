/*-----------------------------------------------------------------------------
 * File        : Tab5SSEM.cpp
 * Description : Application entry point for the M5Stack Tab5 SSEM emulator
 *               firmware.  Initialises all hardware peripherals and delegates
 *               the display pipeline to the SSEM Display component.
 * Author      : Mark Stevens
 * Copyright   : Copyright (c) 2026 Mark Stevens
 * Licence     : MIT — see LICENSE in the repository root for full terms.
 * Target      : M5Stack Tab5 (ESP32-P4)
 * Build system: ESP-IDF v5.5.1
 *---------------------------------------------------------------------------*/

#include <M5GFX.h>
#include <inttypes.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_timer.h>

#include <cstdio>
#include <string>
#include <string_view>
#include <vector>
#include <memory>

#include <dirent.h>
#include <sys/stat.h>

#include "Display.hpp"
#include "Rtc.hpp"
#include "SDCard.hpp"
#include "Touch.hpp"

#include "CPU.hpp"

#include "StoreLines.hpp"
#include "Compiler.hpp"
#include "Instructions.hpp"
#include "Utility.hpp"

using namespace std;

/**
 * @brief Tag used for logging the main component.
 */
static const char *LOG_TAG = "Tab5SSEM";

/**
 * @brief Global store lines instance used by the SSEM emulator.
 */
static StoreLines _storeLines;

/**
 * @brief Global CPU instance used by the SSEM emulator.
 */
static unique_ptr<Cpu> _cpu = nullptr;

/**
 * @brief Handle of the app_main task; used by OnStopRunPressed to send a run
 *        notification that wakes the execution loop.
 */
static TaskHandle_t _appMainTaskHandle = nullptr;

/**
 * @brief Set to true by OnStopRunPressed(false) to request execution to stop.
 *
 * Read by the execution loop in app_main; written by the touch callback from
 * the TouchTask context.  Declared volatile to prevent compiler optimisation
 * across task boundaries.
 */
static volatile bool _stopRequested = false;

/**
 * @brief Store the speed setting for this run.
 *
 * @note This global does not need protecting from access as it can only be changed when a program
 *       is not running as the controls are disabled when the Run button is pressed.
 */
static Display::SpeedSetting _speedSetting = Display::SpeedSetting::Original;

/**
 * @brief Handle for the 1 ms system timer.
 *
 * Created in Setup() via esp_timer_create() and left in the stopped state
 * until explicitly started via esp_timer_start_periodic().
 */
static esp_timer_handle_t _systemTimerHandle = nullptr;

/**
 * @brief Global display instance used by M5GFX and the SSEM Display component.
 */
M5GFX display;

/**
 * @brief 1 ms system timer callback.
 *
 * Invoked by the esp_timer dispatch task every millisecond once the timer
 * has been started.  Add periodic housekeeping logic here as required.
 *
 * @param arg  User-supplied argument passed to esp_timer_create(); unused.
 */
static void SystemTimerCallback(void *__attribute__((unused)) arg)
{
    if (_appMainTaskHandle != nullptr)
    {
        BaseType_t higherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(_appMainTaskHandle, &higherPriorityTaskWoken);
        portYIELD_FROM_ISR(higherPriorityTaskWoken);
    }
}

/**
 * @brief One-time application setup.
 *
 * Initialises the display, touch input, SD card, and RTC in the order
 * required by the hardware constraints documented in the component headers.
 * Sets an initial time on the RTC, then hands control to the SSEM display
 * pipeline via Display::Run().
 */
void Setup(void)
{
    // Create the 1 ms system timer in the stopped state.  Start it with
    // esp_timer_start_periodic(_systemTimerHandle, 1000) when required.
    const esp_timer_create_args_t timerArgs = {
        .callback = SystemTimerCallback,
        .arg = nullptr,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "system_timer",
        .skip_unhandled_events = false,
    };
    ESP_ERROR_CHECK(esp_timer_create(&timerArgs, &_systemTimerHandle));

    // Tab5 native panel is portrait (720×1280).
    // Rotation 3 = landscape rotated 180 degrees (1280×720).
    display.init();
    display.setBrightness(128);
    display.setRotation(3);

    // display.init() drove GPIO_NUM_23 high to select the ST7123 I2C address.
    // TouchInput can safely reconfigure it as an interrupt input now that
    // init has completed.
    TouchInput::Initialise(display);

    SDCard *sdCard = SDCard::Initialise();
    Rtc *rtc = Rtc::Initialise();

    if (rtc != nullptr)
    {
        struct tm setTime = {};
        setTime.tm_year = 2026 - 1900;
        setTime.tm_mon = 3 - 1; // March (0-based)
        setTime.tm_mday = 21;
        setTime.tm_hour = 14;
        setTime.tm_min = 42;
        setTime.tm_sec = 0;
        setTime.tm_wday = 6; // Saturday
        setTime.tm_isdst = -1;
        rtc->SetTime(setTime);
    }

    Display::Run(display, sdCard);

    // Initialise the SSEM instructions lookup table.
    Instructions::PopulateLookupTable();
}

/**
 * @brief Populate a DisplayMessage with the current store line values and labels.
 *
 * @param message  Message to populate.
 * @param halted   true if the CPU has halted; causes the Display task to restore
 *                 the full stopped UI state.
 */
static void PopulateDisplayMessage(Display::DisplayMessage &message, bool halted)
{
    for (int i = 0; i < Display::STORELINE_COUNT; ++i)
    {
        message.storelineValues[i] = static_cast<uint32_t>(_storeLines[i].GetValue());
        _storeLines[i].Disassemble(message.storelineText[i], sizeof(message.storelineText[i]));
    }

    message.enableStopRun = false;
    message.halted = halted;
}

/**
 * @brief Display the contents of the store lines on the console.
 *
 * @param storeLines Store lines to be displayed.
 */
void UpdateDisplayTube(StoreLines &storeLines)
{
    ESP_LOGI(LOG_TAG, "                   00000000001111111111222222222233");
    ESP_LOGI(LOG_TAG, "                   01234567890123456789012345678901");
    for (uint lineNumber = 0; lineNumber < storeLines.Size(); lineNumber++)
    {
        const string binary = storeLines[lineNumber].Binary();
        const string disassembled = storeLines[lineNumber].Disassemble();
        ESP_LOGI(LOG_TAG, "%4" PRIu32 ": 0x%08" PRIx32 " - %32s %-16s ; %" PRId32, (uint32_t) lineNumber, (uint32_t) storeLines[lineNumber].ReverseBits(), binary.c_str(), disassembled.c_str(), (int32_t) storeLines[lineNumber].GetValue());
    }
}

/**
 * @brief Read the names of all SSEM program files on the SD card.
 *
 * Scans the SD card mount point and returns the full path of every file
 * whose name ends with the ".ssem" extension (case-sensitive).
 *
 * @return vector<string> Vector of full file paths for each
 *         ".ssem" file found.  The vector is empty if the SD card could
 *         not be read or no matching files are present.
 */
vector<string> ReadSdCardFileNames()
{
    ESP_LOGI(LOG_TAG, "SD card contents:");

    static constexpr string_view SSEM_EXTENSION = ".ssem";

    vector<string> filenames;

    DIR *dp = opendir(SDCard::MOUNT_POINT);
    if (dp != nullptr)
    {
        struct dirent *ep;
        while ((ep = readdir(dp)) != nullptr)
        {
            if (ep->d_name[0] == '.')
            {
                continue;
            }

            const string name = ep->d_name;
            if ((name.size() > SSEM_EXTENSION.size()) && name.ends_with(SSEM_EXTENSION))
            {
                string fullPath = string(SDCard::MOUNT_POINT) + "/" + name;
                struct stat fileInfo;
                if (stat(fullPath.c_str(), &fileInfo) == 0)
                {
                    ESP_LOGI(LOG_TAG, "    %s, %" PRIu32, ep->d_name, fileInfo.st_size);
                    filenames.emplace_back(move(fullPath));
                }
                else
                {
                    ESP_LOGE(LOG_TAG, "    %s, stat failed: %s", ep->d_name, strerror(errno));
                }
            }
        }
        closedir(dp);
    }
    else
    {
        ESP_LOGE(LOG_TAG, "Failed to open directory %s", SDCard::MOUNT_POINT);
    }

    return (filenames);
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
vector<string> ReadSdCardFileContents(const string &fullPath)
{
    vector<string> lines;

    auto fileDeleter = [](FILE *f) {
        if (f != nullptr)
            fclose(f);
    };
    unique_ptr<FILE, decltype(fileDeleter)> file(fopen(fullPath.c_str(), "r"), fileDeleter);
    if (file == nullptr)
    {
        ESP_LOGE(LOG_TAG, "Failed to open file %s: %s", fullPath.c_str(), strerror(errno));
        return (lines);
    }

    ESP_LOGI(LOG_TAG, "Reading file %s:", fullPath.c_str());

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
            ESP_LOGI(LOG_TAG, "    %s", line.c_str());
            lines.push_back(move(line));
        }
    }

    return (lines);
}

/**
 * @brief Clear the global storelines and update the display to show the cleared state.
 */
void ClearStoreLinesAndUpdateDisplay()
{
    Display::DisplayMessage message = {};

    for (int i = 0; i < Display::STORELINE_COUNT; ++i)
    {
        message.storelineValues[i] = 0U;
        snprintf(message.storelineText[i], sizeof(message.storelineText[i]), "JMP 0");
    }

    message.enableStopRun = false;
    Display::PostMessage(message);

    _storeLines.Clear();
}

/**
 * @brief Load, compile and display an SSEM program file.
 *
 * Reads the file at the given full path, compiles it into store lines,
 * replaces the global CPU instance, then posts the initial storeline
 * state to the Display task.
 *
 * Called from the Display touch handler when the Load button is pressed.
 *
 * @param fullPath  Full file system path to the .ssem file (e.g. "/sdcard/Add.ssem").
 */
void LoadFile(const string &fullPath)
{
    ESP_LOGI(LOG_TAG, "Loading file: %s", fullPath.c_str());

    const vector<string> fileContents = ReadSdCardFileContents(fullPath);
    if (fileContents.empty())
    {
        ESP_LOGE(LOG_TAG, "File is empty or could not be read: %s", fullPath.c_str());
        return;
    }

    _storeLines = Compiler::Compile(fileContents);

    _cpu = make_unique<Cpu>(_storeLines);
    _cpu->Reset();

    Display::DisplayMessage message = {};
    PopulateDisplayMessage(message, false);
    message.enableStopRun = true;
    Display::PostMessage(message);

    // Strip directory prefix and .ssem extension for the header display name.
    const size_t slashPos = fullPath.rfind('/');
    string programName = (slashPos != string::npos) ? fullPath.substr(slashPos + 1) : fullPath;
    const size_t dotPos = programName.rfind('.');
    if (dotPos != string::npos)
    {
        programName = programName.substr(0, dotPos);
    }
    Display::SetProgramName(programName);

    ESP_LOGI(LOG_TAG, "File loaded: %s", fullPath.c_str());
}

/**
 * @brief Invoked by the Display layer when the Stop/Run button is pressed.
 *
 * Sends a task notification to app_main to start execution when running is
 * true, or sets _stopRequested to interrupt the running execution loop when
 * running is false.
 *
 * @param running  true if the user has requested execution to start;
 *                 false if the user has requested execution to stop.
 */
void OnStopRunPressed(bool running)
{
    if (running)
    {
        ESP_LOGI(LOG_TAG, "Run requested");

        if (_cpu == nullptr)
        {
            ESP_LOGW(LOG_TAG, "Run requested but no program is loaded");
            return;
        }

        _cpu->Reset();
        _stopRequested = false;
        xTaskNotifyGive(_appMainTaskHandle);
    }
    else
    {
        ESP_LOGI(LOG_TAG, "Stop requested");
        _stopRequested = true;
    }
}

/**
 * @brief Title for the message box.
 */
static char _messageBoxTitle[64] = {};

/**
 * @brief Message body for the message box.
 */
static char _messageBoxMessage[256] = {};

/**
 * @brief Main program loop.
 */
extern "C" void app_main(void)
{
    Setup();

    ClearStoreLinesAndUpdateDisplay();

    Display::SetLoadCallback(LoadFile);
    Display::SetStopRunCallback(OnStopRunPressed);

    SDCard *sdCard = SDCard::GetInstance();
    if ((sdCard != nullptr) && sdCard->IsMounted())
    {
        vector<string> filenames = ReadSdCardFileNames();
        Display::SetFiles(filenames);
    }

    // Save this task handle so OnStopRunPressed can send a run and single step notification.
    _appMainTaskHandle = xTaskGetCurrentTaskHandle();

    //
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        //
        //  At this point the UI has sent a message to run the application.
        //
        _speedSetting = Display::GetSpeed();
        uint32_t instructionCount = 0;
        struct timespec start;
        clock_gettime(CLOCK_REALTIME, &start);

        if (_speedSetting == Display::SpeedSetting::Original)
        {
            esp_timer_start_periodic(_systemTimerHandle, 1000);
        }

        uint32_t lastFooterUpdateCount = 0;
        int64_t lastFooterUpdateUs = esp_timer_get_time();

        ESP_LOGI(LOG_TAG, "Program loaded, starting execution");
        UpdateDisplayTube(_storeLines);

        // Force an initial update to the display.
        _storeLines.SetDirty(true);

        while (!_stopRequested && !_cpu->IsStopped())
        {
            //
            //  If we are using the original speed setting then we wait for the timer ISR to notify the
            //  task that we are OK to proceed with the next instruction.
            //
            if (_speedSetting == Display::SpeedSetting::Original)
            {
                ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
            }
            instructionCount++;
            _cpu->SingleStep();

            if (_storeLines.IsDirty())
            {
                Display::DisplayMessage message = {};
                PopulateDisplayMessage(message, false);
                Display::PostMessage(message);
                _storeLines.SetDirty(false);
            }

            if ((instructionCount - lastFooterUpdateCount) >= 1000)
            {
                const int64_t nowUs = esp_timer_get_time();
                if ((nowUs - lastFooterUpdateUs) >= 1'000'000LL)
                {
                    struct timespec now;
                    clock_gettime(CLOCK_REALTIME, &now);
                    const double elapsed = (now.tv_sec - start.tv_sec) + (now.tv_nsec - start.tv_nsec) / 1e9;
                    Display::UpdateFooter(instructionCount, elapsed);
                    lastFooterUpdateCount = instructionCount;
                    lastFooterUpdateUs = nowUs;
                }
            }
        }

        esp_timer_stop(_systemTimerHandle); // Stop the timer if it is running, we don't worry if it isn't and we discard the result.
        struct timespec end;
        clock_gettime(CLOCK_REALTIME, &end);
        double elapsedTime = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        ESP_LOGI(LOG_TAG, "Program execution completed, Elapsed time=%.2f seconds", elapsedTime);
        ESP_LOGI(LOG_TAG, "CPU executed %s instructions.", Utility::FormatWithCommas(instructionCount).c_str());

        Display::UpdateFooter(instructionCount, elapsedTime);
        UpdateDisplayTube(_storeLines);

        snprintf(_messageBoxTitle, sizeof(_messageBoxTitle), "Execution Complete");
        snprintf(_messageBoxMessage, sizeof(_messageBoxMessage), "CPU executed %s instructions.\n\nElapsed time: %.2f seconds.", Utility::FormatWithCommas(instructionCount).c_str(), elapsedTime);
        Display::ShowMessageDialog(_messageBoxTitle, _messageBoxMessage);

        Display::SetRunning(false);
        Display::SetSpeedEnabled(true);
        Display::SetLoadEnabled(true);
    }
}
