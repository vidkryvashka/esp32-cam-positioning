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
#include "my_vector.h"

#define BOARD_ESP32CAM_AITHINKER

#if ESP_CAMERA_SUPPORTED

#define PIXFORMAT_CHOISE PIXFORMAT_GRAYSCALE

extern uint16_t frame_width;
extern uint16_t frame_height;

// #define FRAME_WIDTH_AND_HEIGHT 96   // in camera.c static camera_config_t camera_config .frame_size = FRAMESIZE_96X96

esp_err_t init_camera(void);


extern volatile SemaphoreHandle_t frame_mutex;
extern camera_fb_t *current_frame;  // initiated in photographer.c, used in take photo and then in webserver.c


/**
 * @brief holds x y coords
 */
typedef struct {
    uint16_t x;
    uint16_t y;
} pixel_coord_t;

/**
 * @brief pixel cloud with center
 */
typedef struct {
    vector_t *coords;
    pixel_coord_t center_coord;
    vector_t *claster_centers_coords;
} pixels_cloud_t;


/**
 * @brief what's unclear ?
 * 
 */
typedef struct {
    pixel_coord_t top_left;
    uint16_t width;
    uint16_t height;
} rectangle_coords_t;

#define ANGLE_THRESHOLD 5   // degrees ignore

/**
 * @brief calculates X Y degrees from point to center of frame
 * 
 * @param coord input point
 * @param angles to write - output float array must be size 2
 * @return errors
 */
esp_err_t calculate_angles_diff(
    const pixel_coord_t *sun_coord,
    angles_diff_t *angles_diff
);


/**
 * @brief mallocs and initiates camera_fb_t frame by parameters
 * 
 * @param width 
 * @param height 
 * @param format PIXFORMAT_(GRAYSCALE | RGB565)
 * @return camera_fb_t* 
 */
camera_fb_t* camera_fb_create(
    const uint16_t width,
    const uint16_t height,
    const pixformat_t format
);


/**
 * @brief allocs and initiates camera_fb_t frame by src fields
 * 
 * @param src camera_fb_t source
 * 
 * @return camera_bf_t* alloced copy
 */
camera_fb_t* camera_fb_copy(
    const camera_fb_t* src
);


/**
 * @brief destructs self created camera_fb_t, don't use to esp_camera_fb_get() created fbs, they should be returned
 * 
 * @param fb to free
 */
void camera_fb_free(
    camera_fb_t* fb
);


/**
 * @brief allocs and cropps by rectangle coords and form from src camera_fb_t*
 * 
 * @param src image to crop
 * @param rect coords and form
 * @return camera_fb_t * alloced cropped frame
 */
camera_fb_t* camera_fb_crop(
    const camera_fb_t *src,
    const rectangle_coords_t *rect
);


#endif
#endif







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