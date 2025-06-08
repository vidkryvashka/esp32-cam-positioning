#ifndef FIND_SUN_H
#define FIND_SUN_H

#include "my_vector.h"
#include "img_processing/camera.h"


// /**
//  * @brief pixel cloud with center
//  */
// typedef struct {
//     vector_t *coords;
//     pixel_coord_t center_coord;
// } pixels_cloud_t;


/**
 * @brief makes as called
 * 
 * @param *mbp pixels_cloud_t destination to write
 * @param frame camera_fb_t*
 * @return pixels_cloud_t* which then needs to be freed
 */
esp_err_t mark_sun(
    pixels_cloud_t *pixels_cloud,
    const camera_fb_t *frame,
    angles_diff_t *angles_diff
);


#endif