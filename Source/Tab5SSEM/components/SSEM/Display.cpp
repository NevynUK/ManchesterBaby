/*-----------------------------------------------------------------------------
 * File        : Display.cpp
 * Description : SSEM emulator display controller for the M5Stack Tab5.
 *               Manages the splash screen, main interface header, footer,
 *               storeline LED grid, and storeline text labels.
 * Author      : Mark Stevens
 * Copyright   : Copyright (c) 2026 Mark Stevens
 * Licence     : MIT — see LICENSE in the repository root for full terms.
 * Target      : M5Stack Tab5 (ESP32-P4)
 * Build system: ESP-IDF v5.5.1
 *---------------------------------------------------------------------------*/

#include "Display.hpp"
#include "Touch.hpp"
#include "Utility.hpp"
#include <cstdio>
#include <inttypes.h>
#include <freertos/task.h>

using namespace std;

// ---------------------------------------------------------------------------
// Static member definitions
// ---------------------------------------------------------------------------

/**
 * @brief Pointer to the global M5GFX display instance; assigned by Run().
 */
M5GFX *Display::_display = nullptr;

/**
 * @brief Set to true by OnSplashTouch to signal touch-based dismissal.
 */
volatile bool Display::_splashDismissed = false;

/**
 * @brief Flags indicating which storelines have changed.
 */
bool Display::_changed[Display::STORELINE_COUNT] = {};

/**
 * @brief SSEM store words; all initialised to zero (JMP 0) in Run().
 */
uint32_t Display::_store[Display::STORELINE_COUNT] = {};

/**
 * @brief Storeline text labels; all initialised to "JMP 0" in Run().
 */
char Display::_labels[Display::STORELINE_COUNT][32] = {};

/**
 * @brief true while the SSEM CPU is executing a program.
 */
bool Display::_running = false;

/**
 * @brief Currently selected execution speed; defaults to Maximum.
 */
Display::SpeedSetting Display::_speedSetting = Display::SpeedSetting::Maximum;

/**
 * @brief Ordered list of loadable SSEM program file paths.
 */
vector<string> Display::_files;

/**
 * @brief Zero-based index of the selected file; −1 when none is selected.
 */
int Display::_selectedFile = -1;

/**
 * @brief Index of the first file visible at the top of the list.
 */
int Display::_scrollOffset = 0;

/**
 * @brief true when the Load button is enabled.
 */
bool Display::_loadEnabled = false;

/**
 * @brief true when the Stop/Run button is enabled.
 */
bool Display::_stopRunEnabled = false;

/**
 * @brief true when the speed radio buttons are enabled.
 */
bool Display::_speedEnabled = true;

/**
 * @brief Previous touch active state used to detect press edges.
 */
bool Display::_prevTouched = false;

/**
 * @brief Registered callback for the Stop/Run button; empty by default.
 */
Display::StopRunCallback Display::_stopRunCallback = nullptr;

/**
 * @brief Registered callback for the Load button; empty by default.
 */
Display::LoadCallback Display::_loadCallback = nullptr;

/**
 * @brief Set to true by OnMessageDialogTouch when the OK button is pressed.
 */
volatile bool Display::_messageDialogDismissed = false;

/**
 * @brief Previous touch active state used for press-edge detection in the
 *        message dialog touch handler.
 */
volatile bool Display::_messageDialogPrevTouched = false;

/**
 * @brief true while the message dialog is being displayed; suppresses
 *        storeline redraws from the DisplayTask.
 */
volatile bool Display::_messageDialogActive = false;

/**
 * @brief Name of the currently loaded program; empty when none is loaded.
 */
string Display::_loadedProgram;

/**
 * @brief Instruction count displayed in the footer during and after execution.
 */
uint32_t Display::_instructionCount = 0;

/**
 * @brief Elapsed execution time in seconds displayed in the footer.
 */
double Display::_elapsedSeconds = 0.0;

/**
 * @brief FreeRTOS queue handle; created by Run() before the Display task is started.
 */
QueueHandle_t Display::_queue = nullptr;

/**
 * @brief Mutex that serialises all M5GFX draw calls.
 *
 * Taken by the DisplayTask for each queue message and by every public method
 * that writes directly to the display, preventing concurrent framebuffer
 * access from multiple FreeRTOS tasks.
 */
SemaphoreHandle_t Display::_displayMutex = nullptr;

// ---------------------------------------------------------------------------
// Public methods
// ---------------------------------------------------------------------------

/**
 * @brief Initialises storage, shows the splash screen, draws the main
 *        interface, then starts the Display FreeRTOS task.
 *
 * Stores the display pointer, zeroes the SSEM store, sets all labels
 * to "JMP 0", delegates to ShowSplash() then ShowMain(), creates the
 * message queue and launches the Display task.
 *
 * @param display  Reference to the global M5GFX instance (already
 *                 initialised with the correct rotation).
 * @param sdCard   Pointer to the SDCard singleton, or nullptr if no
 *                 card is present.
 */
void Display::Run(M5GFX &display, SDCard *sdCard)
{
    _display = &display;

    for (int i = 0; i < STORELINE_COUNT; ++i)
    {
        _store[i] = 0U;
        _changed[i] = true;
        snprintf(_labels[i], sizeof(_labels[i]), "JMP 0");
    }

    ShowSplash(sdCard);
    ShowMain();

    TouchInput::GetInstance()->AddCallback(OnPanelTouch);

    _queue = xQueueCreate(4, sizeof(DisplayMessage));
    configASSERT(_queue != nullptr);
    _displayMutex = xSemaphoreCreateMutex();
    configASSERT(_displayMutex != nullptr);
    xTaskCreate(DisplayTask, "DisplayTask", 8192, nullptr, 5, nullptr);
}

/**
 * @brief Updates the running/halted state and redraws the affected regions.
 *
 * Updates the status indicator and the Stop/Run button label.
 *
 * @param running  true if the SSEM CPU is now executing; false if halted.
 */
void Display::SetRunning(bool running)
{
    const MutexGuard guard(_displayMutex);
    _running = running;
    DrawRunningIndicator();
    DrawFileList();
    DrawActionButtons();
    _display->display();
}

/**
 * @brief Replaces the file list and redraws the list box.
 *
 * Resets the scroll offset and file selection whenever the list is
 * refreshed.
 *
 * @param files  Ordered vector of full file paths to display.
 */
void Display::SetFiles(const vector<string> &files)
{
    const MutexGuard guard(_displayMutex);
    _files = files;
    _selectedFile = -1;
    _scrollOffset = 0;
    DrawFileList();
    DrawActionButtons();
    _display->display();
}

/**
 * @brief Sets the enabled state of the Load button.
 *
 * Redraws the action buttons to reflect the new state.
 *
 * @param enabled  true to enable the Load button; false to disable it.
 */
void Display::SetLoadEnabled(bool enabled)
{
    const MutexGuard guard(_displayMutex);
    _loadEnabled = enabled;
    DrawActionButtons();
    _display->display();
}

/**
 * @brief Sets the enabled state of the Stop/Run button.
 *
 * Redraws the action buttons to reflect the new state.
 *
 * @param enabled  true to enable the Stop/Run button; false to disable it.
 */
void Display::SetStopRunEnabled(bool enabled)
{
    const MutexGuard guard(_displayMutex);
    _stopRunEnabled = enabled;
    DrawActionButtons();
    _display->display();
}

/**
 * @brief Sets the enabled state of the speed radio buttons.
 *
 * Redraws the speed section to reflect the new state.
 *
 * @param enabled  true to enable the speed buttons; false to disable them.
 */
void Display::SetSpeedEnabled(bool enabled)
{
    const MutexGuard guard(_displayMutex);
    _speedEnabled = enabled;
    DrawSpeedSection();
    _display->display();
}

/**
 * @brief Sets the name of the currently loaded program.
 *
 * Updates the header to display the program name in brackets beside the
 * title.  Pass an empty string to clear the name.
 *
 * @param name  Program name to display (typically the filename without path
 *              or extension).
 */
void Display::SetProgramName(const string &name)
{
    const MutexGuard guard(_displayMutex);
    _loadedProgram = name;
    _instructionCount = 0;
    _elapsedSeconds = 0.0;
    DrawHeader();
    DrawFooter();
    _display->display();
}

/**
 * @brief Updates the footer with the current instruction count and elapsed time.
 *
 * Redraws the footer bar in place without touching any other region of the
 * display.
 *
 * @param instructionCount  Total number of instructions executed so far.
 * @param elapsedSeconds    Wall-clock execution time in seconds.
 */
void Display::UpdateFooter(uint32_t instructionCount, double elapsedSeconds)
{
    const MutexGuard guard(_displayMutex);
    _instructionCount = instructionCount;
    _elapsedSeconds = elapsedSeconds;
    DrawFooter();
    _display->display();
}

/**
 * @brief Retrieves the current speed setting.
 *
 * @return SpeedSetting  The active speed mode (Maximum or Original).
 */
Display::SpeedSetting Display::GetSpeed()
{
    return (_speedSetting);
}

/**
 * @brief Registers the callback for the Stop/Run button.
 *
 * The callback is invoked with the new intended running state when the
 * Stop/Run button is pressed: true = start, false = stop.
 *
 * @param callback  Callable accepting a bool (the new running state).
 */
void Display::SetStopRunCallback(StopRunCallback callback)
{
    _stopRunCallback = callback;
}

/**
 * @brief Registers the callback for the Load button.
 *
 * The callback is invoked with the full path of the selected file when
 * the Load button is pressed.
 *
 * @param callback  Callable accepting a const string reference (file path).
 */
void Display::SetLoadCallback(LoadCallback callback)
{
    _loadCallback = callback;
}

// ---------------------------------------------------------------------------
// Private methods
// ---------------------------------------------------------------------------

/**
 * @brief Renders the splash screen and blocks until dismissed.
 *
 * Displays the SSEM Emulator title, "Manchester Baby" subtitle, and SD
 * card information.  Registers a temporary touch callback and blocks for
 * up to SPLASH_TIMEOUT_MS milliseconds, or until a touch is detected.
 *
 * @param sdCard  Pointer to the SDCard singleton, or nullptr.
 */
void Display::ShowSplash(SDCard *sdCard)
{
    const int centreX = _display->width() / 2;
    const int centreY = _display->height() / 2;

    _display->startWrite();
    _display->fillScreen(TFT_WHITE);
    _display->setTextColor(TFT_BLACK, TFT_WHITE);
    _display->setTextDatum(textdatum_t::middle_center);

    // Title and subtitle
    _display->setFont(&fonts::Font4);
    _display->drawString("SSEM Emulator", centreX, centreY - 80);
    _display->drawString("Manchester Baby", centreX, centreY - 40);

    // SD card information
    if (sdCard != nullptr && sdCard->IsMounted())
    {
        const sdmmc_card_t *card = sdCard->GetCard();
        const uint64_t sizeBytes = static_cast<uint64_t>(card->csd.capacity) * static_cast<uint64_t>(card->csd.sector_size);
        const double sizeGigabytes = static_cast<double>(sizeBytes) / (1024.0 * 1024.0 * 1024.0);

        char sdInfo[64];
        snprintf(sdInfo, sizeof(sdInfo), "SD Card: %.1f GB  (%s)", sizeGigabytes, card->cid.name);
        _display->drawString(sdInfo, centreX, centreY + 10);
    }
    else
    {
        _display->setTextColor(TFT_RED, TFT_WHITE);
        _display->drawString("SD Card: not present", centreX, centreY + 10);
        _display->setTextColor(TFT_BLACK, TFT_WHITE);
    }

    // Dismissal hint
    _display->setFont(&fonts::Font2);
    _display->drawString("Touch the screen or wait 5 seconds to continue.", centreX, centreY + 70);

    _display->endWrite();
    _display->display();

    // Register touch callback and poll until dismissed or timed out
    _splashDismissed = false;
    TouchInput::GetInstance()->AddCallback(OnSplashTouch);

    const TickType_t startTick = xTaskGetTickCount();

    while (!_splashDismissed)
    {
        if ((xTaskGetTickCount() - startTick) >= pdMS_TO_TICKS(SPLASH_TIMEOUT_MS))
        {
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }

    TouchInput::GetInstance()->RemoveCallback(OnSplashTouch);
}

/**
 * @brief Renders the full main SSEM interface.
 *
 * Fills the screen black, then draws the header, footer, and all 32
 * storeline rows.  Flushes the framebuffer to the MIPI-DSI panel on
 * completion.
 */
void Display::ShowMain()
{
    _display->startWrite();
    _display->fillScreen(TFT_BLACK);
    _display->endWrite();

    DrawHeader();
    DrawFooter();
    DrawAllStorelines();
    DrawControlPanel();

    _display->display();
}

/**
 * @brief Draws the header banner across the top of the screen.
 *
 * White background, black text, centred, using Font4 (~14 px).
 */
void Display::DrawHeader()
{
    _display->startWrite();
    _display->fillRect(0, 0, _display->width(), HEADER_HEIGHT, TFT_WHITE);
    _display->setFont(&fonts::Font4);
    _display->setTextColor(TFT_BLACK, TFT_WHITE);
    _display->setTextDatum(textdatum_t::middle_center);

    if (_loadedProgram.empty())
    {
        _display->drawString("SSEM - Manchester Baby", _display->width() / 2, HEADER_HEIGHT / 2);
    }
    else
    {
        const string title = "SSEM - Manchester Baby (" + _loadedProgram + ")";
        _display->drawString(title.c_str(), _display->width() / 2, HEADER_HEIGHT / 2);
    }

    _display->endWrite();
}

/**
 * @brief Draws the footer bar across the bottom of the screen.
 *
 * White background, black text, left-aligned, using Font2 (~8 px).
 */
void Display::DrawFooter()
{
    const int footerY = _display->height() - FOOTER_HEIGHT;

    _display->startWrite();
    _display->fillRect(0, footerY, _display->width(), FOOTER_HEIGHT, TFT_WHITE);
    _display->setFont(&fonts::Font2);
    _display->setTextColor(TFT_BLACK, TFT_WHITE);
    _display->setTextDatum(textdatum_t::middle_left);

    char footerText[64];
    snprintf(footerText, sizeof(footerText), "Instructions: %s  Time: %.2f s", Utility::FormatWithCommas(_instructionCount).c_str(), _elapsedSeconds);
    _display->drawString(footerText, 8, footerY + FOOTER_HEIGHT / 2);

    _display->endWrite();
}

/**
 * @brief Draws all 32 storeline rows into the centre panel.
 *
 * Wraps individual DrawStoreline() calls inside a single
 * startWrite / endWrite pair for efficiency.
 */
void Display::DrawAllStorelines()
{
    _display->startWrite();

    const int centreHeight = _display->height() - HEADER_HEIGHT - FOOTER_HEIGHT;
    const int boxY = HEADER_HEIGHT + BOX_PADDING;
    const int boxHeight = centreHeight - (2 * BOX_PADDING);

    // White border box enclosing all LED rows
    _display->drawRect(LED_SECTION_X, boxY, LED_SECTION_WIDTH + BOX_PADDING, boxHeight, TFT_WHITE);

    // White border box enclosing all storeline text rows (with TEXT_LEFT_MARGIN separation from LEDs)
    _display->drawRect(TEXT_SECTION_X + TEXT_LEFT_MARGIN - BOX_PADDING, boxY, TEXT_SECTION_WIDTH - TEXT_LEFT_MARGIN + (2 * BOX_PADDING), boxHeight, TFT_WHITE);

    for (int i = 0; i < STORELINE_COUNT; ++i)
    {
        if (_changed[i])
        {
            DrawStoreline(i);
        }
    }

    _display->endWrite();
}

/**
 * @brief Draws one storeline row: index number, 32 LEDs, and the text label.
 *
 * The storeline index is drawn to the left of the LED box, outside it.
 * Must be called within an active startWrite / endWrite pair on _display.
 *
 * @param lineIndex  Zero-based storeline index (0–31).
 */
void Display::DrawStoreline(int lineIndex)
{
    const int centreHeight = _display->height() - HEADER_HEIGHT - FOOTER_HEIGHT;
    const int rowHeight = centreHeight / STORELINE_COUNT;
    const int totalUsed = rowHeight * STORELINE_COUNT;
    const int storelineOffset = (centreHeight - totalUsed) / 2;
    const int rowY = HEADER_HEIGHT + storelineOffset + lineIndex * rowHeight;
    const int ledCentreY = rowY + rowHeight / 2;

    // Draw the storeline index number to the left of the LED box (outside it)
    char lineNumber[4];
    snprintf(lineNumber, sizeof(lineNumber), "%" PRId32, (int32_t) lineIndex);
    _display->setFont(&fonts::Font2);
    _display->setTextColor(TFT_WHITE, TFT_BLACK);
    _display->setTextDatum(textdatum_t::middle_right);
    _display->drawString(lineNumber, LED_SECTION_X - BOX_PADDING, ledCentreY);

    // Draw 32 LEDs — bit 0 (LSB) is displayed leftmost
    const uint32_t value = _store[lineIndex];

    for (int bit = 0; bit < LED_COUNT; ++bit)
    {
        const bool on = ((value >> bit) & 1U) != 0U;
        const int group = bit / LED_GROUP_SIZE;
        const int ledCentreX = LED_SECTION_X + bit * LED_CELL_WIDTH + LED_CELL_WIDTH / 2 + group * LED_GROUP_GAP;
        DrawLed(ledCentreX, ledCentreY, LED_OUTER_RADIUS, on);
    }

    // Draw the zero-padded 8-digit hex value of the storeline word
    char hexValue[9];
    snprintf(hexValue, sizeof(hexValue), "%08" PRIX32, value);
    _display->setFont(&fonts::Font2);
    _display->setTextColor(TFT_WHITE, TFT_BLACK);
    _display->setTextDatum(textdatum_t::middle_left);
    _display->drawString(hexValue, TEXT_SECTION_X + TEXT_LEFT_MARGIN + BOX_PADDING, ledCentreY);

    // Draw the mnemonic text label to the right of the hex column
    const int labelX = TEXT_SECTION_X + TEXT_LEFT_MARGIN + BOX_PADDING + HEX_COLUMN_WIDTH;
    const int labelWidth = TEXT_SECTION_WIDTH - TEXT_LEFT_MARGIN - HEX_COLUMN_WIDTH - 1;
    _display->fillRect(labelX, rowY + 1, labelWidth, rowHeight - 2, TFT_BLACK);

    _display->drawString(_labels[lineIndex], labelX, ledCentreY);
}

/**
 * @brief Draws a single LED as a white-bordered circle with a colour fill.
 *
 * Renders a filled white circle of the given radius, then overlays a
 * smaller filled circle in green (on) or black (off) to simulate an
 * illuminated LED.  Must be called within an active startWrite /
 * endWrite pair on _display.
 *
 * @param centreX  X co-ordinate of the LED centre.
 * @param centreY  Y co-ordinate of the LED centre.
 * @param radius   Outer radius of the white border circle in pixels.
 * @param on       true for green inner fill (LED lit); false for black.
 */
void Display::DrawLed(int centreX, int centreY, int radius, bool on)
{
    _display->fillCircle(centreX, centreY, radius, TFT_WHITE);
    _display->fillCircle(centreX, centreY, radius - 2, on ? TFT_GREEN : TFT_BLACK);
}

/**
 * @brief Redraws the entire control panel from the current state.
 *
 * Clears the panel background, then redraws all four regions in order:
 * running indicator, speed section, file list, and action buttons.
 */
void Display::DrawControlPanel()
{
    _display->startWrite();
    _display->fillRect(CONTROL_SECTION_X, HEADER_HEIGHT, DISPLAY_WIDTH - CONTROL_SECTION_X, DISPLAY_HEIGHT - HEADER_HEIGHT - FOOTER_HEIGHT, TFT_BLACK);
    _display->endWrite();

    DrawRunningIndicator();
    DrawSpeedSection();
    DrawFileList();
    DrawActionButtons();
}

/**
 * @brief Draws the Halted/Running status indicator.
 *
 * Renders a white-bordered rounded rectangle with a black fill, containing
 * "Running" in green or "Halted" in red, centred within the rectangle.
 */
void Display::DrawRunningIndicator()
{
    _display->startWrite();

    // White border
    _display->fillRoundRect(PANEL_X, INDICATOR_Y, PANEL_WIDTH, INDICATOR_HEIGHT, CORNER_RADIUS, TFT_WHITE);
    // Black fill inside the border
    _display->fillRoundRect(PANEL_X + 2, INDICATOR_Y + 2, PANEL_WIDTH - 4, INDICATOR_HEIGHT - 4, CORNER_RADIUS - 1, TFT_BLACK);

    const uint16_t textColour = _running ? static_cast<uint16_t>(TFT_GREEN) : static_cast<uint16_t>(TFT_RED);
    const char *label = _running ? "Running" : "Halted";

    _display->setFont(&fonts::Font4);
    _display->setTextColor(textColour, TFT_BLACK);
    _display->setTextDatum(textdatum_t::middle_center);
    _display->drawString(label, PANEL_X + PANEL_WIDTH / 2, INDICATOR_Y + INDICATOR_HEIGHT / 2);

    _display->endWrite();
}

/**
 * @brief Draws the speed radio-button section.
 *
 * Renders a "Speed:" label above two radio buttons labelled "Maximum"
 * and "Original" placed side by side on a single row.  The selected
 * option has its inner circle filled white; the unselected option has
 * a hollow circle (black inner fill).
 */
void Display::DrawSpeedSection()
{
    _display->startWrite();

    // Clear section background
    _display->fillRect(PANEL_X, SPEED_Y, PANEL_WIDTH, SPEED_HEIGHT, TFT_BLACK);

    // Use grey for all elements when the speed section is disabled
    const uint16_t sectionColour = _speedEnabled ? static_cast<uint16_t>(TFT_WHITE) : static_cast<uint16_t>(TFT_DARKGREY);

    // Section label
    _display->setFont(&fonts::Font4);
    _display->setTextColor(sectionColour, TFT_BLACK);
    _display->setTextDatum(textdatum_t::top_left);
    _display->drawString("Speed:", PANEL_X, SPEED_Y + 2);

    const int halfWidth = PANEL_WIDTH / 2;
    const int rowCentreY = SPEED_Y + 40;

    // Maximum radio button (left half)
    const int maxCircleX = PANEL_X + RADIO_INDENT + RADIO_OUTER_RADIUS;
    const int maxLabelX = maxCircleX + RADIO_OUTER_RADIUS + RADIO_LABEL_OFFSET;
    const bool maxSelected = (_speedSetting == SpeedSetting::Maximum);
    _display->drawCircle(maxCircleX, rowCentreY, RADIO_OUTER_RADIUS, sectionColour);
    _display->fillCircle(maxCircleX, rowCentreY, RADIO_INNER_RADIUS, maxSelected ? sectionColour : static_cast<uint16_t>(TFT_BLACK));
    _display->setFont(&fonts::Font4);
    _display->setTextDatum(textdatum_t::middle_left);
    _display->drawString("Maximum", maxLabelX, rowCentreY);

    // Original radio button (right half)
    const int origCircleX = PANEL_X + halfWidth + RADIO_INDENT + RADIO_OUTER_RADIUS;
    const int origLabelX = origCircleX + RADIO_OUTER_RADIUS + RADIO_LABEL_OFFSET;
    const bool origSelected = (_speedSetting == SpeedSetting::Original);
    _display->drawCircle(origCircleX, rowCentreY, RADIO_OUTER_RADIUS, sectionColour);
    _display->fillCircle(origCircleX, rowCentreY, RADIO_INNER_RADIUS, origSelected ? sectionColour : static_cast<uint16_t>(TFT_BLACK));
    _display->drawString("Original", origLabelX, rowCentreY);

    _display->endWrite();
}

/**
 * @brief Draws the scrollable file list box.
 *
 * Renders a "Files:" label, then draws a white-bordered box containing
 * an up-scroll arrow, up to VISIBLE_ITEMS file rows, and a down-scroll
 * arrow.  The selected item is highlighted with a white background and
 * black text.  Scroll arrows are grey when scrolling in that direction
 * is not available.
 */
void Display::DrawFileList()
{
    _display->startWrite();

    // "Files:" label above the box
    _display->fillRect(PANEL_X, FILES_LABEL_Y, PANEL_WIDTH, FILES_LABEL_HEIGHT, TFT_BLACK);
    _display->setFont(&fonts::Font4);
    _display->setTextColor(TFT_WHITE, TFT_BLACK);
    _display->setTextDatum(textdatum_t::top_left);
    _display->drawString("Files:", PANEL_X, FILES_LABEL_Y + 2);

    // White border box
    _display->drawRect(PANEL_X, FILES_LIST_Y, PANEL_WIDTH, FILES_LIST_HEIGHT, TFT_WHITE);

    // Clear list interior
    _display->fillRect(PANEL_X + 1, FILES_LIST_Y + 1, PANEL_WIDTH - 2, FILES_LIST_HEIGHT - 2, TFT_BLACK);

    // Scroll-up arrow (grey when disabled or already at the top)
    const bool canScrollUp = (_scrollOffset > 0);
    const uint16_t upColour = (!_running && canScrollUp) ? static_cast<uint16_t>(TFT_WHITE) : static_cast<uint16_t>(TFT_DARKGREY);
    const int upArrowCentreX = PANEL_X + PANEL_WIDTH / 2;
    const int upArrowCentreY = FILES_LIST_Y + SCROLL_ARROW_HEIGHT / 2;
    _display->fillTriangle(upArrowCentreX, upArrowCentreY - 6, upArrowCentreX - 8, upArrowCentreY + 5, upArrowCentreX + 8, upArrowCentreY + 5, upColour);

    // Scroll-down arrow (grey when disabled or already at the bottom)
    const int totalFiles = static_cast<int>(_files.size());
    const bool canScrollDown = (totalFiles > 0) && ((_scrollOffset + VISIBLE_ITEMS) < totalFiles);
    const uint16_t downColour = (!_running && canScrollDown) ? static_cast<uint16_t>(TFT_WHITE) : static_cast<uint16_t>(TFT_DARKGREY);
    const int downArrowCentreX = PANEL_X + PANEL_WIDTH / 2;
    const int downArrowCentreY = FILES_LIST_BOTTOM - SCROLL_ARROW_HEIGHT / 2;
    _display->fillTriangle(downArrowCentreX, downArrowCentreY + 6, downArrowCentreX - 8, downArrowCentreY - 5, downArrowCentreX + 8, downArrowCentreY - 5, downColour);

    // File list items
    const int itemsAreaY = FILES_LIST_Y + SCROLL_ARROW_HEIGHT;
    const int textX = PANEL_X + 6;

    _display->setFont(&fonts::Font4);
    _display->setTextDatum(textdatum_t::middle_left);

    for (int i = 0; i < VISIBLE_ITEMS; ++i)
    {
        const int fileIndex = _scrollOffset + i;
        const int itemY = itemsAreaY + (i * LIST_ITEM_HEIGHT);
        const int itemCentreY = itemY + LIST_ITEM_HEIGHT / 2;

        if (fileIndex < totalFiles)
        {
            const bool selected = (fileIndex == _selectedFile);

            // When running, render all items in grey to signal the list is disabled
            uint16_t bgColour;
            uint16_t textColour;

            if (_running)
            {
                bgColour = TFT_BLACK;
                textColour = TFT_DARKGREY;
            }
            else
            {
                bgColour = selected ? static_cast<uint16_t>(TFT_WHITE) : static_cast<uint16_t>(TFT_BLACK);
                textColour = selected ? static_cast<uint16_t>(TFT_BLACK) : static_cast<uint16_t>(TFT_WHITE);
            }

            _display->fillRect(PANEL_X + 1, itemY, PANEL_WIDTH - 2, LIST_ITEM_HEIGHT, bgColour);

            char nameBuffer[40];
            const string name = Basename(_files[fileIndex]);
            snprintf(nameBuffer, sizeof(nameBuffer), "%s", name.c_str());

            _display->setTextColor(textColour, bgColour);
            _display->drawString(nameBuffer, textX, itemCentreY);
        }
    }

    _display->endWrite();
}

/**
 * @brief Draws the Load and Stop/Run action buttons.
 *
 * The Load button label is always "Load".  The Stop/Run button label
 * is "Run" when the CPU is halted, or "Stop" when it is running.
 */
void Display::DrawActionButtons()
{
    DrawButton(PANEL_X, LOAD_BUTTON_Y, PANEL_WIDTH, BUTTON_HEIGHT, "Load", _loadEnabled);
    DrawButton(PANEL_X, STOPRUN_BUTTON_Y, PANEL_WIDTH, BUTTON_HEIGHT, _running ? "Stop" : "Run", _stopRunEnabled);
}

/**
 * @brief Draws a single rounded-rectangle button.
 *
 * Enabled style: white border, black fill, white label text.
 * Disabled style: grey border, black fill, grey label text.
 *
 * @param x        Left edge of the button in screen co-ordinates.
 * @param y        Top edge of the button in screen co-ordinates.
 * @param width    Width of the button in pixels.
 * @param height   Height of the button in pixels.
 * @param label    Null-terminated text to draw centred inside the button.
 * @param enabled  true for the enabled style; false for the disabled style.
 */
void Display::DrawButton(int x, int y, int width, int height, const char *label, bool enabled)
{
    const uint16_t colour = enabled ? static_cast<uint16_t>(TFT_WHITE) : static_cast<uint16_t>(TFT_DARKGREY);

    _display->startWrite();

    _display->fillRoundRect(x, y, width, height, CORNER_RADIUS, TFT_BLACK);
    _display->drawRoundRect(x, y, width, height, CORNER_RADIUS, colour);

    _display->setFont(&fonts::Font4);
    _display->setTextColor(colour, TFT_BLACK);
    _display->setTextDatum(textdatum_t::middle_center);
    _display->drawString(label, x + width / 2, y + height / 2);

    _display->endWrite();
}

/**
 * @brief Extracts the filename component from a full file path.
 *
 * Returns the substring following the last '/' character.  If no '/'
 * is present the full string is returned unchanged.
 *
 * @param path  Full file path, e.g. "/sdcard/program.ssem".
 * @return string  The filename portion, e.g. "program.ssem".
 */
string Display::Basename(const string &path)
{
    const size_t pos = path.rfind('/');
    return (pos != string::npos) ? path.substr(pos + 1) : path;
}

// ---------------------------------------------------------------------------
// Error dialog
// ---------------------------------------------------------------------------

/**
 * @brief Draws the message dialog overlay onto the current screen contents.
 *
 * Renders a rounded-corner popup with a white border and black fill.
 * The title is drawn in bold white at the top, separated from the body
 * by a horizontal rule.  The message is drawn centred in white below
 * the rule; newline characters produce additional lines.  An OK
 * rounded-rectangle button is drawn at the bottom of the dialog.
 *
 * Must be called while _displayMutex is held and inside a
 * startWrite / endWrite pair.
 *
 * @param title    Null-terminated title string.
 * @param message  Null-terminated message body; may contain '\n'.
 */
void Display::DrawMessageDialog(const char *title, const char *message)
{
    const int centreX = DIALOG_X + DIALOG_WIDTH / 2;

    // White border
    _display->fillRoundRect(DIALOG_X, DIALOG_Y, DIALOG_WIDTH, DIALOG_HEIGHT, DIALOG_CORNER_RADIUS, TFT_WHITE);
    // Black fill inside border
    _display->fillRoundRect(DIALOG_X + 2, DIALOG_Y + 2, DIALOG_WIDTH - 4, DIALOG_HEIGHT - 4, DIALOG_CORNER_RADIUS - 1, TFT_BLACK);

    // Title in bold white, centred horizontally, vertically centred in the
    // title band.  Bold is simulated by drawing the string twice: once at
    // the nominal position and once shifted one pixel to the right.
    const int titleY = DIALOG_Y + DIALOG_TITLE_HEIGHT / 2;
    _display->setFont(&fonts::Font4);
    _display->setTextColor(TFT_WHITE, TFT_BLACK);
    _display->setTextDatum(textdatum_t::middle_center);
    _display->drawString(title, centreX, titleY);
    _display->drawString(title, centreX + 1, titleY); // second pass for bold effect

    // Horizontal rule separating title from body
    _display->drawFastHLine(DIALOG_X + 2, DIALOG_Y + DIALOG_TITLE_HEIGHT, DIALOG_WIDTH - 4, TFT_WHITE);

    // Message body — split on '\n' and draw each line centred
    _display->setFont(&fonts::Font4);
    _display->setTextColor(TFT_WHITE, TFT_BLACK);
    _display->setTextDatum(textdatum_t::middle_center);

    const int messageAreaTop = DIALOG_Y + DIALOG_TITLE_HEIGHT + 4;
    const int messageAreaBottom = DIALOG_OK_BUTTON_Y - 4;
    const int messageAreaHeight = messageAreaBottom - messageAreaTop;
    constexpr int LINE_HEIGHT = 20;

    // Collect lines
    const string messageStr(message != nullptr ? message : "");
    vector<string> lines;
    size_t lineStart = 0;

    while (true)
    {
        const size_t newline = messageStr.find('\n', lineStart);
        if (newline == string::npos)
        {
            lines.push_back(messageStr.substr(lineStart));
            break;
        }
        lines.push_back(messageStr.substr(lineStart, newline - lineStart));
        lineStart = newline + 1;
    }

    // Vertically centre the block of lines within the message area
    const int totalTextHeight = static_cast<int>(lines.size()) * LINE_HEIGHT;
    int lineY = messageAreaTop + (messageAreaHeight - totalTextHeight) / 2 + LINE_HEIGHT / 2;

    for (const string &line: lines)
    {
        _display->drawString(line.c_str(), centreX, lineY);
        lineY += LINE_HEIGHT;
    }

    // OK button — white border, black fill, white label
    _display->fillRoundRect(DIALOG_OK_BUTTON_X, DIALOG_OK_BUTTON_Y, DIALOG_BUTTON_WIDTH, DIALOG_BUTTON_HEIGHT, CORNER_RADIUS, TFT_BLACK);
    _display->drawRoundRect(DIALOG_OK_BUTTON_X, DIALOG_OK_BUTTON_Y, DIALOG_BUTTON_WIDTH, DIALOG_BUTTON_HEIGHT, CORNER_RADIUS, TFT_WHITE);

    _display->setFont(&fonts::Font4);
    _display->setTextColor(TFT_WHITE, TFT_BLACK);
    _display->setTextDatum(textdatum_t::middle_center);
    _display->drawString("OK", DIALOG_OK_BUTTON_X + DIALOG_BUTTON_WIDTH / 2, DIALOG_OK_BUTTON_Y + DIALOG_BUTTON_HEIGHT / 2);
}

/**
 * @brief Displays a modal message dialog without blocking the calling task.
 *
 * Draws a popup with a bold-white title, a white message body, and a
 * rounded OK button.  Touch input is routed exclusively to the dialog
 * while it is visible.  A short-lived FreeRTOS task (MessageDialogTask)
 * is spawned to poll for dismissal and restore the main interface; this
 * avoids blocking the calling task, which may be TouchTask itself.
 *
 * @param title    Title text (mandatory; must not be nullptr or empty).
 * @param message  Message body.  May contain '\n' for line breaks.
 */
void Display::ShowMessageDialog(const char *title, const char *message)
{
    const char *resolvedMessage = (message != nullptr) ? message : "";

    // Draw the dialog over the current screen contents
    {
        const MutexGuard guard(_displayMutex);
        _messageDialogActive = true;
        _display->startWrite();
        DrawMessageDialog(title, resolvedMessage);
        _display->endWrite();
        _display->display();
    }

    // Transfer touch routing exclusively to the dialog handler before
    // spawning the dismissal task, so that no panel events are processed
    // between the dialog appearing and the dismissal task starting.
    TouchInput::GetInstance()->RemoveCallback(OnPanelTouch);
    _messageDialogDismissed = false;
    _messageDialogPrevTouched = false;
    TouchInput::GetInstance()->AddCallback(OnMessageDialogTouch);

    // Spawn a short-lived task to poll for dismissal and restore the UI.
    // This avoids blocking the calling task (which may be TouchTask itself,
    // making a blocking poll here a deadlock).
    xTaskCreate(MessageDialogTask, "MsgDialogTask", 4096, nullptr, 5, nullptr);
}

/**
 * @brief Short-lived FreeRTOS task that waits for the message dialog to be
 *        dismissed and then restores the main interface.
 *
 * Polls _messageDialogDismissed every 50 ms.  When the flag is set by
 * OnMessageDialogTouch, removes the dialog touch callback, redraws the
 * full main interface, re-registers OnPanelTouch, and deletes itself.
 *
 * Running the blocking wait in a dedicated task prevents deadlocking
 * when ShowMessageDialog is called from within a touch callback.
 *
 * @param parameter  Unused; required by the FreeRTOS task signature.
 */
void Display::MessageDialogTask(void *parameter)
{
    (void) parameter;

    // Poll until the OK button has been pressed
    while (!_messageDialogDismissed)
    {
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    TouchInput::GetInstance()->RemoveCallback(OnMessageDialogTouch);

    // Redraw the full main interface
    {
        const MutexGuard guard(_displayMutex);
        _messageDialogActive = false;

        _display->startWrite();
        _display->fillScreen(TFT_BLACK);
        _display->endWrite();

        // Force all storelines to redraw: the DisplayTask may have updated
        // _store and _labels while the dialog suppressed draws, so _changed[]
        // may be stale (all-false) even though the content has changed.
        for (int i = 0; i < STORELINE_COUNT; ++i)
        {
            _changed[i] = true;
        }

        DrawHeader();
        DrawFooter();
        DrawAllStorelines();
        DrawControlPanel();
        _display->display();
    }

    // Prevent the finger-lift from the OK button triggering a spurious press
    _prevTouched = true;
    TouchInput::GetInstance()->AddCallback(OnPanelTouch);

    vTaskDelete(nullptr);
}

/**
 * @brief Touch callback active while the message dialog is displayed.
 *
 * Detects a press on the OK button using edge-detection (transition from
 * no touch to touch).  Sets _messageDialogDismissed when the touch
 * co-ordinates fall within the OK button hit area.
 *
 * @param points  Array of screen-space touch co-ordinates.
 * @param count   Number of active touch points; zero when all lifted.
 */
void Display::OnMessageDialogTouch(const lgfx::touch_point_t *points, int count)
{
    const bool touched = (count > 0);

    if (touched && !_messageDialogPrevTouched)
    {
        if (Display::HitTest(points[0].x, points[0].y, DIALOG_OK_BUTTON_X, DIALOG_OK_BUTTON_Y, DIALOG_BUTTON_WIDTH, DIALOG_BUTTON_HEIGHT))
        {
            _messageDialogDismissed = true;
        }
    }

    _messageDialogPrevTouched = touched;
}

/**
 * @brief Touch callback active during the splash screen.
 *
 * Sets _splashDismissed to true when at least one touch point is active,
 * allowing the splash polling loop to exit early.
 *
 * @param points  Array of screen-space touch co-ordinates.
 * @param count   Number of active touch points; zero when all lifted.
 */
void Display::OnSplashTouch(const lgfx::touch_point_t *points, int count)
{
    (void) points;

    if (count > 0)
    {
        _splashDismissed = true;
    }
}

/**
 * @brief Touch callback registered with TouchInput for the main interface.
 *
 * Detects press edges (the moment a finger first contacts the screen) by
 * comparing the current touch state against the previous state.  Only new
 * press events (count transitions from 0 to > 0) are forwarded to
 * HandlePress().  Events with all fingers lifted reset the edge state.
 *
 * @param points  Array of screen-space touch co-ordinates.
 * @param count   Number of active touch points; zero when all fingers lifted.
 */
void Display::OnPanelTouch(const lgfx::touch_point_t *points, int count)
{
    const bool touched = (count > 0);

    if (touched && !_prevTouched)
    {
        HandlePress(points[0].x, points[0].y);
    }

    _prevTouched = touched;
}

/**
 * @brief Processes a newly detected touch press on the control panel.
 *
 * Tests the touch co-ordinate against each interactive region in order:
 * speed radio buttons, file list scroll arrows, file list items, Load
 * button, Stop/Run button.  Touches whose X co-ordinate falls to the left
 * of the control section are silently ignored.
 *
 * @param touchX  Screen X co-ordinate of the press.
 * @param touchY  Screen Y co-ordinate of the press.
 */
void Display::HandlePress(int touchX, int touchY)
{
    // Must be in the control panel column
    if (touchX < CONTROL_SECTION_X)
    {
        return;
    }

    // -------------------------------------------------------------------
    // Speed radio buttons (side by side on one row) — disabled while running
    // -------------------------------------------------------------------
    if (_speedEnabled)
    {
        const int halfWidth = PANEL_WIDTH / 2;
        const int rowCentreY = SPEED_Y + 38;
        const int rowHalfH = LIST_ITEM_HEIGHT / 2;

        if (HitTest(touchX, touchY, PANEL_X, rowCentreY - rowHalfH, halfWidth, LIST_ITEM_HEIGHT))
        {
            _speedSetting = SpeedSetting::Maximum;
            DrawSpeedSection();
            _display->display();
            return;
        }

        if (HitTest(touchX, touchY, PANEL_X + halfWidth, rowCentreY - rowHalfH, halfWidth, LIST_ITEM_HEIGHT))
        {
            _speedSetting = SpeedSetting::Original;
            DrawSpeedSection();
            _display->display();
            return;
        }
    }

    // -------------------------------------------------------------------
    // Scroll-up arrow
    // -------------------------------------------------------------------
    if (HitTest(touchX, touchY, PANEL_X, FILES_LIST_Y, PANEL_WIDTH, SCROLL_ARROW_HEIGHT))
    {
        if (!_running && _scrollOffset > 0)
        {
            _scrollOffset--;
            DrawFileList();
            _display->display();
        }
        return;
    }

    // -------------------------------------------------------------------
    // Scroll-down arrow
    // -------------------------------------------------------------------
    if (HitTest(touchX, touchY, PANEL_X, FILES_LIST_BOTTOM - SCROLL_ARROW_HEIGHT, PANEL_WIDTH, SCROLL_ARROW_HEIGHT))
    {
        const int totalFiles = static_cast<int>(_files.size());
        if (!_running && (_scrollOffset + VISIBLE_ITEMS) < totalFiles)
        {
            _scrollOffset++;
            DrawFileList();
            _display->display();
        }
        return;
    }

    // -------------------------------------------------------------------
    // File list item selection
    // -------------------------------------------------------------------
    {
        const int itemsAreaY = FILES_LIST_Y + SCROLL_ARROW_HEIGHT;
        const int itemsAreaBottom = FILES_LIST_BOTTOM - SCROLL_ARROW_HEIGHT;
        const int itemsAreaHeight = itemsAreaBottom - itemsAreaY;

        if (HitTest(touchX, touchY, PANEL_X, itemsAreaY, PANEL_WIDTH, itemsAreaHeight))
        {
            if (!_running)
            {
                const int relativeY = touchY - itemsAreaY;
                const int itemIndex = relativeY / LIST_ITEM_HEIGHT;
                const int fileIndex = _scrollOffset + itemIndex;
                const int totalFiles = static_cast<int>(_files.size());

                if (fileIndex >= 0 && fileIndex < totalFiles)
                {
                    _selectedFile = fileIndex;
                    _loadEnabled = true;
                    DrawFileList();
                    DrawActionButtons();
                    _display->display();
                }
            }
            return;
        }
    }

    // -------------------------------------------------------------------
    // Load button
    // -------------------------------------------------------------------
    if (_loadEnabled && HitTest(touchX, touchY, PANEL_X, LOAD_BUTTON_Y, PANEL_WIDTH, BUTTON_HEIGHT))
    {
        if (_loadCallback && _selectedFile >= 0 && _selectedFile < static_cast<int>(_files.size()))
        {
            _loadCallback(_files[_selectedFile]);
        }
        return;
    }

    // -------------------------------------------------------------------
    // Stop/Run button
    // -------------------------------------------------------------------
    if (_stopRunEnabled && HitTest(touchX, touchY, PANEL_X, STOPRUN_BUTTON_Y, PANEL_WIDTH, BUTTON_HEIGHT))
    {
        _running = !_running;

        if (_running)
        {
            // Transitioning to running: disable speed, load, and list interaction
            _speedEnabled = false;
            _loadEnabled = false;
        }
        else
        {
            // Transitioning to stopped: re-enable speed and restore load state
            _speedEnabled = true;
            _loadEnabled = (_selectedFile >= 0);
        }

        DrawRunningIndicator();
        DrawSpeedSection();
        DrawFileList();
        DrawActionButtons();
        _display->display();

        if (_stopRunCallback)
        {
            _stopRunCallback(_running);
        }
        return;
    }
}

/**
 * @brief Returns true if the touch point falls within the given rectangle.
 *
 * @param touchX  X co-ordinate of the touch point.
 * @param touchY  Y co-ordinate of the touch point.
 * @param x       Left edge of the test rectangle.
 * @param y       Top edge of the test rectangle.
 * @param width   Width of the test rectangle in pixels.
 * @param height  Height of the test rectangle in pixels.
 * @return bool   true if the point lies inside (inclusive of edges).
 */
bool Display::HitTest(int touchX, int touchY, int x, int y, int width, int height)
{
    return (touchX >= x && touchX < (x + width) && touchY >= y && touchY < (y + height));
}

/**
 * @brief Posts a DisplayMessage to the display queue.
 *
 * Non-blocking: if the queue is full the message is discarded and false
 * is returned.
 *
 * @param message  The display state snapshot to enqueue.
 * @return true if the message was accepted; false if the queue was full.
 */
bool Display::PostMessage(const DisplayMessage &message)
{
    return (xQueueSend(_queue, &message, portMAX_DELAY) == pdTRUE);
}

/**
 * @brief FreeRTOS task body for the Display task.
 *
 * Blocks indefinitely on _queue.  On each message received, copies the
 * storeline values and text labels into the static members then redraws
 * all storeline rows and flushes the framebuffer.  If the message carries
 * enableStopRun set to true, the Stop/Run button is also enabled and redrawn
 * in the same pass, avoiding concurrent drawing from other tasks.
 *
 * @param parameter  Unused; required by the FreeRTOS task signature.
 */
void Display::DisplayTask(void *parameter)
{
    (void) parameter;

    DisplayMessage message;

    while (true)
    {
        if (xQueueReceive(_queue, &message, portMAX_DELAY) == pdTRUE)
        {
            const MutexGuard guard(_displayMutex);

            // Always apply state updates so _store, _labels, and _changed
            // remain current even when the message dialog is covering the screen.
            for (int i = 0; i < STORELINE_COUNT; ++i)
            {
                if (_store[i] != message.storelineValues[i])
                {
                    _changed[i] = true;
                    _store[i] = message.storelineValues[i];
                    snprintf(_labels[i], sizeof(_labels[i]), "%s", message.storelineText[i]);
                }
                else
                {
                    _changed[i] = false;
                }
            }

            if (message.enableStopRun)
            {
                _stopRunEnabled = true;
            }

            // Skip drawing while the message dialog is covering the screen.
            if (_messageDialogActive)
            {
                continue;
            }

            DrawAllStorelines();

            if (message.enableStopRun)
            {
                DrawActionButtons();
            }

            if (message.halted)
            {
                _running = false;
                _speedEnabled = true;
                _loadEnabled = (_selectedFile >= 0);
                DrawRunningIndicator();
                DrawSpeedSection();
                DrawFileList();
                DrawActionButtons();
            }

            _display->display();
        }
    }
}
