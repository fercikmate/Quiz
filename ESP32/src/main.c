#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define LED_GPIO 4       // LED
#define BUTTON_GPIO26 26 // Button
#define BUTTON_GPIO25 25 // Button
#define BUTTON_GPIO27 27 // Button
#define BUTTON_GPIO33 33 // Button
#define BUTTON_GPIO32 32 // Button

const gpio_num_t button_gpios[] = {BUTTON_GPIO27, BUTTON_GPIO33,
                                   BUTTON_GPIO26, BUTTON_GPIO25, BUTTON_GPIO32};
#define NUM_BUTTONS (sizeof(button_gpios) / sizeof(button_gpios[0]))

uint8_t buttons_pressed_bitmask = 0;
void vListenTask(void *pvParameters)
{
    // Configure LED pin as OUTPUT
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    // Configure button pin as INPUT with pull-up resistor
    for (size_t i = 0; i < NUM_BUTTONS; i++)
    {
        gpio_reset_pin(button_gpios[i]);
        gpio_set_direction(button_gpios[i], GPIO_MODE_INPUT);
        gpio_set_pull_mode(button_gpios[i], GPIO_PULLDOWN_ONLY);
    }

    // Initialize last states to LOW
    int last_button_states[NUM_BUTTONS] = {0};
    for (;;)
    {
        // Read button state once per session and update bitmask
        for (uint8_t i = 0; i < NUM_BUTTONS; i++)
        {
            if (gpio_get_level(button_gpios[i]) == 1 && last_button_states[i] == 0)
            {
                buttons_pressed_bitmask |= (1 << i); // Set the bit for this button
                ESP_LOGI("BUTTON_TASK", "Button %d pressed. Bitmask: 0x%02X",
                         button_gpios[i], buttons_pressed_bitmask);
                last_button_states[i] = 1; // Update last state to HIGH
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS); // Small delay for debouncing
    }
    vTaskDelete(NULL);
}
void vSendSignal(void *pvParameters)
{

    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
        // -- Task application code here. --
    }

    /* Tasks must not attempt to return from their implementing
       function or otherwise exit. In newer FreeRTOS port
       attempting to do so will result in an configASSERT() being
       called if it is defined. If it is necessary for a task to
       exit then have the task call vTaskDelete( NULL ) to ensure
       its exit is clean. */
    vTaskDelete(NULL);
}

void app_main()
{

    BaseType_t xReturned;
    TaskHandle_t xHandle = NULL;

    /* Create the task, storing the handle. */
    // task to listen for button state
    xReturned = xTaskCreatePinnedToCore(
        vListenTask, /* Function that implements the task. */
        "NAME",      /* Text name for the task. */
        2048,        /* Stack size in words, not bytes. */
        (void *)1,   /* Parameter passed into the task. */
        1,           /* Priority at which the task is created. */
        &xHandle,
        1); /* Pin task to core 0. */

    // get the high water mark of the stack, which is the minimum amount of stack that has remained for the task since it was created. This is useful for debugging and optimizing stack usage.
    uxTaskGetStackHighWaterMark(xHandle);
    UBaseType_t stackMark = uxTaskGetStackHighWaterMark(xHandle);
    // ESP_LOGI(BUTTON13, "Stack high water mark: %u words", stackMark);
    uxTaskGetStackHighWaterMark(xHandle);

    if (xReturned == pdPASS)
    {
        /* The task was created. Use the task's handle to delete the task. */
        // vTaskDelete(xHandle);
    }
    // task to send signal from listening task
    xReturned = xTaskCreatePinnedToCore(
        vSendSignal, /* Function that implements the task. */
        "NAME",      /* Text name for the task. */
        2048,        /* Stack size in words, not bytes. */
        (void *)1,   /* Parameter passed into the task. */
        1,           /* Priority at which the task is created. */
        &xHandle,
        0); /* Pin task to core 0. */

    // get the high water mark of the stack, which is the minimum amount of stack that has remained for the task since it was created. This is useful for debugging and optimizing stack usage.

    if (xReturned == pdPASS)
    {
        /* The task was created. Use the task's handle to delete the task. */
        // vTaskDelete(xHandle);
    }
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}