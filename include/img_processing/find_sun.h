#ifndef FIND_SUN_H
#define FIND_SUN_H

#include "my_vector.h"
#include "img_processing/camera.h"


/**
 * @brief allocs and needs to free .coords
 */
// typedef struct {
//     pixel_coord_t *coords;
//     uint16_t count;
//     uint16_t capacity;
//     
//     vector_t *coords_vec;
//     pixel_coord_t center_coord;
// } max_brightness_pixels_t;

typedef struct {
    vector_t *coords;
    pixel_coord_t center_coord;
} max_brightness_pixels_t;


/**
 * @brief makes as called
 * 
 * @param frame takes camera_fb_t*
 * @return max_brightness_pixels_t* which then needs to be freed
 */
max_brightness_pixels_t* mark_sun(const camera_fb_t *frame);

esp_err_t free_mbp(max_brightness_pixels_t *mbp);


#endif