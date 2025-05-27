#ifndef ORB_DEFS_H
#define ORB_DEFS_H

#include "defs.h"
#include "my_vector.h"
#include "img_processing/camera.h"

#define THRESHOLD 80
#define BRIEF_SIZE 256
#define PATCH_SIZE 31
#define SIGMA 5
#define MAX_KEYPOINTS 10000
#define SCALE_FACTOR 1.41421356237 // sqrt(2)
#define NLEVELS 8
#define EDGE_THRESHOLD 31
#define SAVIMG_IMAGE_DIM_COEF 0.4


typedef struct {
    uint8_t data[BRIEF_SIZE / NLEVELS]; // 256 bit = 32 byte
} brief_descriptor_t;


typedef struct {
    pixel_coord_t pixel_coord;
    float angle;
    uint8_t level;
    brief_descriptor_t descriptor;
} keypoint_t;



/**
 * @brief fast9 decision tree keypoints algorithm
 * 
 * @param img1  gray single channel image buffer
 * @param w     width
 * @param h     height
 * @return vector_t* pixel_coord_t keypoints
 */
vector_t* fast9(
    const uint8_t *img1,
    const uint16_t w,
    const uint16_t h
);


// void operate(int argc, char **argv);

esp_err_t find_fragment(
    camera_fb_t *frame,
    camera_fb_t *fragment,
    float *similarity,
    pixel_coord_t *top_left
);

#endif