/*-----------------------------------------------------------------------------
 * File        : Display.hpp
 * Description : SSEM emulator display controller for the M5Stack Tab5.
 *               Manages the splash screen, main interface header, footer,
 *               storeline LED grid, and storeline text labels.
 * Author      : Mark Stevens
 * Copyright   : Copyright (c) 2026 Mark Stevens
 * Licence     : MIT — see LICENSE in the repository root for full terms.
 * Target      : M5Stack Tab5 (ESP32-P4)
 * Build system: ESP-IDF v5.5.1
 *---------------------------------------------------------------------------*/

#pragma once

#include <M5GFX.h>
#include "SDCard.hpp"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <functional>
#include <string>
#include <vector>

/**
 * @brief SSEM emulator display controller.
 *
 * Static class that manages the entire display pipeline for the SSEM
 * Manchester Baby emulator.  Owns the splash screen lifecycle, the main
 * interface layout (header, footer, storeline LED grid, storeline labels,
 * and control panel), and the associated touch callbacks.
 *
 * All state is held in static members so that registered touch callbacks
 * (which are plain function pointers) can reach the display context without
 * requiring a global variable outside this class.
 *
 * @note Run() must be called after TouchInput::Initialise() so that
 *       AddCallback() / RemoveCallback() are available for splash dismissal.
 */
class Display
{
public:
    /**
     * @brief Number of storelines in the SSEM store.
     */
    static constexpr int STORELINE_COUNT = 32;

    /**
     * @brief Selectable SSEM execution speed mode.
     */
    enum class SpeedSetting {
        /**
         * @brief Run as fast as the hardware allows.
         */
        Maximum,
        /**
         * @brief Simulate original 1948 Manchester Baby clock rate.
         */
        Original
    };

    /**
     * @brief Callback type invoked when the Stop/Run button is pressed.
     *
     * The bool argument is the new intended running state:
     * true = the user wants to start execution, false = stop.
     */
    using StopRunCallback = std::function<void(bool running)>;

    /**
     * @brief Callback type invoked when the Load button is pressed.
     *
     * The string argument is the full path of the currently selected file.
     */
    using LoadCallback = std::function<void(const std::string &filename)>;

    /**
     * @brief Message sent to the Display task via the message queue.
     *
     * Holds a complete snapshot of the display state.  The Display task
     * dequeues one of these each cycle and redraws the storeline area.
     */
    struct DisplayMessage
    {
        /**
         * @brief Current value of each storeline word.
         */
        uint32_t storelineValues[STORELINE_COUNT];

        /**
         * @brief Text label shown beside each storeline.
         */
        char storelineText[STORELINE_COUNT][32];

        /**
         * @brief When true, signals that the Stop/Run button should be enabled.
         */
        bool enableStopRun;

        /**
         * @brief true when the CPU has halted; causes the display to restore the
         *        full stopped UI state (indicator, speed section, file list, buttons).
         */
        bool halted;
    };

    static void Run(M5GFX &display, SDCard *sdCard);
    static bool PostMessage(const DisplayMessage &message);

    static void SetRunning(bool running);
    static void SetFiles(const std::vector<std::string> &files);
    static void SetLoadEnabled(bool enabled);
    static void SetStopRunEnabled(bool enabled);
    static void SetSpeedEnabled(bool enabled);
    static void SetProgramName(const std::string &name);
    static void UpdateFooter(uint32_t instructionCount, double elapsedSeconds);

    static SpeedSetting GetSpeed();

    static void SetStopRunCallback(StopRunCallback callback);
    static void SetLoadCallback(LoadCallback callback);

    static void ShowMessageDialog(const char *title, const char *message);

private:
    Display() = delete;

    /**
     * @brief RAII scope guard that acquires _displayMutex on construction and
     *        releases it on destruction.
     *
     * Ensures the mutex is always released even if an early return is added to
     * a guarded method in the future.
     */
    class MutexGuard
    {
    public:
        /**
         * @brief Acquires the mutex; blocks until it is available.
         *
         * @param mutex  FreeRTOS semaphore handle to acquire.
         */
        explicit MutexGuard(SemaphoreHandle_t mutex) : _mutex(mutex)
        {
            xSemaphoreTake(_mutex, portMAX_DELAY);
        }

        /**
         * @brief Releases the mutex.
         */
        ~MutexGuard()
        {
            xSemaphoreGive(_mutex);
        }

        MutexGuard(const MutexGuard &) = delete;
        MutexGuard &operator=(const MutexGuard &) = delete;

    private:
        /**
         * @brief The FreeRTOS semaphore handle being guarded.
         */
        SemaphoreHandle_t _mutex;
    };

    static void ShowSplash(SDCard *sdCard);
    static void ShowMain();
    static void DrawHeader();
    static void DrawFooter();

    // Storeline grid methods
    static void DrawAllStorelines();
    static void DrawStoreline(int lineIndex);
    static void DrawLed(int centreX, int centreY, int radius, bool on);

    // Control panel methods
    static void DrawControlPanel();
    static void DrawRunningIndicator();
    static void DrawSpeedSection();
    static void DrawFileList();
    static void DrawActionButtons();
    static void DrawButton(int x, int y, int width, int height, const char *label, bool enabled);

    // Touch handling
    static void OnSplashTouch(const lgfx::touch_point_t *points, int count);
    static void OnPanelTouch(const lgfx::touch_point_t *points, int count);
    static void HandlePress(int touchX, int touchY);
    static bool HitTest(int touchX, int touchY, int x, int y, int width, int height);

    static std::string Basename(const std::string &path);

    /**
     * @brief Pointer to the global M5GFX display instance; assigned by Run().
     */
    static M5GFX *_display;

    /**
     * @brief Flagged true by OnSplashTouch to dismiss the splash early.
     */
    static volatile bool _splashDismissed;

    /**
     * @brief Flags indicating which storelines have changed.
     */
    static bool _changed[Display::STORELINE_COUNT];

    /**
     * @brief SSEM store — 32 unsigned 32-bit words, all zero initially (JP 0).
     */
    static uint32_t _store[Display::STORELINE_COUNT];

    /**
     * @brief Text label shown to the right of each storeline, initially "JP 0".
     */
    static char _labels[Display::STORELINE_COUNT][32];

    // Control panel state
    static bool _running;
    static SpeedSetting _speedSetting;
    static std::vector<std::string> _files;
    static int _selectedFile;
    static int _scrollOffset;
    static bool _loadEnabled;
    static bool _stopRunEnabled;

    /**
     * @brief true when the speed radio buttons are enabled.
     *
     * Set to false while the SSEM CPU is executing so the speed cannot
     * be changed mid-run; restored to true when execution stops.
     */
    static bool _speedEnabled;
    static bool _prevTouched;
    static StopRunCallback _stopRunCallback;
    static LoadCallback _loadCallback;

    /**
     * @brief Name of the currently loaded program; empty when none is loaded.
     *
     * Displayed in the header enclosed in brackets when non-empty.
     */
    static std::string _loadedProgram;

    /**
     * @brief Instruction count shown in the footer during and after execution.
     */
    static uint32_t _instructionCount;

    /**
     * @brief Elapsed execution time in seconds shown in the footer.
     */
    static double _elapsedSeconds;

    /**
     * @brief Number of LEDs per storeline (one per bit).
     */
    static constexpr int LED_COUNT = 32;

    /**
     * @brief Height of the header banner in pixels.
     */
    static constexpr int HEADER_HEIGHT = 32;

    /**
     * @brief Height of the footer bar in pixels.
     */
    static constexpr int FOOTER_HEIGHT = 24;

    /**
     * @brief Width and height of each LED cell in pixels (square cells).
     */
    static constexpr int LED_CELL_WIDTH = 20;

    /**
     * @brief Number of LEDs per visual group, separated by a small gap.
     */
    static constexpr int LED_GROUP_SIZE = 4;

    /**
     * @brief Gap in pixels inserted between adjacent LED groups.
     */
    static constexpr int LED_GROUP_GAP = 8;

    /**
     * @brief Number of LED groups per storeline.
     */
    static constexpr int LED_GROUP_COUNT = LED_COUNT / LED_GROUP_SIZE;

    /**
     * @brief Total pixel width of the LED grid section including inter-group gaps.
     */
    static constexpr int LED_SECTION_WIDTH = (LED_CELL_WIDTH * LED_COUNT) + ((LED_GROUP_COUNT - 1) * LED_GROUP_GAP);

    /**
     * @brief Width in pixels reserved to the left of the LED box for the per-row
     *        storeline index number.  The number is drawn outside the LED box.
     */
    static constexpr int STORELINE_NUMBER_WIDTH = 28;

    /**
     * @brief X co-ordinate of the left edge of the LED section (offset from the
     *        left of the screen by the storeline number column).
     */
    static constexpr int LED_SECTION_X = STORELINE_NUMBER_WIDTH;

    /**
     * @brief X co-ordinate of the left edge of the storeline text section.
     */
    static constexpr int TEXT_SECTION_X = LED_SECTION_X + LED_SECTION_WIDTH;

    /**
     * @brief Pixel width of the storeline text section.
     */
    static constexpr int TEXT_SECTION_WIDTH = 200;

    /**
     * @brief X co-ordinate of the left edge of the control panel section.
     */
    static constexpr int CONTROL_SECTION_X = TEXT_SECTION_X + TEXT_SECTION_WIDTH;

    /**
     * @brief Outer radius of each LED circle in pixels.
     */
    static constexpr int LED_OUTER_RADIUS = 9;

    /**
     * @brief Inner fill radius of each LED circle in pixels (leaves a white border).
     */
    static constexpr int LED_INNER_RADIUS = 7;

    /**
     * @brief Maximum time the splash is displayed before auto-dismissal (milliseconds).
     */
    static constexpr int SPLASH_TIMEOUT_MS = 5000;

    /**
     * @brief Padding in pixels between a white border box and its enclosed content.
     */
    static constexpr int BOX_PADDING = 4;

    /**
     * @brief Left margin of the text section in pixels, providing separation between
     *        the LED column and the storeline text.
     */
    static constexpr int TEXT_LEFT_MARGIN = 16;

    /**
     * @brief Width in pixels reserved for the zero-padded 8-digit hex value column
     *        that appears inside the text box, to the left of the mnemonic label.
     */
    static constexpr int HEX_COLUMN_WIDTH = 72;

    // Control panel layout constants
    static constexpr int PANEL_PADDING = 8;
    static constexpr int PANEL_X = CONTROL_SECTION_X + PANEL_PADDING;
    static constexpr int DISPLAY_WIDTH = 1280;
    static constexpr int PANEL_WIDTH = DISPLAY_WIDTH - CONTROL_SECTION_X - (2 * PANEL_PADDING);
    static constexpr int DISPLAY_HEIGHT = 720;
    static constexpr int PANEL_Y = HEADER_HEIGHT + PANEL_PADDING;
    static constexpr int PANEL_BOTTOM = DISPLAY_HEIGHT - FOOTER_HEIGHT - PANEL_PADDING;
    static constexpr int INDICATOR_HEIGHT = 48;
    static constexpr int SPEED_HEIGHT = 56;
    static constexpr int FILES_LABEL_HEIGHT = 22;
    static constexpr int FILES_LABEL_GAP = 4;
    static constexpr int BUTTON_HEIGHT = 44;
    static constexpr int BUTTON_GAP = 4;
    static constexpr int SECTION_GAP = 8;
    static constexpr int LIST_ITEM_HEIGHT = 26;
    static constexpr int SCROLL_ARROW_HEIGHT = 26;
    static constexpr int CORNER_RADIUS = 8;

    static constexpr int INDICATOR_Y = PANEL_Y;
    static constexpr int SPEED_Y = INDICATOR_Y + INDICATOR_HEIGHT + SECTION_GAP;
    static constexpr int FILES_LABEL_Y = SPEED_Y + SPEED_HEIGHT + SECTION_GAP;
    static constexpr int FILES_LIST_Y = FILES_LABEL_Y + FILES_LABEL_HEIGHT + FILES_LABEL_GAP;
    static constexpr int STOPRUN_BUTTON_Y = PANEL_BOTTOM - BUTTON_HEIGHT;
    static constexpr int LOAD_BUTTON_Y = STOPRUN_BUTTON_Y - BUTTON_GAP - BUTTON_HEIGHT;
    static constexpr int FILES_LIST_BOTTOM = LOAD_BUTTON_Y - SECTION_GAP;
    static constexpr int FILES_LIST_HEIGHT = FILES_LIST_BOTTOM - FILES_LIST_Y;
    static constexpr int VISIBLE_ITEMS = (FILES_LIST_HEIGHT - (2 * SCROLL_ARROW_HEIGHT)) / LIST_ITEM_HEIGHT;

    static constexpr int RADIO_OUTER_RADIUS = 8;
    static constexpr int RADIO_INNER_RADIUS = 5;
    static constexpr int RADIO_INDENT = 12;
    static constexpr int RADIO_LABEL_OFFSET = 14;

    /**
     * @brief FreeRTOS queue handle for receiving DisplayMessage updates.
     */
    static QueueHandle_t _queue;

    /**
     * @brief Mutex that serialises all M5GFX draw calls between the DisplayTask
     *        and public methods called from other task contexts.
     */
    static SemaphoreHandle_t _displayMutex;

    static void DisplayTask(void *parameter);

    // Message dialog drawing and touch handling
    static void DrawMessageDialog(const char *title, const char *message);
    static void OnMessageDialogTouch(const lgfx::touch_point_t *points, int count);
    static void MessageDialogTask(void *parameter);

    /**
     * @brief Set to true by OnMessageDialogTouch when the OK button is pressed.
     */
    static volatile bool _messageDialogDismissed;

    /**
     * @brief Previous touch active state used for press-edge detection inside
     *        the message dialog touch handler.
     */
    static volatile bool _messageDialogPrevTouched;

    /**
     * @brief true while the message dialog is being displayed.
     *
     * Checked by DisplayTask to suppress storeline redraws that would paint
     * over the dialog.
     */
    static volatile bool _messageDialogActive;

    // Message dialog layout constants
    static constexpr int DIALOG_WIDTH = 600;
    static constexpr int DIALOG_HEIGHT = 300;
    static constexpr int DIALOG_X = (DISPLAY_WIDTH - DIALOG_WIDTH) / 2;
    static constexpr int DIALOG_Y = (DISPLAY_HEIGHT - DIALOG_HEIGHT) / 2;
    static constexpr int DIALOG_CORNER_RADIUS = 16;
    static constexpr int DIALOG_PADDING = 20;
    static constexpr int DIALOG_TITLE_HEIGHT = 56;
    static constexpr int DIALOG_BUTTON_HEIGHT = 44;
    static constexpr int DIALOG_BUTTON_WIDTH = 160;

    /**
     * @brief X co-ordinate of the left edge of the OK button, centred
     *        horizontally inside the dialog.
     */
    static constexpr int DIALOG_OK_BUTTON_X = DIALOG_X + (DIALOG_WIDTH - DIALOG_BUTTON_WIDTH) / 2;

    /**
     * @brief Y co-ordinate of the top edge of the OK button, positioned
     *        DIALOG_PADDING pixels above the bottom of the dialog.
     */
    static constexpr int DIALOG_OK_BUTTON_Y = DIALOG_Y + DIALOG_HEIGHT - DIALOG_PADDING - DIALOG_BUTTON_HEIGHT;
};
