#include <math.h>
#include "string.h"
#include "esp_timer.h"

#include "img_processing/follow_obj_in_img.h"

#ifndef TAG
#define TAG "follow_obj_in_img"
#endif


esp_err_t get_FOVs(
    const pixel_coordinate_t *sun_coord,
    float *FOVs /* with size 2 */
) {
    if (sun_coord->x >= FRAME_WIDTH_AND_HEIGHT || sun_coord->y >= FRAME_WIDTH_AND_HEIGHT) {
        ESP_LOGE(TAG, "get_FOVs got strange sun_coord");
        return ESP_FAIL;
    }

    int8_t diff_x = FRAME_WIDTH_AND_HEIGHT/2 - sun_coord->x;
    int8_t diff_y = FRAME_WIDTH_AND_HEIGHT/2 - sun_coord->y;

    FOVs[0] = atanf((float)diff_x / (float)pixels_focus) * (float)90 / M_PI_2;
    FOVs[1] = atanf((float)diff_y / (float)pixels_focus) * (float)90 / M_PI_2;

    ESP_LOGI("", "FOVs x: %.2f y: %.2f", FOVs[0], FOVs[1]);

    return ESP_OK;
}

static pixel_coordinate_t coords_history[3] = {
    {0, 0}, {0, 0}, {0, 0}
};


static esp_err_t copy_camera_fb(camera_fb_t *dest, camera_fb_t *src) {
    if (src == NULL) {
        ESP_LOGE("CAMERA", "src is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (dest == NULL) {
        dest = (camera_fb_t *)malloc(sizeof(camera_fb_t));
        if (dest == NULL) {
            ESP_LOGE("CAMERA", "Failed to allocate memory for dest");
            return ESP_ERR_NO_MEM;
        }
        dest->buf = (uint8_t *)malloc(src->len);
        if (dest->buf == NULL) {
            ESP_LOGE("CAMERA", "Failed to allocate memory for dest->buf");
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

static void free_camera_fb(camera_fb_t *frame) {
    free(frame->buf);
    frame->buf = NULL;
    free(frame);
}


static camera_fb_t *prev_frame = NULL;
camera_fb_t *current_frame;
// static int dt = 0;


static esp_err_t take_photo() {
    if (prev_frame != NULL) {
        free_camera_fb(prev_frame);
        prev_frame = NULL;
    }
    copy_camera_fb(prev_frame, current_frame);
    esp_camera_fb_return(current_frame);
    current_frame = esp_camera_fb_get();
    ESP_LOGI(TAG, "got fb, %d", current_frame != NULL);
    if (!current_frame)
        return ESP_FAIL;
    
    return ESP_OK;
}


static void photographer_task(void *pvParameters) {
    while (1) {
        if (take_photo())
            ESP_LOGE(TAG, "take_photo falls");
        vTaskDelay(pdMS_TO_TICKS(PHOTOGRAPHER_DELAY_MS));
    }
}


esp_err_t run_photographer() {
    current_frame = esp_camera_fb_get();
    void *pvParameters = NULL;
    if (xTaskCreate(&photographer_task,
                    "photographer_task",
                    4096,
                    pvParameters,
                    2,
                    NULL)
    ) {
                ESP_LOGE(TAG, "Failed to create photographer_task");
                return ESP_FAIL;
    }
    return ESP_OK;
}


esp_err_t compare_frames() {
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