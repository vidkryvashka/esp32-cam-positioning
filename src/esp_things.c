// without own header file, defines declarations in defs.h

#include "nvs_flash.h"
#include "esp_heap_caps.h"
#include "my_servos.h"


#include "defs.h"

#define TAG "my_esp_things"

// support IDF 5.x
#ifndef portTICK_RATE_MS
#define portTICK_RATE_MS portTICK_PERIOD_MS
#endif

// LED BLINKING moments
#define LED_PIN 33
#define BLINK_PERIOD pdMS_TO_TICKS(150)

// idk why on is 0 and off is 1
#define LED_ON()    gpio_set_level(LED_PIN, 0)
#define LED_OFF()   gpio_set_level(LED_PIN, 1)


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

    // esp_err_t servos_err = init_my_servos();
    // ESP_ERROR_CHECK(servos_err);

    esp_err_t err = gpio_err & nvs_flash_err; // & servos_err;

    return err;
}


void led_blink(float periods, uint8_t count)
{
    for (uint8_t def_i = 0; def_i < count; def_i++) {
        LED_ON();
        vTaskDelay(BLINK_PERIOD * periods);
        LED_OFF();
        vTaskDelay(BLINK_PERIOD * periods);
    }
}


void log_memory(
    BaseType_t core_id
) {
    // Масив з усіма типами пам'яті, які потрібно перевірити
    const uint32_t caps[] = {
        MALLOC_CAP_EXEC,
        MALLOC_CAP_32BIT,
        MALLOC_CAP_8BIT,
        MALLOC_CAP_DMA,
        MALLOC_CAP_PID2,        // no use ever detected
        MALLOC_CAP_PID3,        // no use ever detected
        MALLOC_CAP_PID4,        // no use ever detected
        MALLOC_CAP_PID5,        // no use ever detected
        MALLOC_CAP_PID6,        // no use ever detected
        MALLOC_CAP_PID7,        // no use ever detected
        MALLOC_CAP_SPIRAM,
        MALLOC_CAP_INTERNAL,
        MALLOC_CAP_DEFAULT,
        0
    };

    const char *cap_names[] = {
        "EXEC",
        "32BIT",
        "8BIT",
        "DMA",
        "PID2",     // no use
        "PID3",     // no use
        "PID4",     // no use
        "PID5",     // no use
        "PID6",     // no use
        "PID7",     // no use
        "SPIRAM",
        "INTERNAL",
        "DEFAULT",
        NULL
    };

    ESP_LOGI(TAG, "Memory usage for core %d %s:", core_id, core_id ? "webserver" : "photographer");     // core 0 pinned to photographer, core 1 pinned to webserver
    for (int i = 0; caps[i] != 0; i++) {
        size_t free_size = heap_caps_get_free_size(caps[i]);
        size_t total_size = heap_caps_get_total_size(caps[i]);
        size_t used_size = total_size - free_size;

        // Виводимо тільки ті типи пам'яті, які мають ненульовий розмір
        if (total_size > 0) {
            float used_percent = (float)used_size * 100 / (float)total_size;
            ESP_LOGI(TAG, "\t\t%s: used %d / %d (%.2f%%)", cap_names[i], used_size, total_size, used_percent);
        }
    }
}