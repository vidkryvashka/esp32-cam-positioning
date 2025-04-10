#include <math.h>
#include "string.h"
#include "esp_timer.h"

#include "img_processing/photographer.h"

#ifndef TAG
#define TAG "my_photographer"
#endif

// // for prediction, Kalman filter, not implemented yet
// static pixel_coordinate_t coords_history[3] = {
//     {0, 0}, {0, 0}, {0, 0}
// };


volatile SemaphoreHandle_t frame_mutex;
camera_fb_t *current_frame = NULL;
// static int dt = 0;


static esp_err_t take_photo()
{
    if (xSemaphoreTake(frame_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to take frame_mutex");
        return ESP_FAIL;
    }
    if (current_frame != NULL)
        esp_camera_fb_return(current_frame);
    current_frame = esp_camera_fb_get();
    xSemaphoreGive(frame_mutex);
    ESP_LOGI(TAG, "took photo%s", current_frame != NULL ? ", got fb, nice" : ", fail");
    if (!current_frame)
        return ESP_FAIL;
    
    return ESP_OK;
}


volatile bool pause_photographer = 0;   // extern declared in img_processing/photographer.h, used in webserver.c

static void photographer_task(void *pvParameters)
{
    while (1) {
        if (!pause_photographer) {
            if (take_photo())
                ESP_LOGE(TAG, "couldn't take_photo");
        } else
            ESP_LOGI(TAG, "photographer_task paused");
        vTaskDelay(pdMS_TO_TICKS(PHOTOGRAPHER_DELAY_MS));
    }
}


esp_err_t run_photographer()
{
    frame_mutex = xSemaphoreCreateMutex();
    current_frame = esp_camera_fb_get();
    if (xTaskCreate(&photographer_task,
                    "photographer_task",
                    4096,
                    NULL,
                    2,
                    NULL)
    ) {
        ESP_LOGE(TAG, "Failed to create photographer_task");
        return ESP_FAIL;
    }
    return ESP_OK;
}


// void primitive_photographer()
// {
//     float FOVs[2] = {0, 0};
//     while(1) {
//         camera_fb_t *frame = esp_camera_fb_get();
//         max_brightness_pixels_t *mbp = mark_sun(frame);
//         get_FOVs(&mbp->center_coord, FOVs);
//
//         esp_camera_fb_return(frame);
//         vTaskDelay(pdMS_TO_TICKS(1000));
//     }
// }