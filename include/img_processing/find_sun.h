#ifndef FIND_SUN_H
#define FIND_SUN_H

#include "my_vector.h"
#include "img_processing/camera.h"


/**
 * @brief allocs and needs to free .coords
 */
typedef struct {
    vector_t *coords;
    pixel_coord_t center_coord;
} max_brightness_pixels_t;


/**
 * @brief makes as called
 * 
 * @param *mbp max_brightness_pixels_t destination to write
 * @param frame camera_fb_t*
 * @return max_brightness_pixels_t* which then needs to be freed
 */
esp_err_t mark_sun(
    max_brightness_pixels_t *mbp,
    const camera_fb_t *frame
);


#endif