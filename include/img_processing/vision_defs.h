#ifndef VISION_DEFS_H
#define VISION_DEFS_H

#include "defs.h"
#include "my_vector.h"
#include "img_processing/camera.h"



/**
 * @brief fast9 decision tree keypoints algorithm
 * 
 * @param fb1       gray single channel camera_fb_t
 * @param keypoints global messy variable to send it to web page
 * @return          vector_t* pixel_coord_t keypoints
 */
esp_err_t fast9(
    const camera_fb_t *fb1, // gray single channel
    vector_t *keypoint_coords,
    uint8_t threshold
);


/**
 * @brief Performs DBSCAN clustering and returns cluster centers
 * @param pixels_cloud Input data (point coordinates)
 * @param epsilon Maximum distance between points in the same cluster
 * @param min_points Minimum number of points to form a cluster
 * @param result Clustering result (includes cluster centers)
 * @return ESP_OK on success, otherwise ESP_FAIL
 */
esp_err_t dbscan_cluster(
    pixels_cloud_t *pixels_cloud,
    const uint8_t epsilon,
    uint16_t min_points,
    vector_t *cluster_centers
);


/**
 * @brief writes nfo about drone consisting analysis into pixels_cloud
 * 
 * @param frame         to analyze
 * @param pixels_cloud  global messy variable, may be sent to web page
 * @return              esp_err_t
 */
esp_err_t find_drone(
    const camera_fb_t *frame,
    pixels_cloud_t *pixels_cloud
);

#endif







/**
 * @brief makes as called
 * 
 * @param *mbp pixels_cloud_t destination to write
 * @param frame camera_fb_t*
 * @return pixels_cloud_t* which then needs to be freed
 */
esp_err_t find_sun(
    pixels_cloud_t *pixels_cloud,
    const camera_fb_t *frame
);