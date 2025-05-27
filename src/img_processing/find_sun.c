#include <math.h>

#include "img_processing/find_sun.h"
#include "my_vector.h"

#ifndef TAG
#define TAG "my_find_sun"
#endif


#define STD_BRIGHTEST_PIXELS_COUNT 16
#define ROUNDF2U8(X) ((X >= 0.0f) ? (uint16_t)(X + 0.5f) : 0)

static esp_err_t calc_brightest_center(
    max_brightness_pixels_t *mbp
) {
    float avg_x = 0, avg_y = 0;
    for (size_t i = 0; i < mbp->coords->size; ++i) {
        pixel_coord_t *coord = (pixel_coord_t *)vector_get(mbp->coords, i);
        avg_x += ROUNDF2U8((float)coord->x / (float)mbp->coords->size);
        avg_y += ROUNDF2U8((float)coord->y / (float)mbp->coords->size);
    }
    mbp->center_coord = (pixel_coord_t){avg_x, avg_y};
    return ESP_OK;
}


static esp_err_t balance_max_pixels_count(
    max_brightness_pixels_t *mbp
) {
    if (mbp->coords->size <= STD_BRIGHTEST_PIXELS_COUNT) {
        return ESP_OK;
    }

    uint16_t pixels_divider = mbp->coords->size / STD_BRIGHTEST_PIXELS_COUNT + 1;
    vector_t *new_coords = vector_create(sizeof(pixel_coord_t));
    if (!new_coords) {
        ESP_LOGE(TAG, "balance_max_pixels_count vector_create failed");
        return ESP_FAIL;
    }

    for (size_t i = 0; i < mbp->coords->size; i += pixels_divider) {
        pixel_coord_t *coord = (pixel_coord_t *)vector_get(mbp->coords, i);
        if (vector_push_back(new_coords, coord) != ESP_OK) {
            vector_destroy(new_coords);
            return ESP_FAIL;
        }
    }

    vector_destroy(mbp->coords);
    mbp->coords = new_coords;
    return ESP_OK;
}


static max_brightness_pixels_t* find_max_brightness_pixels(
    const camera_fb_t *frame
) {
    uint16_t *pixel_data = (uint16_t *)frame->buf;
    uint8_t max_brightness = 0;
    
    max_brightness_pixels_t *mbp = (max_brightness_pixels_t *)malloc(sizeof(max_brightness_pixels_t));
    if (!mbp) {
        ESP_LOGE(TAG, "find_max_brightness_pixels malloc failed");
        return NULL;
    }
    
    mbp->coords = vector_create(sizeof(pixel_coord_t));
    if (!mbp->coords) {
        free(mbp);
        ESP_LOGE(TAG, "find_max_brightness_pixels vector_create failed");
        return NULL;
    }
    
    mbp->center_coord = (pixel_coord_t){0, 0};
    
    uint8_t max_r = 0, max_g = 0, max_b = 0;
    size_t orig_mbp_count = 0;

    for (uint8_t y = 0; y < frame->height; ++y) {
        for (uint8_t x = 0; x < frame->width; ++x) {
            uint16_t index = y * frame->width + x;
            uint16_t pixel = pixel_data[index];
            uint8_t r = (pixel >> 11) & 0x1F;
            uint8_t g = (pixel >> 5) & 0x3F;
            uint8_t b = pixel & 0x1F;
            uint8_t brightness = (r + g + b);

            if (brightness > max_brightness) {
                max_brightness = brightness;
                vector_clear(mbp->coords);
                max_r = r; max_g = g; max_b = b;
            }

            if (brightness == max_brightness) {
                pixel_coord_t coord = {x, y};
                if (vector_push_back(mbp->coords, &coord) != ESP_OK) {
                    ESP_LOGE(TAG, "find_max_brightness_pixels vector_push_back failed");
                    vector_destroy(mbp->coords);
                    free(mbp);
                    return NULL;
                }
            }
        }
    }
    orig_mbp_count = mbp->coords->size;

    esp_err_t err = balance_max_pixels_count(mbp);
    if (err != ESP_OK) {
        vector_destroy(mbp->coords);
        free(mbp);
        return NULL;
    }

    ESP_LOGI(TAG, "Found %zu / %zu max pixels with max r: %hu g: %hu b: %hu", 
            mbp->coords->size, orig_mbp_count, max_r, max_g, max_b);

    calc_brightest_center(mbp);
    return mbp;
}


max_brightness_pixels_t* mark_sun(
    const camera_fb_t *frame
) {
    max_brightness_pixels_t *mbp = find_max_brightness_pixels(frame);
    if (!mbp) {
        return NULL;
    }

    float FOVs[2] = {0, 0};
    get_FOVs(&mbp->center_coord, FOVs);
    
    return mbp;
}


esp_err_t free_mbp(
    max_brightness_pixels_t *mbp
) {
    if (!mbp) {
        ESP_LOGE(TAG, "free_mbp: mbp is NULL");
        return ESP_FAIL;
    }
    if (mbp->coords) {
        vector_destroy(mbp->coords);
    }
    free(mbp);
    return ESP_OK;
}








// #include <math.h>

#include "img_processing/find_sun.h"
// 
// #ifndef TAG
// #define TAG "my_find_sun"
// #endif
// 
// #define STD_BRIGHTEST_PIXELS_COUNT 16
// 
// 
// #define ROUNDF2U8(X) ( (X >= 0.0f) ? (uint16_t)(X + 0.5f) : 0 )
// 
// 
// static esp_err_t calc_brightest_center(
//     max_brightness_pixels_t *mbp_old
// ) {
//     float avg_x = 0, avg_y = 0;
//     for (int i = 0; i < mbp_old->count; ++i) {
//         avg_x += ROUNDF2U8((float)mbp_old->coords[i].x / (float)mbp_old->count);
//         avg_y += ROUNDF2U8((float)mbp_old->coords[i].y / (float)mbp_old->count);
//     }
//     mbp_old->center_coord = (pixel_coord_t) {avg_x, avg_y};
// 
//     return ESP_OK;
// }
// 
// 
// static esp_err_t ballance_max_pixels_count(
//     max_brightness_pixels_t *mbp_old
// ) {
//     esp_err_t err = ESP_FAIL;
//     uint16_t pixels_divider = 1;
//     if (mbp_old->count > STD_BRIGHTEST_PIXELS_COUNT) {
//         pixels_divider = mbp_old->count / STD_BRIGHTEST_PIXELS_COUNT + 1;
//         uint16_t new_count = 0;
//         for (uint16_t i = 0; i < mbp_old->count; i += pixels_divider) {
//             mbp_old->coords[new_count] = mbp_old->coords[i];
//             ++ new_count;
//         }
//         mbp_old->count = new_count;
//         pixel_coord_t *new_coords = (pixel_coord_t *)realloc(mbp_old->coords, mbp_old->count * sizeof(pixel_coord_t)
//         );
//         if (new_coords) {
//             mbp_old->coords = new_coords;
//             err = ESP_OK;
//         } else {
//             ESP_LOGE(TAG, "ballance_max_pixels_count realloc failed");
//             err = ESP_FAIL;
//         }
//     } else if (mbp_old->count < STD_BRIGHTEST_PIXELS_COUNT) {
//         pixels_divider = 1;
//     }
// 
//     return err;
// }
// 
// 
// static max_brightness_pixels_t *find_max_brightness_pixels(
//     const camera_fb_t *frame
// ) {
//     // vector_t *
// 
//     uint16_t *pixel_data = (uint16_t *)frame->buf;
//     uint8_t max_brightness = 0;
//     
//     max_brightness_pixels_t *mbp_old = (max_brightness_pixels_t *)malloc(sizeof(max_brightness_pixels_t));
//     uint16_t capacity = 0;
//     *mbp_old = (max_brightness_pixels_t){
//         .coords = NULL,
//         .count = 0,
//         .center_coord = {0, 0}
//     };
// 
//     // For logging
//     uint8_t max_r = 0, max_g = 0, max_b = 0;
//     uint16_t orig_mbp_count = 0;
// 
//     for (uint8_t y = 0; y < frame->height; ++y) {
//         for (uint8_t x = 0; x < frame->width; ++x) {
//             uint16_t index = y * frame->width + x;
//             uint16_t pixel = pixel_data[index];
//             uint8_t r = (pixel >> 11) & 0x1F;
//             uint8_t g = (pixel >> 5) & 0x3F;
//             uint8_t b = pixel & 0x1F;
//             uint8_t brightness = (r + g + b);
// 
//             if (brightness > max_brightness) {
//                 max_brightness = brightness;
// 
//                 if (mbp_old->coords) {
//                     free(mbp_old->coords);
//                     mbp_old->coords = NULL;
//                 }
//                 mbp_old->count = 0;
//                 capacity = 0;
//             }
// 
//             if (brightness == max_brightness) {
//                 max_r = r; max_g = g; max_b = b;
//                 if (mbp_old->count >= capacity) {
//                     capacity += STD_BRIGHTEST_PIXELS_COUNT;
//                     pixel_coord_t *new_coords = (pixel_coord_t *)realloc(mbp_old->coords, capacity * sizeof(pixel_coord_t));
//                     if (!new_coords) {
//                         ESP_LOGE(TAG, "find_max_brightness_pixels realloc failed");
//                         if (mbp_old->coords) {
//                             free(mbp_old->coords);
//                             mbp_old->coords = NULL;
//                         }
//                         mbp_old->count = 0;
//                         return mbp_old;
//                     }
//                     mbp_old->coords = new_coords;
//                 }
//                 mbp_old->coords[mbp_old->count++] = (pixel_coord_t){x, y};
//             }
//         }
//     }
//     orig_mbp_count = mbp_old->count;
// 
//     ballance_max_pixels_count(mbp_old);
//     
//     ESP_LOGI(TAG, "Found %zu / %zu max pixels with max r: %hu g: %hu b: %hu", 
//                 mbp_old->count, orig_mbp_count, max_r, max_g, max_b);
// 
//     calc_brightest_center(mbp_old);
// 
//     return mbp_old;
// }
// 
// 
// max_brightness_pixels_t* mark_sun(
//     const camera_fb_t *frame
// ) {
//     max_brightness_pixels_t *mbp_old = find_max_brightness_pixels(frame);
//     float FOVs[2] = {0, 0};
//     get_FOVs(&mbp_old->center_coord, FOVs);
//     return mbp_old;
// }
// 
// esp_err_t free_mbp(
//     max_brightness_pixels_t *mbp_old
// ) {
// 
// 
//     return ESP_OK;
// }