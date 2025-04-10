#ifndef CAMERA_H
#define CAMERA_H

#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// support IDF 5.x
#ifndef portTICK_RATE_MS
#define portTICK_RATE_MS portTICK_PERIOD_MS
#endif

#include "esp_camera.h"
#include "img_converters.h"

#include "defs.h"

#define BOARD_ESP32CAM_AITHINKER

// static const char *TAG = "example:take_picture";

#if ESP_CAMERA_SUPPORTED

#define FRAME_WIDTH_AND_HEIGHT 96   // in camera.c static camera_config_t camera_config .frame_size = FRAMESIZE_96X96

esp_err_t init_camera(void);


extern volatile SemaphoreHandle_t frame_mutex;
extern camera_fb_t *current_frame;  // initiated in photographer.c, used in take photo and then in webserver.c

#define PHOTOGRAPHER_DELAY_MS 1500


/**
 * @brief holds small coords uint8_t
 */
typedef struct {
    uint8_t x;
    uint8_t y;
} pixel_coordinate_t;


/**
 * @brief calculates X Y degrees from point to center of frame
 * 
 * @param coord input point
 * @param fovs2write output int8_t array must be size 2
 * @return errors
 */
esp_err_t get_FOVs(
    const pixel_coordinate_t *sun_coord,
    float *FOVs /* with size 2 */
);


#endif

#endif