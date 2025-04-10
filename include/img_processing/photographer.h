#ifndef FOLLOW_OBJ_IN_IMG_H
#define FOLLOW_OBJ_IN_IMG_H

#include "img_processing/camera.h"


// typedef struct rectangle_coords_t {
//     bool avialable;
//     pixel_coordinate_t up_left;
//     pixel_coordinate_t down_right;
// };


extern volatile bool pause_photographer; // initiated in photographer.c, used in photographer_task, then in webserver.c
// extern volatile SemaphoreHandle_t pause_photographer_mutex; // like an idea

esp_err_t run_photographer();


#endif