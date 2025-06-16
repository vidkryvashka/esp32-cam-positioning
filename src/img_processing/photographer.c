#include <math.h>
#include "string.h"
#include "esp_timer.h"
#include "esp_random.h"

#include "defs.h"
#include "img_processing/photographer.h"
#include "img_processing/vision_defs.h"
#include "my_servos.h"


#define TAG "my_photographer"


#define LOCAL_LOG_LEVEL     0


// volatile bool pause_photographer = 0;   // extern declared in img_processing/photographer.h, used in webserver.c
volatile SemaphoreHandle_t frame_mutex;
camera_fb_t *current_frame = NULL;      // extern declared in camera.h
QueueHandle_t servo_queue = 0;
static keypoints_shell_t keypoints_shell;


keypoints_shell_t* get_keypoints_shell_reference(
    void
) {
    return &keypoints_shell;
}


static esp_err_t mamage_positioning(
    void
) {
    esp_err_t err = ESP_OK;

    angles_diff_t angles_diff = {0,0};
    if (keypoints_shell.pixels_cloud.center_coord.x != 0 || keypoints_shell.pixels_cloud.center_coord.y != 0)
        err |=  calculate_angles_diff(&keypoints_shell.pixels_cloud.center_coord, &angles_diff);
    else
        return ESP_OK;

    if (err) {
        ESP_LOGE(TAG, "mamage_positioning \t got calculate_angles_diff err ");
        return ESP_FAIL;
    }

    if (are_servos_inited && (abs(angles_diff.pan) > ANGLE_THRESHOLD || abs(angles_diff.tilt) > ANGLE_THRESHOLD))
        if (xQueueSend(servo_queue, (void *)&angles_diff, 50) != pdTRUE) {
            ESP_LOGE(TAG, "mamage_positioning \t\t --- couldn't xQueueSend, queue fill ");
        }

    return err;
}


/**
 * @brief highly loaded function: takes photo, analyses, calls servo managing
 * 
 * @return esp_err_t
 */
static esp_err_t take_analize_photo(
    void
) {
    esp_err_t err = ESP_OK;

    if (xSemaphoreTake(frame_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        ESP_LOGE(TAG, "take_analize_photo \t\t failed to take frame_mutex ");
        return ESP_FAIL;
    }

    if (current_frame != NULL)
        esp_camera_fb_return(current_frame);
    
    #if LOCAL_LOG_LEVEL
        ESP_LOGI(TAG, "take_analize_photo \t returned frame, now take ");
    #endif

    current_frame = esp_camera_fb_get();
    if (!current_frame) {
        xSemaphoreGive(frame_mutex);
        ESP_LOGI(TAG, "take_analize_photo \t couldn't esp_camera_fb_get() ");
        return ESP_FAIL;
    }
    
    #if LOCAL_LOG_LEVEL
        ESP_LOGI(TAG, "take_analize_photo \t took frame ");
    #endif

    #if ANALYSIS_MODE == MODE_FIND_SUN
        err |= mark_sun(&keypoints_shell.pixels_cloud, current_frame);
    #elif ANALYSIS_MODE == MODE_FAST9
        err |= find_drone(current_frame, &keypoints_shell.pixels_cloud);
    #endif
    
    xSemaphoreGive(frame_mutex);

    err |= mamage_positioning();

    if (err)
        ESP_LOGW(TAG, "take_analize_photo \t mark_sun || find drone failed ");

    
    return ESP_OK;
}


/**
 * @brief loops take_analize_photo, sometimes logs memory states
 * 
 * @param pvParameters 
 */
static void photographer_task(
    void *pvParameters
) {
    while (1) {
        if (take_analize_photo() != ESP_OK)
            ESP_LOGE(TAG, "task couldn't take_photo ");

        vTaskDelay(pdMS_TO_TICKS(PHOTOGRAPHER_DELAY_MS));

        if (!(esp_random() % MEMORY_LOG_PROBABILITY_DIVIDER))
            log_memory(xPortGetCoreID());
    }
}


esp_err_t run_photographer(
    void
) {
    frame_mutex = xSemaphoreCreateMutex();
    current_frame = esp_camera_fb_get();
    keypoints_shell = (keypoints_shell_t){
        .pixels_cloud = {
            .coords = vector_create(sizeof(pixel_coord_t)),
            .center_coord = (pixel_coord_t){0, 0},
            .claster_centers_coords = vector_create(sizeof(pixel_coord_t))
        }
    };

    if (xTaskCreatePinnedToCore(
                    &photographer_task,
                    "photographer_task",
                    12288, // 16384,
                    NULL,
                    7,
                    NULL,
                    0) != pdPASS
    ) {
        ESP_LOGE(TAG, "run_photographer \t\t failed to create photographer_task \t abort ");
        abort();
        return ESP_FAIL;
    }

    vTaskDelay(pdMS_TO_TICKS(500));

    return ESP_OK;
}