#include <math.h>
#include "string.h"
#include "esp_timer.h"

#include "img_processing/photographer.h"

#ifndef TAG
#define TAG "my_photographer"
#endif



// static pixel_coordinate_t coords_history[3] = {
//     {0, 0}, {0, 0}, {0, 0}
// };


int8_t frames_avialable = 0;

static esp_err_t copy_camera_fb(camera_fb_t *dest, camera_fb_t *src)
{
    if (src == NULL) {
        ESP_LOGE(TAG, "copy_camera_fb src is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (dest == NULL) {
        dest = (camera_fb_t *)malloc(sizeof(camera_fb_t));
        if (dest == NULL) {
            ESP_LOGE(TAG, "copy_camera_fb failed to allocate memory for dest");
            return ESP_ERR_NO_MEM;
        }
        dest->buf = (uint8_t *)malloc(src->len);
        if (dest->buf == NULL) {
            ESP_LOGE(TAG, "copy_camera_fb failed to allocate memory for dest->buf");
            free(dest);
            dest = NULL;
            return ESP_ERR_NO_MEM;
        }
    }

    dest->len = src->len;
    dest->width = src->width;
    dest->height = src->height;
    dest->format = src->format;
    dest->timestamp = src->timestamp;

    memcpy(dest->buf, src->buf, src->len);

    return ESP_OK;
}


static void free_camera_fb(camera_fb_t *frame)
{
    ESP_LOGI(TAG, "free_camera_fb");
    free(frame->buf);
    frame->buf = NULL;
    free(frame);
}


SemaphoreHandle_t frame_mutex = NULL;
static camera_fb_t *prev_frame = NULL;
camera_fb_t *current_frame = NULL;
// static int dt = 0;

static esp_err_t take_photo()
{
    // ESP_LOGI(TAG, "called take_photo");
    if (xSemaphoreTake(frame_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to take frame_mutex");
        return ESP_FAIL;
    }

    if (prev_frame != NULL) {
        -- frames_avialable;
        free_camera_fb(prev_frame);
        prev_frame = NULL;
    }
    copy_camera_fb(prev_frame, current_frame);
    esp_camera_fb_return(current_frame);

    camera_fb_t *new_frame = esp_camera_fb_get();
    if (!new_frame) {
        ESP_LOGE(TAG, "Failed to get new frame");
        xSemaphoreGive(frame_mutex);
        return ESP_FAIL;
    }

    current_frame = new_frame;
    ++ frames_avialable;

    ESP_LOGI(TAG, "took photo, got fb, %s, frames_avialable %d",
             current_frame != NULL ? "nice" : "FAIL", frames_avialable);

    xSemaphoreGive(frame_mutex);
    return ESP_OK;
}


// bool pause_photographer;     // extern declared in img_processing/follow_object_on_img.h

static void photographer_task(void *pvParameters)
{
    ESP_LOGI(TAG, "start photographer_task");

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
    ESP_LOGI(TAG, "start run_photographer");
    pause_photographer = 0;
    current_frame = NULL;
    frame_mutex = xSemaphoreCreateMutex();
    BaseType_t e = xTaskCreate(&photographer_task,
                    "photographer_task",
                    4096,
                    NULL,
                    2,
                    NULL);
    if (e) {
        ESP_LOGE(TAG, "Failed to create photographer_task, e %d", e);
        return ESP_FAIL;
    }
    return ESP_OK;
}


esp_err_t compare_frames()
{
    return 0;
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