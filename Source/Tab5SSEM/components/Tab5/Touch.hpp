/*-----------------------------------------------------------------------------
 * File        : Touch.hpp
 * Description : Interrupt-driven singleton (TouchInput) for the ST7123 touch
 *               controller on the M5Stack Tab5.  Manages a GPIO ISR on
 *               GPIO_NUM_23 and a dedicated FreeRTOS task.  Supports multiple
 *               registered callbacks that receive converted screen-space touch
 *               point data.
 * Author      : Mark Stevens
 * Copyright   : Copyright (c) 2026 Mark Stevens
 * Licence     : MIT — see LICENSE in the repository root for full terms.
 * Target      : M5Stack Tab5 (ESP32-P4)
 * Build system: ESP-IDF v5.5.3
 *---------------------------------------------------------------------------*/

#pragma once

#include <M5GFX.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <vector>

/**
 * @brief Interrupt-driven singleton for ST7123 touch input on the M5Stack Tab5.
 *
 * Manages a dedicated FreeRTOS task and a GPIO ISR on GPIO_NUM_23 (the ST7123
 * interrupt pin).  When the ST7123 asserts the line LOW to signal that touch
 * co-ordinate data is available, the ISR unblocks the task which reads and
 * converts the data before invoking all registered callbacks.
 *
 * @note GPIO_NUM_23 is configured as output-high by M5GFX during display
 *       initialisation to select the ST7123 I2C address (HIGH = 0x14).
 *       TouchInput must therefore be initialised AFTER display.init() has
 *       completed so that the pin can be safely reconfigured as an input.
 *
 * @note Verify that the ST7123 firmware on your specific hardware revision
 *       drives GPIO_NUM_23 LOW when touch data is ready.  If the interrupt
 *       never fires, remove the interrupt-driven approach and revert to polling.
 */
class TouchInput
{
public:
    /**
     * @brief Signature for touch event callbacks.
     *
     * @param touchPoints  Array of screen-space touch points (already
     *                     converted from raw sensor co-ordinates).
     * @param pointCount   Number of valid entries in touchPoints.  May be
     *                     zero when all fingers have been lifted.
     */
    using TouchCallback = void (*)(const lgfx::touch_point_t *touchPoints, int pointCount);

    static TouchInput *GetInstance();
    static TouchInput *Initialise(M5GFX &display);
    static TouchInput *Initialise(M5GFX &display, TouchCallback callback);
    ~TouchInput();
    void AddCallback(TouchCallback callback);
    void RemoveCallback(TouchCallback callback);

private:
    /** GPIO driven LOW by the ST7123 when touch co-ordinates are ready. */
    static constexpr gpio_num_t TOUCH_INTERRUPT_PIN = GPIO_NUM_23;

    /** Stack size in words for the touch processing task. */
    static constexpr uint32_t TOUCH_TASK_STACK_SIZE = 4096;

    /** FreeRTOS priority for the touch processing task. */
    static constexpr UBaseType_t TOUCH_TASK_PRIORITY = 5;

    /** Maximum number of simultaneous touch points read per interrupt. */
    static constexpr uint_fast8_t MAX_TOUCH_POINTS = 5;

    explicit TouchInput(M5GFX &display);
    static void IRAM_ATTR InterruptHandler(void *arg);
    static void Task(void *parameter);

    /** The single instance of this class. */
    static TouchInput *_instance;

    /** Reference to the display used to read and convert touch data. */
    M5GFX &_display;

    /** Binary semaphore signalled by the ISR when touch data is ready. */
    SemaphoreHandle_t _semaphore;

    /** Handle of the touch processing task, used for deletion. */
    TaskHandle_t _taskHandle;

    /** Mutex protecting _callbacks from concurrent access. */
    SemaphoreHandle_t _callbackMutex;

    /** Registered touch event callback functions. */
    std::vector<TouchCallback> _callbacks;
};
