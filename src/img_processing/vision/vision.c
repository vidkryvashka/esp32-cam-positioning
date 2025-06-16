#include <math.h>

#include "img_processing/vision_defs.h"


#define TAG "my_vision"


#define LOCAL_LOG_LEVEL     0

#define FAST9_BALLANCE_ON   1

#define FAST9_START_THRESHOLD       90
#if FAST9_BALLANCE_ON
    // #define FAST9_MIN_THRESHOLD      50
    // #define FAST9_MAX_THRESHOLD      130
    #define KEYPOINTS_MAX_COUNT         64
    #define BALLANCING_MAX_ITERATIONS   2
    #define BALLANCE_COEF               1.3
#endif

#define DBSCAN_EPSILON          25      // min 2D distance between points to attribute the point to the cluster
#define DBSCAN_MIN_POINTS_NUM   3       // min points number in cluster



static uint8_t fast9_threshold = FAST9_START_THRESHOLD;


static esp_err_t balance_fast9(
    const camera_fb_t *fb1, // gray single channel frame buffer
    vector_t *keypoints
) {
    #if FAST9_BALLANCE_ON
        for (uint8_t i = 0; i < BALLANCING_MAX_ITERATIONS; i++) {
            fast9(fb1, keypoints, fast9_threshold);

            if (keypoints->size > KEYPOINTS_MAX_COUNT)
                fast9_threshold *= BALLANCE_COEF;
            else if (fast9_threshold > FAST9_START_THRESHOLD && keypoints->size < KEYPOINTS_MAX_COUNT / 3) {
                fast9_threshold /= BALLANCE_COEF;
                if (fast9_threshold < FAST9_START_THRESHOLD)
                    fast9_threshold = FAST9_START_THRESHOLD;
                break;
            }
        }
    #elif
        fast9(fb1, keypoints, fast9_threshold);
    #endif

    return ESP_OK;
}


esp_err_t find_drone(
    const camera_fb_t *frame,
    pixels_cloud_t *pixels_cloud
) {
    if (frame->format != PIXFORMAT_GRAYSCALE) {
        ESP_LOGE(TAG, "expected PIXFORMAT_GRAYSCALE");
    }

    balance_fast9(frame, pixels_cloud->coords);
    #if LOCAL_LOG_LEVEL
        ESP_LOGI(TAG, "image (%d x %d) corners: %d, threshold %d ",
                frame->width, frame->height, pixels_cloud->coords->size, fast9_threshold);
    #endif

    if (pixels_cloud->coords->size == 0) {
        ESP_LOGW(TAG, "find_drone \t no keypoints found ");
        pixels_cloud->center_coord = (pixel_coord_t){0, 0};
        return ESP_OK;
    }


    vector_t *cluster_centers = vector_create(sizeof(pixel_coord_t));
    esp_err_t dbscan_err = dbscan_cluster(pixels_cloud, DBSCAN_EPSILON, DBSCAN_MIN_POINTS_NUM, cluster_centers);
    if (dbscan_err != ESP_OK) {
        ESP_LOGE(TAG, "DBSCAN clustering failed");
        return ESP_FAIL;
    }

    #if LOCAL_LOG_LEVEL
        ESP_LOGI(TAG, "dbscan clasters centers ");
        vector_print(cluster_centers);

        ESP_LOGI(TAG, "Drone center: (%d, %d)", 
                pixels_cloud->center_coord.x, pixels_cloud->center_coord.y);
    #endif

    return ESP_OK;
}