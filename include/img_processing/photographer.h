#ifndef PHOTOGRAPHER_H
#define PHOTOGRAPHER_H

#include "img_processing/camera.h"


// typedef struct rectangle_coords_t {
//     bool avialable;
//     pixel_coordinate_t up_left;
//     uint8_t width;
//     uint8_t height;
// };

// extern SemaphoreHandle_t pause_mutex; // М’ютекс для pause_photographer
extern bool pause_photographer; // initiated in follow_obj_in_img.c, used in photographer_task, then in webserver.c

esp_err_t compare_frames();


#endif