#ifndef ORB_DEFS_H
#define ORB_DEFS_H

#include "defs.h"
#include "my_vector.h"
#include "img_processing/camera.h"


#define SAVIMG_IMAGE_DIM_COEF 0.4



/**
 * @brief fast9 decision tree keypoints algorithm
 * 
 * @param fb1       gray single channel camera_fb_t
 * @param keypoints global messy variable to send it to web page
 * @return          vector_t* pixel_coord_t keypoints
 */
esp_err_t fast9(
    const camera_fb_t *fb1, // gray single channel
    vector_t *keypoints,
    uint8_t threshold
);


/**
 * @brief 
 * 
 * @param frame     big one
 * @param fragment  consider smaller image
//  * @param top_left  forgot why
 * @param pixels_cloud global messy variable to send it to web page
 * @return          similarity: [0, 100] % posibility measurement fragment is in frame, < 0 if some errors, not implemented
 */
esp_err_t find_drone(
    camera_fb_t *frame,
    camera_fb_t *fragment,
    pixels_cloud_t *pixels_cloud
);

#endif