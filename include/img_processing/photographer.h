#ifndef FOLLOW_OBJ_IN_IMG_H
#define FOLLOW_OBJ_IN_IMG_H

#include "img_processing/camera.h"


extern volatile bool pause_photographer; // initiated in photographer.c, used in photographer_task, then in webserver.c
// extern volatile SemaphoreHandle_t pause_photographer_mutex; // like an idea

#define PHOTOGRAPHER_DELAY_MS 2000


camera_fb_t * write_fragment(rectangle_coords_t rect_coords);

esp_err_t run_photographer();


#endif