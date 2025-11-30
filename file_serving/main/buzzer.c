#include "buzzer.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "BUZZER";

void buzzer_init(void)
{
    gpio_reset_pin(BUZZER_GPIO);
    gpio_set_direction(BUZZER_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(BUZZER_GPIO, 0);
}

void buzzer_trigger(void)
{
    ESP_LOGI(TAG, "Buzzer ON");
    gpio_set_level(BUZZER_GPIO, 1);
    vTaskDelay(pdMS_TO_TICKS(1000));
    gpio_set_level(BUZZER_GPIO, 0);
    ESP_LOGI(TAG, "Buzzer OFF");
}
