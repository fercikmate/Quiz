#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define LED_GPIO 4       // LED
#define BUTTON_GPIO13 13 // Button
#define BUTTON_GPIO27 27 // Button
#define BUTTON_GPIO26 26 // Button
#define BUTTON_GPIO25 25 // Button
#define BUTTON_GPIO32 32 // Button

static const char *BUTTON13 = "BUTTON_13";
static const char *BUTTON27 = "BUTTON_27";
static const char *BUTTON26 = "BUTTON_26";
static const char *BUTTON25 = "BUTTON_25";
static const char *BUTTON32 = "BUTTON_32";

void vListenTask(void *pvParameters)
{

    for (;;)
    {
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
void vSendSignal(void *pvParameters)
{

    for (;;)
    {
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

    // Configure LED pin as OUTPUT
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    // Configure BUTTON pin as INPUT with pull-down resistor
    gpio_reset_pin(BUTTON_GPIO13);
    gpio_set_direction(BUTTON_GPIO13, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_GPIO13, GPIO_PULLDOWN_ONLY);

    int prev_button_state = -1;

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
        0); /* Pin task to core 0. */

    // get the high water mark of the stack, which is the minimum amount of stack that has remained for the task since it was created. This is useful for debugging and optimizing stack usage.
    uxTaskGetStackHighWaterMark(xHandle);
    UBaseType_t stackMark = uxTaskGetStackHighWaterMark(xHandle);
    ESP_LOGI(BUTTON13, "Stack high water mark: %u words", stackMark);
    uxTaskGetStackHighWaterMark(xHandle);

    if (xReturned == pdPASS)
    {
        /* The task was created. Use the task's handle to delete the task. */
        vTaskDelete(xHandle);
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
        vTaskDelete(xHandle);
    }
    uxTaskGetStackHighWaterMark(xHandle);
    ESP_LOGI(BUTTON13, "Stack high water mark: %u words", stackMark);
    while (1)
    {
        // Read button state (HIGH when pressed, LOW when not pressed)
        int button_state = gpio_get_level(BUTTON_GPIO13);

        // Set LED to match button state
        gpio_set_level(LED_GPIO, button_state);

        // Log when button state changes
        if (button_state != prev_button_state)
        {
            ESP_LOGI(BUTTON13, "%s -> LED %s",
                     button_state ? "PRESSED" : "RELEASED",
                     button_state ? "ON" : "OFF");
            prev_button_state = button_state;
        }

        vTaskDelay(10 / portTICK_PERIOD_MS); // Small delay for debouncing
    }
}