#ifndef FOLLOW_OBJ_IN_IMG_H
#define FOLLOW_OBJ_IN_IMG_H

#include "img_processing/defs.h"


typedef struct rectangle_coords_t {
    bool avialable;
    pixel_coordinate_t up_left;
    pixel_coordinate_t down_right;
};

esp_err_t compare_frames();


#endif