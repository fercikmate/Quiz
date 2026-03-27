#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define LED_GPIO 4     // LED
#define BUTTON_GPIO 13 // Button

static const char *BUTTON1 = "BUTTON_1";

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

    // // Configure LED pin as OUTPUT
    // gpio_reset_pin(LED_GPIO);
    // gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    // // Configure BUTTON pin as INPUT with pull-down resistor
    // gpio_reset_pin(BUTTON_GPIO);
    // gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
    // gpio_set_pull_mode(BUTTON_GPIO, GPIO_PULLDOWN_ONLY);

    // int prev_button_state = -1;

    BaseType_t xReturned;
    TaskHandle_t xHandle = NULL;
    UBaseType_t uxCoreAffinityMask;

    /* Create the task, storing the handle. */
    xReturned = xTaskCreate(
        vListenTask, /* Function that implements the task. */
        "NAME",      /* Text name for the task. */
        2048,        /* Stack size in words, not bytes. */
        (void *)1,   /* Parameter passed into the task. */
        1,           /* Priority at which the task is created. */
        &xHandle);   /* Used to pass out the created task's handle. */
    // set the task to run on explicitly on core 0
    uxCoreAffinityMask = ((1 << 0));
    vTaskCoreAffinitySet(xHandle, uxCoreAffinityMask);

    // get the high water mark of the stack, which is the minimum amount of stack that has remained for the task since it was created. This is useful for debugging and optimizing stack usage.

    uxTaskGetStackHighWaterMark(xHandle);

    if (xReturned == pdPASS)
    {
        /* The task was created. Use the task's handle to delete the task. */
        vTaskDelete(xHandle);
    }

    xReturned = xTaskCreate(
        vSendSignal, /* Function that implements the task. */
        "NAME",      /* Text name for the task. */
        2048,        /* Stack size in words, not bytes. */
        (void *)1,   /* Parameter passed into the task. */
        1,           /* Priority at which the task is created. */
        &xHandle);   /* Used to pass out the created task's handle. */
    // set the task to run on explicitly on core 0
    uxCoreAffinityMask = ((1 << 0));
    vTaskCoreAffinitySet(xHandle, uxCoreAffinityMask);

    // get the high water mark of the stack, which is the minimum amount of stack that has remained for the task since it was created. This is useful for debugging and optimizing stack usage.

    uxTaskGetStackHighWaterMark(xHandle);

    if (xReturned == pdPASS)
    {
        /* The task was created. Use the task's handle to delete the task. */
        vTaskDelete(xHandle);
    }

    while (1)
    {
        // // Read button state (HIGH when pressed, LOW when not pressed)
        // int button_state = gpio_get_level(BUTTON_GPIO);

        // // Set LED to match button state
        // gpio_set_level(LED_GPIO, button_state);

        // // Log when button state changes
        // if (button_state != prev_button_state)
        // {
        //     ESP_LOGI(BUTTON1, "%s -> LED %s",
        //              button_state ? "PRESSED" : "RELEASED",
        //              button_state ? "ON" : "OFF");
        //     prev_button_state = button_state;
        // }

        // vTaskDelay(10 / portTICK_PERIOD_MS); // Small delay for debouncing
    }
}