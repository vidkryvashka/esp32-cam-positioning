#ifndef OPERATING_WITH_FB_H
#define OPERATING_WITH_FB_H

#include "esp_camera.h"


camera_fb_t* camera_fb_create(size_t width, size_t height, pixformat_t format);


camera_fb_t* camera_fb_copy(const camera_fb_t* src);


void camera_fb_free(camera_fb_t* fb);


camera_fb_t* camera_fb_crop(const camera_fb_t* src, const rectangle_coords_t* rect);


#endif