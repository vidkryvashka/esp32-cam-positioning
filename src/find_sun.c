#include <math.h>

#include "find_sun.h"

#define TAG "esp:find_sun"


typedef struct {
    size_t x;
    size_t y;
} pixel_coordinate_t;

/**
 * @brief needs to alloc and free .coords
 */
typedef struct {
    pixel_coordinate_t *coords;
    size_t count;
    pixel_coordinate_t center_coord;
} max_brightness_pixels_t;


#define BRIGHTEST_PIXELS_RADIUS 5
#define CENTER_BTIGHT_PIXEL_RADIUS 10
#define CIRCLE_WIDTH 5

#define STD_BRIGHTEST_PIXELS_COUNT 20
static size_t pixels_divider = 1;


// untested
static esp_err_t calc_brightest_center(max_brightness_pixels_t *mbp) {
    uint8_t avg_x = 0, avg_y = 0;
    for (int i = 0; i < mbp->count; ++i) {
        avg_x += mbp->coords[i].x / mbp->count;
        avg_y += mbp->coords[i].y / mbp->count;
    }
    mbp->center_coord.x = avg_x;
    mbp->center_coord.y = avg_y;

    return ESP_OK;
}

static max_brightness_pixels_t *find_max_brightness_pixels(camera_fb_t *frame, max_brightness_pixels_t *mbp) {
    if (!frame || !frame->buf || !mbp) {
        ESP_LOGE(TAG, "Invalid input parameters");
        return NULL;
    }

    uint16_t *pixel_data = (uint16_t *)frame->buf;
    size_t max_brightness = 0;
    
    *mbp = (max_brightness_pixels_t){
        .coords = NULL,
        .count = 0,
        .center_coord = {0, 0}
    };

    // For logging
    uint8_t max_r = 0, max_g = 0, max_b = 0;

    for (size_t y = 0; y < frame->height; ++y) {
        for (size_t x = 0; x < frame->width; ++x) {
            size_t index = y * frame->width + x;
            uint16_t pixel = pixel_data[index];
            uint8_t r = (pixel >> 11) & 0x1F;
            uint8_t g = (pixel >> 5) & 0x3F;
            uint8_t b = pixel & 0x1F;
            size_t brightness = (r + g + b);

            if (brightness > max_brightness) {
                max_brightness = brightness;
                max_r = r; max_g = g; max_b = b;

                if (mbp->coords) {
                    free(mbp->coords);
                    mbp->coords = NULL;
                }
                mbp->count = 0;
            }

            if (brightness == max_brightness) {
                mbp->count++;
                pixel_coordinate_t *new_coords = (pixel_coordinate_t *)realloc(
                    mbp->coords, 
                    mbp->count * sizeof(pixel_coordinate_t)
                );
                
                if (new_coords == NULL) {
                    ESP_LOGE(TAG, "New max brightest pixel memory allocation failed");
                    if (mbp->coords) {
                        free(mbp->coords);
                        mbp->coords = NULL;
                    }
                    mbp->count = 0;
                    return mbp;
                }
                
                mbp->coords = new_coords;
                mbp->coords[mbp->count - 1].x = x;
                mbp->coords[mbp->count - 1].y = y;
            }
        }
    }

    // brightest pixels count ballancing
    if (mbp->count > STD_BRIGHTEST_PIXELS_COUNT) {
        pixels_divider = mbp->count / (STD_BRIGHTEST_PIXELS_COUNT / 2);
        size_t new_count = 0;
        for (size_t i = 0; i < mbp->count; i += pixels_divider) {
            mbp->coords[new_count] = mbp->coords[i];
            ++ new_count;
        }
        mbp->count = new_count;
        //                                                  !! suspicious !!
        mbp->coords = (pixel_coordinate_t *)realloc(
            mbp->coords,
            mbp->count * sizeof(pixel_coordinate_t)
        );
    } else if (mbp->count < 10) {
        pixels_divider = 1;
    }
    
    ESP_LOGI(TAG, "Found %zu pixels with max r: %hu g: %hu b: %hu, brightness: %zu, divider: %zu", 
                mbp->count, max_r, max_g, max_b, max_brightness, pixels_divider);

    calc_brightest_center(mbp);

    return mbp;
}


esp_err_t mark_sun(camera_fb_t *frame) {

    max_brightness_pixels_t mbp;
    max_brightness_pixels_t *pixels = find_max_brightness_pixels(frame, &mbp);
    
    free(pixels->coords);
    return ESP_OK;
}
