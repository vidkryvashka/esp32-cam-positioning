#include <math.h>
#include "string.h"
#include "esp_timer.h"
#include "esp_random.h"

#include "img_processing/photographer.h"

#if ANALISIS_MODE == MODE_FIND_SUN
    #include "img_processing/ORB_defs.h"
#endif

#if ANALISIS_MODE == MODE_FIND_SUN
    #include "img_processing/find_sun.h"
#endif

#ifndef TAG
    #define TAG "my_photographer"
#endif


volatile bool pause_photographer = 0;   // extern declared in img_processing/photographer.h, used in webserver.c
volatile SemaphoreHandle_t frame_mutex;
camera_fb_t *current_frame = NULL;      // extern declared in camera.h


static keypoints_shell_t keypoints_shell; // = {NULL, 0};

keypoints_shell_t* get_keypoints_shell_reference()
{
    // vTaskDelay(pdMS_TO_TICKS(PHOTOGRAPHER_DELAY_MS*2));
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

    // keypoints_shell.need2ORB = 1;
    pause_photographer = 0;
    ESP_LOGI(TAG, "Unpaused");

    return fragment;
}


static esp_err_t take_analize_photo()
{
    if (xSemaphoreTake(frame_mutex, pdMS_TO_TICKS(PHOTOGRAPHER_DELAY_MS * 10)) != pdTRUE) {
        ESP_LOGE(TAG, "take_photo() failed to take frame_mutex ");
        return ESP_FAIL;
    }

    if (current_frame != NULL)
        esp_camera_fb_return(current_frame);

    current_frame = esp_camera_fb_get();

#if ANALISIS_MODE == MODE_FIND_SUN
    mark_sun(&keypoints_shell.mbp, current_frame);
#elif ANALISIS_MODE == MODE_FAST9
    find_fragment(current_frame, fragment, NULL, keypoints_shell.keypoints);
#endif

    xSemaphoreGive(frame_mutex);

    ESP_LOGI(TAG, "take_photo%s", current_frame != NULL ? ", got fb" : ", fail");

    if (!current_frame)
        return ESP_FAIL;
    
    return ESP_OK;
}


static void photographer_task(
    void *pvParameters
) {
    while (1) {
        if (!pause_photographer) {
            if (take_analize_photo() != ESP_OK)
                ESP_LOGE(TAG, "task couldn't take_photo ");
            
        } else {
            ESP_LOGI(TAG, "photographer_task paused ");
            while (pause_photographer)
                vTaskDelay(pdMS_TO_TICKS(100)); // keep stable
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
#if ANALISIS_MODE == MODE_FIND_SUN
        .mbp = {
            .coords = vector_create(sizeof(pixel_coord_t)),
            .center_coord = (pixel_coord_t){0, 0}
        },
#else
        .keypoints = vector_create(sizeof(pixel_coord_t)),
#endif
        .need2ORB = 0
    };
    if (xTaskCreatePinnedToCore(
                    &photographer_task,
                    "photographer_task",
                    2 << 13, // = 16384, // 2 << 12 = 8192,
                    NULL,
                    5,
                    NULL,
                    0 )
    ) {
        ESP_LOGE(TAG, "Failed to create photographer_task");
        return ESP_FAIL;
    }
    return ESP_OK;
}
