/**
 * This example takes a picture every 5s and print its size on serial monitor.
 */

#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// support IDF 5.x
#ifndef portTICK_RATE_MS
#define portTICK_RATE_MS portTICK_PERIOD_MS
#endif

#include "defs.h"
#include "camera.h"
#include "webserver.h"

void server_up(void) {
        // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    connect_wifi();

    // GPIO initialization
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    led_state = 0;
    ESP_LOGI(TAG, "LED Control Web Server is running ... ...\n");
    setup_server();
    ESP_LOGI(TAG, "server is up, good boiii");
}

void start_camera(void) {
#if ESP_CAMERA_SUPPORTED
    if(ESP_OK != init_camera()) {
        return;
    }

    while (1)
    {
        // ESP_LOGI(TAG, "Taking picture...");
        camera_fb_t *pic = esp_camera_fb_get();

        if (pic->format == PIXFORMAT_RGB565) {
            size_t jpg_buf_len = 0;
            uint8_t *jpg_buf = NULL;

            // Convert the RGB565 image to JPEG
            bool converted = frame2jpg(pic, 80, &jpg_buf, &jpg_buf_len);
            if (converted) {
                // You can now send `jpg_buf` with size `jpg_buf_len` via HTTP

                esp_camera_fb_return(pic);
            } else {
                // Conversion failed
                esp_camera_fb_return(pic);
            }
        }
        // use pic->buf to access the image
        ESP_LOGI(TAG, "Picture taken! Its size was: %zu bytes, width: %zu heigth: %zu format: %d",
            pic->len, pic->width, pic->height, pic->format);

        vTaskDelay(5000 / portTICK_RATE_MS);
    }
#else
    ESP_LOGE(TAG, "Camera support is not available for this chip");
    return;
#endif
}

void app_main(void)
{
    server_up();

    // start_camera();
}
