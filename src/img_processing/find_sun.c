#include <math.h>

#include "img_processing/find_sun.h"

#ifndef TAG
#define TAG "my_find_sun"
#endif

#define STD_BRIGHTEST_PIXELS_COUNT 16


#define ROUNDF2U8(X) ( (X >= 0.0f) ? (uint8_t)(X + 0.5f) : 0 )


static esp_err_t calc_brightest_center(
    max_brightness_pixels_t *mbp
) {
    float avg_x = 0, avg_y = 0;
    for (int i = 0; i < mbp->count; ++i) {
        avg_x += ROUNDF2U8((float)mbp->coords[i].x / (float)mbp->count);
        avg_y += ROUNDF2U8((float)mbp->coords[i].y / (float)mbp->count);
    }
    mbp->center_coord = (pixel_coordinate_t) {avg_x, avg_y};

    return ESP_OK;
}


static esp_err_t ballance_max_pixels_count(
    max_brightness_pixels_t *mbp
) {
    esp_err_t err = ESP_FAIL;
    uint16_t pixels_divider = 1;
    if (mbp->count > STD_BRIGHTEST_PIXELS_COUNT) {
        pixels_divider = mbp->count / STD_BRIGHTEST_PIXELS_COUNT + 1;
        uint16_t new_count = 0;
        for (uint16_t i = 0; i < mbp->count; i += pixels_divider) {
            mbp->coords[new_count] = mbp->coords[i];
            ++ new_count;
        }
        mbp->count = new_count;
        pixel_coordinate_t *new_coords = (pixel_coordinate_t *)realloc(mbp->coords, mbp->count * sizeof(pixel_coordinate_t)
        );
        if (new_coords) {
            mbp->coords = new_coords;
            err = ESP_OK;
        } else {
            ESP_LOGE(TAG, "ballance_max_pixels_count realloc failed");
            err = ESP_FAIL;
        }
    } else if (mbp->count < STD_BRIGHTEST_PIXELS_COUNT) {
        pixels_divider = 1;
    }

    return err;
}


static max_brightness_pixels_t *find_max_brightness_pixels(
    const camera_fb_t *frame
) {
    uint16_t *pixel_data = (uint16_t *)frame->buf;
    uint8_t max_brightness = 0;
    
    max_brightness_pixels_t *mbp = (max_brightness_pixels_t *)malloc(sizeof(max_brightness_pixels_t));
    uint16_t capacity = 0;
    *mbp = (max_brightness_pixels_t){
        .coords = NULL,
        .count = 0,
        .center_coord = {0, 0}
    };

    // For logging
    uint8_t max_r = 0, max_g = 0, max_b = 0;
    uint16_t orig_mbp_count = 0;

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

                if (mbp->coords) {
                    free(mbp->coords);
                    mbp->coords = NULL;
                }
                mbp->count = 0;
                capacity = 0;
            }

            if (brightness == max_brightness) {
                max_r = r; max_g = g; max_b = b;
                if (mbp->count >= capacity) {
                    capacity += STD_BRIGHTEST_PIXELS_COUNT;
                    pixel_coordinate_t *new_coords = (pixel_coordinate_t *)realloc(mbp->coords, capacity * sizeof(pixel_coordinate_t));
                    if (!new_coords) {
                        ESP_LOGE(TAG, "find_max_brightness_pixels realloc failed");
                        if (mbp->coords) {
                            free(mbp->coords);
                            mbp->coords = NULL;
                        }
                        mbp->count = 0;
                        return mbp;
                    }
                    mbp->coords = new_coords;
                }
                mbp->coords[mbp->count++] = (pixel_coordinate_t){x, y};
            }
        }
    }
    orig_mbp_count = mbp->count;

    ballance_max_pixels_count(mbp);
    
    ESP_LOGI(TAG, "Found %zu / %zu max pixels with max r: %hu g: %hu b: %hu", 
                mbp->count, orig_mbp_count, max_r, max_g, max_b);

    calc_brightest_center(mbp);

    return mbp;
}


max_brightness_pixels_t *mark_sun(
    const camera_fb_t *frame
) {
    max_brightness_pixels_t *mbp = find_max_brightness_pixels(frame);
    float FOVs[2] = {0, 0};
    get_FOVs(&mbp->center_coord, FOVs);
    return mbp;
}