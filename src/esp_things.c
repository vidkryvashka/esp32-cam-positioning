// without own eader file, defines declarations in defs.h



#include "nvs_flash.h"


#include "defs.h"

#ifndef TAG
#define TAG "my_esp_things"
#endif

// support IDF 5.x
#ifndef portTICK_RATE_MS
#define portTICK_RATE_MS portTICK_PERIOD_MS
#endif

// LED BLINKING moments
#define LED_PIN 33
#define BLINK_PERIOD pdMS_TO_TICKS(150)



static esp_err_t init_gpio(void)
{
    esp_err_t err = ESP_FAIL;
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_PIN, 1);
    err = ESP_OK;
    
    return err;
}


esp_err_t init_esp_things(void)
{   
    // esp_err_t err = ESP_FAIL;
    esp_err_t gpio_err = init_gpio();
    ESP_ERROR_CHECK(gpio_err);

    // Initialize NVS
    esp_err_t nvs_flash_err = nvs_flash_init();
    if (nvs_flash_err == ESP_ERR_NVS_NO_FREE_PAGES || nvs_flash_err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvs_flash_err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(nvs_flash_err);

    esp_err_t err = gpio_err && nvs_flash_err;

    return err;
}


void led_blink(float periods, uint8_t count)
{
    for (uint8_t def_i = 0; def_i < count; ++ def_i) {
        LED_ON();
        vTaskDelay(BLINK_PERIOD * periods);
        LED_OFF();
        vTaskDelay(BLINK_PERIOD * periods);
    }
}


void log_memory()
{
    size_t free_size = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    size_t total_size = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
    size_t used_size = total_size - free_size;
    ESP_LOGI(TAG, "  --  memory used %d / %d (%.2f %%) ", used_size, total_size,
        (float)used_size * 100 / (float)total_size);
}
