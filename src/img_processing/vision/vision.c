#include <math.h>

#include "img_processing/vision_defs.h"
#include "img_processing/dbscan.h"


#define TAG "my_vision"


#define START_THRESHOLD     90
// #define MIN_THRESHOLD       50
// #define MAX_THRESHOLD       130
#define KEYPOINTS_MAX_COUNT    64
#define MAX_ITERATIONS      2

#define BALLANCE_COEF   1.3

static uint8_t fast9_threshold = START_THRESHOLD;


static esp_err_t balance_fast9(
    const camera_fb_t *fb1, // gray single channel frame buffer
    vector_t *keypoints
) {
    for (uint8_t i = 0; i < MAX_ITERATIONS; i++) {
        fast9(fb1, keypoints, fast9_threshold);

        if (keypoints->size > KEYPOINTS_MAX_COUNT)
            fast9_threshold *= BALLANCE_COEF;
        else if (fast9_threshold > START_THRESHOLD && keypoints->size < KEYPOINTS_MAX_COUNT / 3) {
            // fast9_threshold /= BALLANCE_COEF;
            fast9_threshold = START_THRESHOLD;
            break;
        }
    }

    return ESP_OK;
}


esp_err_t find_drone(
    camera_fb_t *frame,
    pixels_cloud_t *pixels_cloud
) {

    if (frame->format != PIXFORMAT_GRAYSCALE) {
        ESP_LOGE(TAG, "expected PIXFORMAT_GRAYSCALE");
    }

    balance_fast9(frame, pixels_cloud->coords);
    ESP_LOGI(TAG, "image (%d x %d) corners: %d, threshold %d ",
            frame->width, frame->height, pixels_cloud->coords->size, fast9_threshold);


    if (pixels_cloud->coords->size == 0) {
        ESP_LOGW(TAG, "find_drone \t no keypoints found ");
        pixels_cloud->center_coord = (pixel_coord_t){0, 0};
        return ESP_OK;
    }



    double epsilon = 20.0;
    unsigned int min_points = 3;


    vector_t *cluster_centers = vector_create(sizeof(pixel_coord_t));
    esp_err_t dbscan_err = dbscan_cluster(pixels_cloud, epsilon, min_points, cluster_centers);
    if (dbscan_err != ESP_OK) {
        ESP_LOGE(TAG, "DBSCAN clustering failed");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "dbscan clasters centers ");
    vector_print(cluster_centers);

    ESP_LOGI(TAG, "Drone center: (%d, %d)", 
             pixels_cloud->center_coord.x, pixels_cloud->center_coord.y);

    return ESP_OK;
}