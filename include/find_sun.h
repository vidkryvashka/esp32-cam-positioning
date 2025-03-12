#ifndef FIND_SUN
#define FIND_SUN


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
 * @return max_brightness_pixels_t* which then need to free
 */
max_brightness_pixels_t *mark_sun(camera_fb_t *frame);

esp_err_t get_FOVs(pixel_coordinate_t *sun_coord, int8_t *fovs2write);


#endif