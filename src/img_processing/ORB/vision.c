#include <math.h>

#include "img_processing/vision_defs.h"

#define TAG "my_vision"

static uint8_t fast9_threshold = START_THRESHOLD;
#define BALLANCE_COEF   1.35


static esp_err_t balance_fast9(
    const camera_fb_t *fb1, // gray single channel
    vector_t *keypoints
) {
    while (1) {
        fast9(fb1, keypoints, fast9_threshold);
        if (keypoints->size > KEYPOINTS_MAX_COUNT)
            fast9_threshold *= BALLANCE_COEF;
        else if (keypoints->size < KEYPOINTS_MAX_COUNT) {
            fast9_threshold /= BALLANCE_COEF;
            break;
        }
    }
    
    return ESP_OK;
}


int8_t find_drone(
    camera_fb_t *frame,
    camera_fb_t *fragment,
    // rectangle_coords_t *rect,
    pixels_cloud_t *pixels_cloud
) {

    if (frame->format != PIXFORMAT_GRAYSCALE) {
        ESP_LOGE(TAG, "expected PIXFORMAT_GRAYSCALE");
    }

    balance_fast9(frame, pixels_cloud->coords);
    ESP_LOGI(TAG, "image (%d x %d) corners: %d, threshold %d\n", frame->width, frame->height, pixels_cloud->coords->size, fast9_threshold);

    return 0;
}