#include <math.h>
#include "string.h"
#include "esp_timer.h"
#include "esp_random.h"

#include "img_processing/photographer.h"
#if ANALISIS_MODE == MODE_FIND_SUN
    #include "img_processing/find_sun.h"
#elif ANALISIS_MODE == MODE_FAST9
    #include "img_processing/vision_defs.h"
#endif
#include "my_servos.h"


#define TAG "my_photographer"


volatile bool pause_photographer = 0;   // extern declared in img_processing/photographer.h, used in webserver.c
volatile SemaphoreHandle_t frame_mutex;
camera_fb_t *current_frame = NULL;      // extern declared in camera.h
static camera_fb_t *fragment = NULL;
QueueHandle_t servo_queue;
static keypoints_shell_t keypoints_shell;



keypoints_shell_t* get_keypoints_shell_reference()
{
    return &keypoints_shell;
}


camera_fb_t* operate_fragment(
    const rectangle_coords_t *rect
) {
    ESP_LOGI(TAG, "Rectangle coordinates: Top left: (%d, %d), width: %d, height: %d)", 
            rect->top_left.x, rect->top_left.y, rect->width, rect->height);
    
    if (fragment != NULL) {
        ESP_LOGI(TAG, "free fragment on replace ");
        camera_fb_free(fragment);
    }
    
    fragment = camera_fb_crop(current_frame, rect);

    // keypoints_shell.need2ORB = 1;
    ESP_LOGI(TAG, "Unpaused");

    return fragment;
}


static esp_err_t take_analize_photo()
{
    if (xSemaphoreTake(frame_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        ESP_LOGE(TAG, "take_photo() failed to take frame_mutex ");
        // log_memory(1);  // another (webserver) core
        return ESP_FAIL;
    }

    if (current_frame != NULL)
        esp_camera_fb_return(current_frame);

    current_frame = esp_camera_fb_get();
    angles_diff_t angles_diff = {0,0};
    // float angles[2] = {0,0};

#if ANALISIS_MODE == MODE_FIND_SUN
    mark_sun(&keypoints_shell.mbp, current_frame, &angles_diff);
#elif ANALISIS_MODE == MODE_FAST9
    find_drone(current_frame, fragment, &keypoints_shell.pixels_cloud);
#endif

    xSemaphoreGive(frame_mutex);

    if (are_servos_inited && (abs(angles_diff.pan) > ANGLE_THRESHOLD || abs(angles_diff.tilt) > ANGLE_THRESHOLD))
        if (xQueueSend(servo_queue, (void *)&angles_diff, 10) != pdTRUE) {
            ESP_LOGE(TAG, "\t\t--- servo_actions couldn't xQueueSend, queue fill ");
        }
    
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
        .pixels_cloud = {
            .coords = vector_create(sizeof(pixel_coord_t)),
            .center_coord = (pixel_coord_t){0, 0}
            // .need2ORB = 0
        },
    };
    if (xTaskCreatePinnedToCore(
                    &photographer_task,
                    "photographer_task",
                    1 << 14, // = 16384
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
