#include <math.h>

#include "img_processing/ORB_defs.h"


#define TAG "my_vision"


// Transition between 1 and 3 channel Pictures
// static void im1to3(
//     uint8_t *im1,
//     uint8_t *im3,
//     size_t size1
// ) {
//     for (size_t i1 = 0, i3 = 0; i1 < size1; ++i1) {
//         im3[i3] = im1[i1];
//         im3[i3 + 1] = im1[i1];
//         im3[i3 + 2] = im1[i1];
//         i3 += 3;
//     }
// }


static void im3to1(
    const uint8_t *im3,
    uint8_t *im1,
    const size_t size1
) {
    for (size_t i1 = 0, i3 = 0; i1 < size1; ++i1) {
        im1[i1] = (im3[i3] + im3[i3 + 1] + im3[i3 + 2]) / 3;
        i3 += 3;
    }
}


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


int8_t find_fragment(
    camera_fb_t *frame,
    camera_fb_t *fragment,
    rectangle_coords_t *rect,
    vector_t *keypoints
) {
    // uint16_t wt = fragment->width;    // target
    // uint16_t ht = fragment->height;
    uint16_t ws = frame->width;    // seek
    uint16_t hs = frame->height;

    // uint8_t *img3_target = fragment->buf; // (uint8_t *)malloc(10000); // = loadBMP(&wt, &ht, argc > 1 ? argv[1] : DEFAULT_TARGET_IMG_PATH);
    // uint8_t *img1_target = (uint8_t *)malloc(wt * ht * sizeof(uint8_t));
    uint8_t *img3_seek = frame->buf; // loadBMP(&ws, &hs, DEFAULT_SEEK_IMG_PATH);
    uint8_t *img1_seek = (uint8_t *)malloc(ws * hs * sizeof(uint8_t));
    
    // into gray
    // im3to1(img3_target, img1_target, wt * ht);
    im3to1(img3_seek, img1_seek, ws * hs);
    
    // orb(img3_target, img3_seek, img1_target, img1_seek, wt, ht, ws, hs);

    // apply detector
    fast9(img1_seek, ws, hs, keypoints);
    ESP_LOGI(TAG, "image (%d x %d) corners: %d\n", ws, hs, keypoints->size);
    // vector_print(*keypoints);
    // paint(img1_target, img3_target, c, wt, ht);
    // vector_destroy(c);

    // im1to3(img1_target, img3_target, wt * ht);

    // saveBMP(DEFAULT_OUT_IMG_PATH, img3_target, wt, ht);

    // free(img3_target);
    // free(img1_target);
    // free(img3_seek);
    free(img1_seek);

    return 0;
}