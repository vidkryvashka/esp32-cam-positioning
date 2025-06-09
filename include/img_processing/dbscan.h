#ifndef DBSCAN_H
#define DBSCAN_H

#include "my_vector.h"
#include "img_processing/camera.h"
#include "img_processing/vision_defs.h"

// #define TAG_DBSCAN "dbscan"


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
    double epsilon,
    uint min_points,
    vector_t *cluster_centers
);


#endif