/*-----------------------------------------------------------------------------
 * File        : Touch.cpp
 * Description : Implementation of the TouchInput singleton.  Configures the
 *               GPIO falling-edge interrupt on GPIO_NUM_23, spawns the touch
 *               processing FreeRTOS task, and dispatches touch events to all
 *               registered callbacks.
 * Author      : Mark Stevens
 * Copyright   : Copyright (c) 2026 Mark Stevens
 * Licence     : MIT — see LICENSE in the repository root for full terms.
 * Target      : M5Stack Tab5 (ESP32-P4)
 * Build system: ESP-IDF v5.5.3
 *---------------------------------------------------------------------------*/

#include "Touch.hpp"

#include <algorithm>

/** Initialise the static singleton pointer to null. */
TouchInput *TouchInput::_instance = nullptr;

/**
 * @brief Returns the existing singleton instance.
 *
 * @return Pointer to the singleton, or nullptr if Initialise() has not
 *         yet been called.
 */
TouchInput *TouchInput::GetInstance()
{
    return (_instance);
}

/**
 * @brief Creates and initialises the singleton with no registered callback.
 *
 * Acts as the default-constructor role for the singleton.  Must be called
 * after display.init().
 *
 * @param display  Reference to the global M5GFX display instance used to
 *                 read and convert raw touch data.
 * @return Pointer to the newly created singleton, or nullptr if the
 *         singleton already exists.
 */
TouchInput *TouchInput::Initialise(M5GFX &display)
{
    if (_instance != nullptr)
    {
        return (nullptr);
    }

    _instance = new TouchInput(display);
    return (_instance);
}

/**
 * @brief Creates and initialises the singleton with one pre-registered callback.
 *
 * Acts as the overloaded-constructor role for the singleton.  Must be
 * called after display.init().
 *
 * @param display   Reference to the global M5GFX display instance.
 * @param callback  Function invoked on every touch event.  Must not be
 *                  nullptr.
 * @return Pointer to the newly created singleton, or nullptr if the
 *         singleton already exists.
 */
TouchInput *TouchInput::Initialise(M5GFX &display, TouchCallback callback)
{
    TouchInput *instance = Initialise(display);

    if (instance != nullptr && callback != nullptr)
    {
        instance->AddCallback(callback);
    }

    return (instance);
}

/**
 * @brief Private constructor — use Initialise() to create the singleton.
 *
 * Configures the GPIO interrupt, installs the ISR, creates the binary
 * semaphore and mutex, and spawns the touch processing task.
 *
 * @param display  Reference to the global M5GFX display instance.
 */
TouchInput::TouchInput(M5GFX &display) : _display(display), _semaphore(nullptr), _taskHandle(nullptr), _callbackMutex(nullptr)
{
    _semaphore = xSemaphoreCreateBinary();
    _callbackMutex = xSemaphoreCreateMutex();

    gpio_config_t ioConfiguration = {};
    ioConfiguration.pin_bit_mask = (1ULL << TOUCH_INTERRUPT_PIN);
    ioConfiguration.mode = GPIO_MODE_INPUT;
    ioConfiguration.pull_up_en = GPIO_PULLUP_ENABLE;
    ioConfiguration.pull_down_en = GPIO_PULLDOWN_DISABLE;
    ioConfiguration.intr_type = GPIO_INTR_NEGEDGE;
    gpio_config(&ioConfiguration);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(TOUCH_INTERRUPT_PIN, InterruptHandler, this);

    xTaskCreate(Task, "TouchTask", TOUCH_TASK_STACK_SIZE, this, TOUCH_TASK_PRIORITY, &_taskHandle);
}

/**
 * @brief Destructor.
 *
 * Removes the GPIO ISR handler, deletes the FreeRTOS task and both
 * semaphores, clears the callback list, and resets the singleton pointer
 * so that Initialise() may be called again.
 */
TouchInput::~TouchInput()
{
    gpio_isr_handler_remove(TOUCH_INTERRUPT_PIN);

    if (_taskHandle != nullptr)
    {
        vTaskDelete(_taskHandle);
        _taskHandle = nullptr;
    }

    if (_semaphore != nullptr)
    {
        vSemaphoreDelete(_semaphore);
        _semaphore = nullptr;
    }

    if (_callbackMutex != nullptr)
    {
        vSemaphoreDelete(_callbackMutex);
        _callbackMutex = nullptr;
    }

    _instance = nullptr;
}

/**
 * @brief Registers a callback to receive touch events.
 *
 * Thread-safe.  Has no effect if callback is nullptr or already
 * registered.
 *
 * @param callback  Non-null function pointer to register.
 */
void TouchInput::AddCallback(TouchCallback callback)
{
    if (callback == nullptr)
    {
        return;
    }

    xSemaphoreTake(_callbackMutex, portMAX_DELAY);

    auto iterator = std::find(_callbacks.begin(), _callbacks.end(), callback);
    if (iterator == _callbacks.end())
    {
        _callbacks.push_back(callback);
    }

    xSemaphoreGive(_callbackMutex);
}

/**
 * @brief Removes a previously registered callback.
 *
 * Thread-safe.  Has no effect if callback is not currently registered.
 *
 * @param callback  Function pointer to remove.
 */
void TouchInput::RemoveCallback(TouchCallback callback)
{
    xSemaphoreTake(_callbackMutex, portMAX_DELAY);

    auto iterator = std::find(_callbacks.begin(), _callbacks.end(), callback);
    if (iterator != _callbacks.end())
    {
        _callbacks.erase(iterator);
    }

    xSemaphoreGive(_callbackMutex);
}

/**
 * @brief ISR fired on the falling edge of TOUCH_INTERRUPT_PIN.
 *
 * Gives _semaphore from interrupt context to unblock the touch task.
 * Must be placed in IRAM.
 *
 * @param arg  Pointer to the TouchInput singleton instance.
 */
void IRAM_ATTR TouchInput::InterruptHandler(void *arg)
{
    TouchInput *instance = static_cast<TouchInput *>(arg);
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(instance->_semaphore, &higherPriorityTaskWoken);
    portYIELD_FROM_ISR(higherPriorityTaskWoken);
}

/**
 * @brief FreeRTOS task entry point for touch event processing.
 *
 * Blocks on _semaphore, reads raw touch data, converts to screen
 * co-ordinates, then invokes all registered callbacks with the result.
 *
 * @param parameter  Pointer to the TouchInput singleton instance.
 */
void TouchInput::Task(void *parameter)
{
    TouchInput *instance = static_cast<TouchInput *>(parameter);

    while (true)
    {
        if (xSemaphoreTake(instance->_semaphore, portMAX_DELAY) != pdTRUE)
        {
            continue;
        }

        lgfx::touch_point_t touchPoints[MAX_TOUCH_POINTS];
        int pointCount = static_cast<int>(instance->_display.getTouchRaw(touchPoints, MAX_TOUCH_POINTS));

        if (pointCount > 0)
        {
            instance->_display.convertRawXY(touchPoints, static_cast<uint_fast8_t>(pointCount));
        }

        // Take a local snapshot of callbacks so the mutex is not held during
        // invocation, allowing AddCallback / RemoveCallback to be called from
        // within a callback without deadlocking.
        std::vector<TouchCallback> localCallbacks;
        xSemaphoreTake(instance->_callbackMutex, portMAX_DELAY);
        localCallbacks = instance->_callbacks;
        xSemaphoreGive(instance->_callbackMutex);

        for (const auto &callback: localCallbacks)
        {
            callback(touchPoints, pointCount);
        }
    }
}
