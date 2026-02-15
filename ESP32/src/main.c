#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define LED_GPIO 4     // LED
#define BUTTON_GPIO 13 // Button

static const char *BUTTON1 = "BUTTON_1";

void app_main()
{

    // Configure LED pin as OUTPUT
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    // Configure BUTTON pin as INPUT with pull-down resistor
    gpio_reset_pin(BUTTON_GPIO);
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_GPIO, GPIO_PULLDOWN_ONLY);

    int prev_button_state = -1;

    while (1)
    {
        // Read button state (HIGH when pressed, LOW when not pressed)
        int button_state = gpio_get_level(BUTTON_GPIO);

        // Set LED to match button state
        gpio_set_level(LED_GPIO, button_state);

        // Log when button state changes
        if (button_state != prev_button_state)
        {
            ESP_LOGI(BUTTON1, "%s -> LED %s",
                     button_state ? "PRESSED" : "RELEASED",
                     button_state ? "ON" : "OFF");
            prev_button_state = button_state;
        }

        vTaskDelay(10 / portTICK_PERIOD_MS); // Small delay for debouncing
    }
}