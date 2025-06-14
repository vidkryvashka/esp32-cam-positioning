#include <math.h>

#include "img_processing/find_sun.h"
#include "my_vector.h"

#define TAG "my_find_sun"


#define STD_BRIGHTEST_PIXELS_COUNT 16
#define ROUNDF2U8(X) ((X >= 0.0f) ? (uint16_t)(X + 0.5f) : 0)

static esp_err_t calc_brightest_center(
    pixels_cloud_t *pixels_cloud
) {
    float avg_x = 0, avg_y = 0;
    for (size_t i = 0; i < pixels_cloud->coords->size; i++) {
        pixel_coord_t *coord = (pixel_coord_t *)vector_get(pixels_cloud->coords, i);
        avg_x += ROUNDF2U8((float)coord->x / (float)pixels_cloud->coords->size);
        avg_y += ROUNDF2U8((float)coord->y / (float)pixels_cloud->coords->size);
    }
    pixels_cloud->center_coord = (pixel_coord_t){avg_x, avg_y};
    return ESP_OK;
}


static esp_err_t balance_max_pixels_count(
    pixels_cloud_t *pixels_cloud
) {
    if (pixels_cloud->coords->size <= STD_BRIGHTEST_PIXELS_COUNT) {
        return ESP_OK;
    }

    uint16_t pixels_divider = pixels_cloud->coords->size / STD_BRIGHTEST_PIXELS_COUNT + 1;
    vector_t *new_coords = vector_create(sizeof(pixel_coord_t));
    if (!new_coords) {
        ESP_LOGE(TAG, "balance_max_pixels_count vector_create failed");
        return ESP_FAIL;
    }

    for (size_t i = 0; i < pixels_cloud->coords->size; i += pixels_divider) {
        pixel_coord_t *coord = (pixel_coord_t *)vector_get(pixels_cloud->coords, i);
        if (vector_push_back(new_coords, coord) != ESP_OK) {
            vector_destroy(new_coords);
            return ESP_FAIL;
        }
    }

    vector_destroy(pixels_cloud->coords);
    pixels_cloud->coords = new_coords;
    return ESP_OK;
}


static esp_err_t find_max_brightness_pixels(
    pixels_cloud_t *pixels_cloud,
    const camera_fb_t *frame
) {
    uint8_t max_brightness = 0;
    vector_clear(pixels_cloud->coords);
    pixels_cloud->center_coord = (pixel_coord_t){0, 0};
    for (uint16_t y = 0; y < frame->height; y++) {
        for (uint16_t x = 0; x < frame->width; x++) {
            uint16_t index = y * frame->width + x;
            uint8_t brightness = 0;

#if PIXFORMAT_CHOISE == PIXFORMAT_GRAYSCALE
            brightness = ((uint8_t *)frame->buf)[index];
#elif PIXFORMAT_CHOISE == PIXFORMAT_RGB565
            uint16_t pixel = (uint16_t *)frame->buf[index];
            uint8_t r = (pixel >> 11) & 0x1F;
            uint8_t g = (pixel >> 5) & 0x3F;
            uint8_t b = pixel & 0x1F;
            brightness = (r + g + b);
#else
            ESP_LOGE(TAG, "pixels_cloud_t unsupported camera)fb_t.format ");
#endif

            if (brightness > max_brightness) {
                max_brightness = brightness;
                vector_clear(pixels_cloud->coords);
            }

            if (brightness == max_brightness) {
                pixel_coord_t coord = {x, y};
                if (vector_push_back(pixels_cloud->coords, &coord) != ESP_OK) {
                    ESP_LOGE(TAG, "find_max_brightness_pixels vector_push_back failed");
                    vector_clear(pixels_cloud->coords);
                    return ESP_FAIL;
                }
            }
        }
    }
    size_t orig_mbp_count = pixels_cloud->coords->size;

    esp_err_t err = balance_max_pixels_count(pixels_cloud);
    if (err != ESP_OK) {
        vector_clear(pixels_cloud->coords);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Found %zu / %zu max brightness pixels %d ", 
            pixels_cloud->coords->size, orig_mbp_count, max_brightness);

    calc_brightest_center(pixels_cloud);
    return ESP_OK;
}


esp_err_t mark_sun(
    pixels_cloud_t *pixels_cloud,
    const camera_fb_t *frame
) {
    esp_err_t err = find_max_brightness_pixels(pixels_cloud, frame);
    if (err) {
        ESP_LOGE(TAG, "find_max_brightness_pixels err %d ", err);
        return err;
    }
    
    return ESP_OK;
}