#ifndef IMG_PROCESSING_DEFS_H
#define IMG_PROCESSING_DEFS_H

// #include "defs.h"    // calls self
#include "img_processing/camera.h"


#define PHOTOGRAPHER_DELAY_MS 1000

extern int8_t current_frame_avialable;
extern camera_fb_t *current_frame;

/**
 * @brief holds small coords uint8_t
 */
typedef struct {
    uint8_t x;
    uint8_t y;
} pixel_coordinate_t;


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