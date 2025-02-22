#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_camera.h"
#include "img_converters.h"

#include "defs.h"
#include "camera.h"

#ifndef TAG
#define TAG "esp_camera"
#endif

// ESP32Cam (AiThinker) PIN Map
#ifdef BOARD_ESP32CAM_AITHINKER

#define CAM_PIN_PWDN 32
#define CAM_PIN_RESET -1 //software reset will be performed
#define CAM_PIN_XCLK 0
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27

#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 21
#define CAM_PIN_D2 19
#define CAM_PIN_D1 18
#define CAM_PIN_D0 5
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22

#endif

static camera_config_t camera_config = {
    .pin_pwdn = CAM_PIN_PWDN,
    .pin_reset = CAM_PIN_RESET,
    .pin_xclk = CAM_PIN_XCLK,
    .pin_sccb_sda = CAM_PIN_SIOD,
    .pin_sccb_scl = CAM_PIN_SIOC,

    .pin_d7 = CAM_PIN_D7,
    .pin_d6 = CAM_PIN_D6,
    .pin_d5 = CAM_PIN_D5,
    .pin_d4 = CAM_PIN_D4,
    .pin_d3 = CAM_PIN_D3,
    .pin_d2 = CAM_PIN_D2,
    .pin_d1 = CAM_PIN_D1,
    .pin_d0 = CAM_PIN_D0,
    .pin_vsync = CAM_PIN_VSYNC,
    .pin_href = CAM_PIN_HREF,
    .pin_pclk = CAM_PIN_PCLK,

    //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_RGB565, //YUV422,GRAYSCALE,RGB565,JPEG
    .frame_size = FRAMESIZE_96X96, //QQVGA-UXGA, For ESP32, do not use sizes above QVGA when not JPEG. The performance of the ESP32-S series has improved a lot, but JPEG mode always gives better frame rates.

    .jpeg_quality = 12, //0-63, for OV series camera sensors, lower number means higher quality
    .fb_count = 1,      //When jpeg mode is used, if fb_count more than one, the driver will work in continuous mode.
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
};

esp_err_t init_camera(void)
{
    //initialize the camera
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera Init Failed");
        return err;
    }

    return ESP_OK;
}

esp_err_t print_pixel_value(camera_fb_t *frame, size_t x, size_t y) {
    if (frame->format != PIXFORMAT_RGB565) {
        perror("get_pixel_value wrong PIXFORMAT");
        return -1;
    }

    if (x >= frame->width || y >= frame->height) {
        perror("Coordinates out of bounds!");
        return -1;
    }

    // frame->buf is (uint8_t *)
    uint16_t *pixel_data = (uint16_t *)frame->buf;
    // Calculate the index in the buffer
    size_t index = y * frame->width + x;
    uint16_t pixel = pixel_data[index];

    // Each pixel is 2 bytes in RGB565
    // bits per colors RRRRRGGG GGGBBBBB
    // Extract R, G, B components
    uint8_t r = (pixel >> 11) & 0x1F; // 5 bits for red
    uint8_t g = (pixel >> 5) & 0x3F;  // 6 bits for green
    uint8_t b = pixel & 0x1F;         // 5 bits for blue
    printf("RGB565 Pixel at (%zu, %zu): R=%d, G=%d, B=%d\n", x, y, r, g, b);

    return ESP_OK;
}












/*
void start_camera(void) {q
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
            bool is_converted = frame2jpg(pic, 80, &jpg_buf, &jpg_buf_len);
            esp_camera_fb_return(pic);
                // You can now send `jpg_buf` with size `jpg_buf_len` via HTTP
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
*/