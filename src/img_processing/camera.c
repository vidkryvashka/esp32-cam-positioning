#include <math.h>
#include <string.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_camera.h"
#include "img_converters.h"

#include "defs.h"
#include "img_processing/camera.h"

#define TAG "my_camera"


static camera_config_t camera_config = {
    .pin_pwdn = 32,
    .pin_reset = -1,    // software reset will be performed
    .pin_xclk = 0,
    .pin_sccb_sda = 26,
    .pin_sccb_scl = 27,

    .pin_d7 = 35,
    .pin_d6 = 34,
    .pin_d5 = 39,
    .pin_d4 = 36,
    .pin_d3 = 21,
    .pin_d2 = 19,
    .pin_d1 = 18,
    .pin_d0 = 5,
    .pin_vsync = 25,
    .pin_href = 23,
    .pin_pclk = 22,

    //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_CHOISE, // RGB565, //YUV422,GRAYSCALE,RGB565,JPEG

    // For ESP32, do not use sizes above QVGA when not JPEG. The performance of the ESP32-S series has improved a lot, but JPEG mode always gives better frame rates.
    /** considering cases
     * FRAMESIZE_96X96
     * FRAMESIZE_240X240
     * FRAMESIZE_QVGA   // 320x240
     * FRAMESIZE_320X320    // crashing
     */
    .frame_size = FRAMESIZE_240X240,

    .jpeg_quality = 12, // 0-63, for OV series camera sensors, lower number means higher quality
    .fb_count = 1,      // When jpeg mode is used, if fb_count more than one, the driver will work in continuous mode.
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY
};

uint16_t frame_width;
uint16_t frame_height;

esp_err_t init_camera(void)
{
    esp_err_t err = esp_camera_init(&camera_config);
    sensor_t *s = esp_camera_sensor_get();
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera Init Failed");
        return err;
    }

    switch (camera_config.frame_size) {
        case FRAMESIZE_96X96:
            frame_width = frame_height = 96;
            break;
        case FRAMESIZE_240X240:
            frame_width = frame_height = 240;
            break;

        case FRAMESIZE_QVGA:
            frame_width = 320;
            frame_height = 240;
            break;
        
        case FRAMESIZE_320X320:
            frame_width = frame_height= 320;
            break;
        
        default:
            ESP_LOGE(TAG, "init_camera unpredicted framesize");
            return ESP_FAIL;
            break;
    }

    return ESP_OK;
}


esp_err_t calculate_angles_diff(
    const pixel_coord_t *coord,
    angles_diff_t *angles_diff
) {
    if (coord->x >= frame_width || coord->y >= frame_height) {
        ESP_LOGE(TAG, "get_angles got strange coord");
        return ESP_FAIL;
    }

    int16_t diff_x = frame_width / 2 - coord->x;
    int16_t diff_y = frame_height / 2 - coord->y;

    const float tan26_degrees = 0.4877f;

    angles_diff->pan = (int16_t)(atanf((double)diff_x / (double)frame_width / (double)2 / tan26_degrees) * (double)90 / -M_PI_2); // mirroring
    angles_diff->tilt = (int16_t)(atanf((double)diff_y / (double)frame_height / (double)2 / tan26_degrees) * (double)90 / M_PI_2);

    ESP_LOGI(TAG, "angles_diff x: %d° y: %df°", angles_diff->pan, angles_diff->tilt);

    return ESP_OK;
}