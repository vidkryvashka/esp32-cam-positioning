#include "defs.h"

static esp_err_t init_gpio(void)
{
    esp_err_t err = ESP_FAIL;
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_PIN, 1);
    err = ESP_OK;
    
    return err;
}


void led_blink(float periods, uint8_t count)
{
    for (uint8_t def_i = 0; def_i < count; ++def_i) {
        LED_ON();
        vTaskDelay(BLINK_PERIOD * periods);
        LED_OFF();
        vTaskDelay(BLINK_PERIOD * periods);
    }
}


esp_err_t init_esp_things(void)
{   
    esp_err_t err = ESP_FAIL;
    err = init_gpio();

    return err;
}