#include <math.h>
#include "string.h"
#include "esp_timer.h"
#include "esp_random.h"

#include "img_processing/photographer.h"
#include "img_processing/operating_with_fb.h"
// #include "img_processing/recognizer.h"

#ifndef TAG
#define TAG "my_photographer"
#endif


volatile bool pause_photographer = 0;   // extern declared in img_processing/photographer.h, used in webserver.c
volatile SemaphoreHandle_t frame_mutex;
camera_fb_t *current_frame = NULL;      // extern declared in camera.h


// for prediction, Kalman filter, not implemented yet
#define MAX_COORDS_AMOUNT 3
uint8_t coords_amount = 0;
static pixel_coordinate_t coords_history[MAX_COORDS_AMOUNT] = {
    {0, 0}, {0, 0}, {0, 0}
};


static camera_fb_t *fragment = NULL; 

camera_fb_t * write_fragment(rectangle_coords_t rect_coords)
{
    ESP_LOGI(TAG, "Rectangle coordinates: top teft: %d %d, width: %d, height: %d)",
             rect_coords.top_left.x, rect_coords.top_left.y, rect_coords.width, rect_coords.height);
    if (fragment != NULL)
        camera_fb_free(fragment);
    fragment = camera_fb_crop(current_frame, &rect_coords);
    coords_history[coords_amount] = (pixel_coordinate_t) {
        .x = rect_coords.top_left.x + rect_coords.width / 2,
        .y = rect_coords.top_left.y + rect_coords.height / 2
    };

    // float similarity = 0;
    // pixel_coordinate_t similar_coord = {0, 0};
    // find_fragment(current_frame, fragment, &similarity, &similar_coord);
    // ESP_LOGI(TAG, "find_fragment found similarity %.2f ", similarity);

    // // crashes

    pause_photographer = 0;
    ESP_LOGI(TAG, "Unpause");

    return fragment;
}


// static int dt = 0;   // for prediction


static esp_err_t take_photo()
{
    if (xSemaphoreTake(frame_mutex, pdMS_TO_TICKS(5000)) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to take frame_mutex ");
        return ESP_FAIL;
    }
    if (current_frame != NULL) {
        esp_camera_fb_return(current_frame);
    }
    current_frame = esp_camera_fb_get();
    xSemaphoreGive(frame_mutex);
    ESP_LOGI(TAG, "took photo%s", current_frame != NULL ? ", got fb, nice" : ", fail");
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
            ESP_LOGI(TAG, "photographer_task paused");
            while (pause_photographer)
                vTaskDelay(pdMS_TO_TICKS(100));
        }
        vTaskDelay(pdMS_TO_TICKS(PHOTOGRAPHER_DELAY_MS));
        if (!(esp_random() % 3))
            log_memory();
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
