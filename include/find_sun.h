#ifndef FIND_SUN_H
#define FIND_SUN_H


#include "defs.h"
#include "camera.h"

/**
 * @brief holds small coords uint8_t
 */
typedef struct {
    uint8_t x;
    uint8_t y;
} pixel_coordinate_t;

/**
 * @brief allocs and needs to free .coords
 */
typedef struct {
    pixel_coordinate_t *coords;

    uint16_t count;
    uint16_t capacity;
    pixel_coordinate_t center_coord;
} max_brightness_pixels_t;

/**
 * @brief makes as called
 * 
 * @param frame takes camera_fb_t*
 * @return max_brightness_pixels_t* which then needs to be freed
 */
max_brightness_pixels_t *mark_sun(camera_fb_t *frame);

/**
 * @brief calculates X Y degrees from point to center of frame
 * 
 * @param sun_coord input point
 * @param fovs2write output int8_t array must be size 2
 * @return errors
 */
esp_err_t get_FOVs(
    const pixel_coordinate_t *sun_coord,
    float *FOVs /* with size 2 */
);


#endif