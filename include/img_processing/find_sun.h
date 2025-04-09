#ifndef FOLLOW_OBJ_IN_IMG_H
#define FOLLOW_OBJ_IN_IMG_H

#include "img_processing/camera.h"


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
max_brightness_pixels_t *mark_sun(const camera_fb_t *frame);


#endif