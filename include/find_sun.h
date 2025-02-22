#ifndef FIND_SUN
#define FIND_SUN


#include "defs.h"
#include "camera.h"

typedef struct {
    size_t x;
    size_t y;
} pixel_coordinate_t;

/**
 * @brief allocs and needs to free .coords
 */
typedef struct {
    pixel_coordinate_t *coords;
    size_t count;
    size_t capacity;
    pixel_coordinate_t center_coord;
} max_brightness_pixels_t;

/**
 * @brief makes as called
 * 
 * @param frame takes camera_fb_t*
 * @return max_brightness_pixels_t* which then need to free
 */
max_brightness_pixels_t *mark_sun(camera_fb_t *frame);

#endif