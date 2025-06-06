#include "img_processing/camera.h"

#define TAG "my_operating_with_fb"

camera_fb_t* camera_fb_create(
    const uint16_t width,
    const uint16_t height,
    const pixformat_t format
) {
    camera_fb_t* fb = (camera_fb_t*)heap_caps_malloc(sizeof(camera_fb_t), MALLOC_CAP_SPIRAM);
    if (fb == NULL) {
        return NULL;
    }

    const uint8_t pixel_size = (format == PIXFORMAT_GRAYSCALE) ? 1 : 2;     // PIXFORMAT_RGB565 pixelsize 2 (uint16_t)
    const size_t buffer_size = width * height * pixel_size;

    fb->buf = (uint8_t*)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);
    if (fb->buf == NULL) {
        heap_caps_free(fb);
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
    const camera_fb_t *src)
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


void camera_fb_free(
    camera_fb_t *fb
) {
    if (fb == NULL)
        return;
    if (fb->buf != NULL) {
        heap_caps_free(fb->buf);
        fb->buf = NULL;
    }
    heap_caps_free(fb);
}


camera_fb_t* camera_fb_crop(
    const camera_fb_t *src,
    const rectangle_coords_t *rect
) {
    if (src == NULL || rect == NULL) {
        return NULL;
    }

    uint16_t top_left_x = rect->top_left.x;
    uint16_t top_left_y = rect->top_left.y;

    if (top_left_x + rect->width > src->width || top_left_y + rect->height > src->height) {
        return NULL;
    }

    size_t pixel_size = (src->format == PIXFORMAT_GRAYSCALE) ? 1 : 2;   // PIXFORMAT_RGB565 pixelsize 2 (uint16_t)
    size_t dest_row_size = rect->width * pixel_size;
    camera_fb_t *dest = camera_fb_create(rect->width, rect->height, current_frame->format);

    for (size_t y = top_left_y; y < top_left_y + rect->height; ++y) {
        size_t src_index = (y * src->width + top_left_x) * pixel_size;
        size_t dest_index = (y - top_left_y) * dest_row_size;
        memcpy(dest->buf + dest_index, src->buf + src_index, dest_row_size);
    }

    // dest->timestamp = src->timestamp;
    dest->timestamp = (struct timeval) {0, 0};
    // bzero(dest->timestamp, sizeof(dest->timestamp));

    return dest;
}