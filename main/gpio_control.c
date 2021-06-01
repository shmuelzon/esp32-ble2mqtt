#include "gpio_control.h"
#include "config.h"
#include "driver/gpio.h"
#include <esp_err.h>
#include <esp_log.h>
#include <cJSON.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <freertos/semphr.h>

/* Constants */
static const char *TAG = "GPIO_CONTROL";

void gpio_control_handle(char * payload)
{
    ESP_LOGD(TAG, "Incomming MQTT message for GPIO control: %s", payload);

    cJSON *msg = cJSON_Parse(payload);
    if (msg == NULL) {
        ESP_LOGE(TAG, "Unable to parse GPIO control message: %s", payload);
        return;
    }

    cJSON *msg_pin = cJSON_GetObjectItemCaseSensitive(msg, "pin");
    if (msg_pin == NULL) {
        ESP_LOGE(TAG, "Invalid format of GPIO control message: %s", payload);
        return;
    }

    int gpio_pin = msg_pin->valueint;
    if (gpio_pin <= 0) {
        ESP_LOGE(TAG, "Invalid GPIO port passed: %d", gpio_pin);
        return;
    }

    if (!GPIO_IS_VALID_OUTPUT_GPIO(gpio_pin)) {
        ESP_LOGE(TAG, "GPIO port %d is not an output GPIO.", gpio_pin);
        return;
    }

    // Ensure GPIO is in output mode
    gpio_set_direction(gpio_pin, GPIO_MODE_OUTPUT);

    // Perform state transition based on JSON message
    int gpio_state = 0;
    cJSON *msg_state = cJSON_GetObjectItemCaseSensitive(msg, "state");
    if (msg_state != NULL) {
        gpio_state = msg_state->valueint;
    }

    int gpio_delay = 400;
    cJSON *msg_delay = cJSON_GetObjectItemCaseSensitive(msg, "delay");
    if (msg_delay != NULL) {
        gpio_delay = msg_delay->valueint;
    }

    switch (gpio_state) {
        case -1:
            // Turn on the PIN for the duration of the delay
            gpio_set_level(gpio_pin, 1);
            vTaskDelay(gpio_delay / portTICK_PERIOD_MS);
            gpio_set_level(gpio_pin, 0);
            break;
        case 1:
            gpio_set_level(gpio_pin, 1);
            break;
        default:
            // By default we turn the pin LOW
            gpio_set_level(gpio_pin, 0);
            break;
    }
}
