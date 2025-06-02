#include <math.h>

#include "img_processing/ORB_defs.h"

#ifndef TAG
    #define TAG "my_vision"
#endif

static uint8_t fast9_threshold = START_THRESHOLD;
#define BALLANCE_COEF   1.3


// static void paint(
//     uint8_t *im1,
//     uint8_t *im3,
//     vector_t *c,
//     uint16_t w,
//     uint16_t h
// ) {
//     for (size_t i = 0; i < w * h; ++i)
//         im1[i] = im1[i] * SAVIMG_IMAGE_DIM_COEF;
// 
//     for (size_t i = 0; i < c->size; ++i) {
//         pixel_coord_t *p = (pixel_coord_t *)vector_get(c, i);
//         if (p->x < w && p->y < h)
//             im1[w * p->y + p->x] = 255;
//     }
// }


// esp_err_t orb(
//     uint8_t *img3_target,
//     uint8_t *img3_seek,
//     uint8_t *img1_target,
//     uint8_t *img1_seek,
//     uint16_t wt,
//     uint16_t ht,
//     uint16_t ws,
//     uint16_t hs
// );


static esp_err_t balance_fast9(
    const camera_fb_t *fb1, // gray single channel
    vector_t *keypoints
) {
    while (1) {
        fast9(fb1, keypoints, fast9_threshold);
        if (keypoints->size > KEYPOINTS_MAX_COUNT)
            fast9_threshold *= BALLANCE_COEF;
        else {
            fast9_threshold /= BALLANCE_COEF;
            break;
        }
    }
    
    return ESP_OK;
}


int8_t find_fragment(
    camera_fb_t *frame,
    camera_fb_t *fragment,
    rectangle_coords_t *rect,
    vector_t *keypoints
) {

    if (frame->format != PIXFORMAT_GRAYSCALE) {
        ESP_LOGE(TAG, "expected PIXFORMAT_GRAYSCALE");
    }

    // uint8_t *img3_target = fragment->buf; // (uint8_t *)malloc(10000); // = loadBMP(&wt, &ht, argc > 1 ? argv[1] : DEFAULT_TARGET_IMG_PATH);
    // uint8_t *img3_seek = frame->buf; // loadBMP(&ws, &hs, DEFAULT_SEEK_IMG_PATH);
    // uint8_t *img1_seek = (uint8_t *)malloc(ws * hs * sizeof(uint8_t));
    
    
    // orb(img3_target, img3_seek, img1_target, img1_seek, wt, ht, ws, hs);

    balance_fast9(frame, keypoints);
    ESP_LOGI(TAG, "image (%d x %d) corners: %d, threshold %d\n", frame->width, frame->height, keypoints->size, fast9_threshold);
    // paint(img1_target, img3_target, c, wt, ht);

    return 0;
}