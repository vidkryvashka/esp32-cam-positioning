#ifndef CAMERA_H
#define CAMERA_H

#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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
#define PIXELS_FOCUS 109    // calculated with camera FOW 56 width & height degrees for аurther calculations

esp_err_t init_camera(void);

#endif

#endif