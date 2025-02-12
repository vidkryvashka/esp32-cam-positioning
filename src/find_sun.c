#include "find_sun.h"

#define TAG "esp:find_sun"

#define BRIGHTEST_PIXELS_RADIUS 5
#define CENTER_BTIGHT_PIXEL_RADIUS 10

// too much brightest pixels (expl 60, 7740), we will consider every DIVIDERs brightest pixel
// maybe in future this threshold will be dynamic
size_t pixels_divider = 1;

typedef struct {
    size_t x;
    size_t y;
    // size_t width;
    // size_t height;
} pixel_coordinate_t;

typedef struct {
    pixel_coordinate_t *coords;
    size_t count;
    pixel_coordinate_t center_coord;
} max_brightness_pixels_t;


static max_brightness_pixels_t find_max_brightness_pixels(camera_fb_t *frame) {
    uint16_t *pixel_data = (uint16_t *)frame->buf;
    size_t max_brightness = 0;
    max_brightness_pixels_t result = {NULL, 0, {0, 0}};

    // for logging
    uint8_t max_r = 0, max_g = 0, max_b = 0;
    uint8_t pixels_divider_counter = 0;


    for (size_t y = 0; y < frame->height; ++y) {
        for (size_t x = 0; x < frame->width; ++x) {
            size_t index = y * frame->width + x;
            uint16_t pixel = pixel_data[index];
            uint8_t r = (pixel >> 11) & 0x1F;
            uint8_t g = (pixel >> 5) & 0x3F;
            uint8_t b = pixel & 0x1F;
            size_t brightness = (r + g + b); // simplest brightness formula

            if (brightness > max_brightness) {
                max_brightness = brightness;
                // for logging
                max_r = r;  max_g = g;  max_b = b;
                if (result.coords) {
                    free(result.coords);
                }
                result.coords = NULL;
                result.count = 0;
            }

            if (brightness == max_brightness) {
                if (pixels_divider_counter == 10)
                    pixels_divider_counter = 0;
                
                if (pixels_divider_counter == 0) {
                    ++ result.count;
                    result.coords = (pixel_coordinate_t *)realloc(result.coords, result.count * sizeof(pixel_coordinate_t));
                    if (result.coords == NULL) {
                        perror("Memory allocation failed");
                        exit(EXIT_FAILURE);
                    }
                    result.coords[result.count - 1].x = x;
                    result.coords[result.count - 1].y = y;
                }
                ++ pixels_divider_counter;
            }
        }
    }

    // for logging
    ESP_LOGI(TAG, "Found %zu pixels with max r: %hu g: %hu b: %hu", result.count, max_r, max_g, max_b);

    return result;
}


static void log_bright_coords(camera_fb_t *frame) {
    max_brightness_pixels_t pixels = find_max_brightness_pixels(frame);

    // Звільнення пам'яті
    free(pixels.coords);
}

esp_err_t mark_sun(camera_fb_t *frame) {
    log_bright_coords(frame);

    return ESP_OK;
}








// static int example_main() {
//     // Приклад використання
//     camera_fb_t frame;
//     frame.width = 320;
//     frame.height = 240;
//     frame.buf = (uint8_t *)malloc(frame.width * frame.height * 2); // RGB565: 2 байти на піксель
//     // Заповніть frame.buf тестовими даними...
// 
//     max_brightness_pixels_t pixels = find_max_brightness_pixels(&frame);
// 
//     printf("Found %zu pixels with maximum brightness:\n", pixels.count);
//     for (size_t i = 0; i < pixels.count; i++) {
//         printf("Pixel %zu: (x: %zu, y: %zu)\n", i + 1, pixels.coords[i].x, pixels.coords[i].y);
//     }
// 
//     // Звільнення пам'яті
//     free(pixels.coords);
//     free(frame.buf);
// 
//     return 0;
// }