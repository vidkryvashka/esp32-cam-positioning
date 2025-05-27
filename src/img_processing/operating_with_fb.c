#include "img_processing/camera.h"

#define TAG "my_operating_with_fb"


camera_fb_t* camera_fb_create(size_t width, size_t height, pixformat_t format)
{
    camera_fb_t* fb = (camera_fb_t*)malloc(sizeof(camera_fb_t));
    if (fb == NULL) {
        return NULL;
    }

    const uint8_t pixel_size = 2; // RGB565
    const size_t buffer_size = width * height * pixel_size;

    fb->buf = (uint8_t*)malloc(buffer_size);
    if (fb->buf == NULL) {
        free(fb);
        return NULL;
    }

    fb->len = buffer_size;
    fb->width = width;
    fb->height = height;
    fb->format = format;
    gettimeofday(&fb->timestamp, NULL);

    return fb;
}


camera_fb_t* camera_fb_copy(
    const camera_fb_t* src)
{
    if (src == NULL) {
        return NULL;
    }

    camera_fb_t* dest = camera_fb_create(src->width, src->height, src->format);
    if (dest == NULL) {
        return NULL;
    }

    memcpy(dest->buf, src->buf, src->len);
    dest->timestamp = src->timestamp;

    return dest;
}


void camera_fb_free(camera_fb_t* fb)
{
    if (fb == NULL)
        return;
    if (fb->buf != NULL)
        free(fb->buf);
    free(fb);
}


esp_err_t camera_fb_crop(
    camera_fb_t *dest,
    const camera_fb_t *src,
    const rectangle_coords_t *rect
) {
    if (src == NULL || rect == NULL) {
        return ESP_FAIL;
    }

    uint8_t top_left_x = rect->top_left.x;
    uint8_t top_left_y = rect->top_left.y;
    uint8_t crop_width = rect->width;
    uint8_t crop_height = rect->height;

    if (top_left_x + crop_width > src->width || top_left_y + crop_height > src->height) {
        return ESP_FAIL; // Помилка: прямокутник виходить за межі зображення
    }

    size_t pixel_size = 2; // RGB565
    // size_t src_row_size = src->width * pixel_size;
    size_t dest_row_size = crop_width * pixel_size;

    // Копіюємо пікселі
    for (size_t y = top_left_y; y < top_left_y + crop_height; ++y) {
        size_t src_index = (y * src->width + top_left_x) * pixel_size;    // Початок рядка у вихідному буфері
        size_t dest_index = (y - top_left_y) * dest_row_size;             // Початок рядка у новому буфері
        memcpy(dest->buf + dest_index, src->buf + src_index, dest_row_size);
    }

    dest->timestamp = src->timestamp;
    // dest->timestamp = (struct timeval) {0, 0};
    // bzero(dest->timestamp, sizeof(dest->timestamp));

    return ESP_OK;
}



// static void example()
// {
//     // Створюємо зображення 96x96
//     camera_fb_t* original = camera_fb_create(96, 96, PIXFORMAT_RGB565);
//     if (original == NULL) {
//         printf("Failed to create original image\n");
//         return -1;
//     }
// 
//     // Заповнюємо буфер (для прикладу, просто нулями)
//     memset(original->buf, 0, original->len);
// 
//     // Визначаємо координати для вирізання (наприклад, 10x10 з позиції (20, 30))
//     rectangle_coords_t rect = {{20, 30}, 10, 10};
// 
//     // Вирізаємо частину зображення
//     camera_fb_t* cropped = camera_fb_crop(original, &rect);
//     if (cropped == NULL) {
//         printf("Failed to crop image\n");
//     } else {
//         printf("Cropped image: %zux%zu\n", cropped->width, cropped->height);
//     }
// 
//     // Копіюємо оригінал
//     camera_fb_t* copy = camera_fb_copy(original);
//     if (copy == NULL) {
//         printf("Failed to copy image\n");
//     } else {
//         printf("Copied image: %zux%zu\n", copy->width, copy->height);
//     }
// 
//     // Очищаємо пам’ять
//     camera_fb_destroy(original);
//     camera_fb_destroy(cropped);
//     camera_fb_destroy(copy);
// }