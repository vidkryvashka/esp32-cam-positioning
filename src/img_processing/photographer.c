#include <math.h>
#include "string.h"
#include "esp_timer.h"
#include "esp_random.h"

#include "img_processing/photographer.h"
#include "img_processing/ORB_defs.h"

#ifndef TAG
#define TAG "my_photographer"
#endif


volatile bool pause_photographer = 0;   // extern declared in img_processing/photographer.h, used in webserver.c
volatile SemaphoreHandle_t frame_mutex;
camera_fb_t *current_frame = NULL;      // extern declared in camera.h


static keypoints_shell_t keypoints_shell = {NULL, 0, 0};

keypoints_shell_t* get_keypoints_shell_reference() {
    return &keypoints_shell;
}

static camera_fb_t *fragment = NULL;

camera_fb_t* decorate_fragment(
    const rectangle_coords_t *rect_coords
) {
    ESP_LOGI(TAG, "Rectangle coordinates: Top left: (%d, %d), width: %d, height: %d)", 
            rect_coords->top_left.x, rect_coords->top_left.y, rect_coords->width, rect_coords->height);
    
    if (fragment != NULL)
        camera_fb_free(fragment);
    
    fragment = camera_fb_create(rect_coords->width, rect_coords->height, current_frame->format);
    camera_fb_crop(fragment, current_frame, rect_coords);

    keypoints_shell.need2ORB = 1;
    pause_photographer = 0;
    ESP_LOGI(TAG, "Unpaused");

    return fragment;
}



static esp_err_t take_photo()
{
    if (xSemaphoreTake(frame_mutex, pdMS_TO_TICKS(5000)) != pdTRUE) {
        ESP_LOGE(TAG, "take_photo() failed to take frame_mutex ");
        return ESP_FAIL;
    }
    if (current_frame != NULL) {
        esp_camera_fb_return(current_frame);
    }
    current_frame = esp_camera_fb_get();
    xSemaphoreGive(frame_mutex);
    ESP_LOGI(TAG, "take_photo%s", current_frame != NULL ? ", got fb" : ", fail");
    if (!current_frame)
        return ESP_FAIL;
    
    return ESP_OK;
}


static void photographer_task(void *pvParameters)
{
    while (1) {
        if (!pause_photographer) {
            if (take_photo())
                ESP_LOGE(TAG, "couldn't take_photo ");
        } else {
            ESP_LOGI(TAG, "photographer_task paused ");
            while (pause_photographer)
                vTaskDelay(pdMS_TO_TICKS(100));
        }

        if (keypoints_shell.need2ORB | 1) {
            if (xSemaphoreTake(keypoints_shell.mutex, pdMS_TO_TICKS(1000)) != pdTRUE)
                ESP_LOGE(TAG, "photographer_task failed to take keypoints_shell.mutex ");
            find_fragment(current_frame, fragment, NULL, keypoints_shell.keypoints);
            xSemaphoreGive(keypoints_shell.mutex);
            keypoints_shell.need2ORB = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(PHOTOGRAPHER_DELAY_MS));

        if (!(esp_random() % MEMORY_LOG_PROBABILITY_DIVIDER))
            log_memory(xPortGetCoreID());
    }
}


esp_err_t run_photographer()
{
    frame_mutex = xSemaphoreCreateMutex();
    current_frame = esp_camera_fb_get();
    keypoints_shell = (keypoints_shell_t){
        .keypoints = vector_create(sizeof(pixel_coord_t)),
        .mutex = xSemaphoreCreateMutex(),
        0
    };
    // if (xTaskCreate(&photographer_task,
    if (xTaskCreatePinnedToCore(
                    &photographer_task,
                    "photographer_task",
                    2 << 13, // = 16384, // 2 << 12 = 8192,
                    NULL,
                    2,
                    NULL,
                    0 )
    ) {
        ESP_LOGE(TAG, "Failed to create photographer_task");
        return ESP_FAIL;
    }
    return ESP_OK;
}















// /**
//  * @brief Data structure of camera frame buffer
//  */
// typedef struct {
//     uint8_t * buf;              /*!< Pointer to the pixel data */
//     size_t len;                 /*!< Length of the buffer in bytes */
//     size_t width;               /*!< Width of the buffer in pixels */
//     size_t height;              /*!< Height of the buffer in pixels */
//     pixformat_t format;         /*!< Format of the pixel data */
//     struct timeval timestamp;   /*!< Timestamp since boot of the first DMA buffer of the frame */
// } camera_fb_t;
