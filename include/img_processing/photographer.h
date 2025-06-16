#ifndef FOLLOW_OBJ_IN_IMG_H
#define FOLLOW_OBJ_IN_IMG_H


#include "my_vector.h"
#include "img_processing/camera.h"
#include "freertos/semphr.h"


typedef struct {
    pixels_cloud_t pixels_cloud;
} keypoints_shell_t;


/**
 * @brief To get private variable in photographer.c
 * 
 * @return keypoints_shell_t* 
 */
keypoints_shell_t* get_keypoints_shell_reference(void);

/**
 * @brief Starts & configures task photographer, determines global variables
 * 
 * @return esp_err_t 
 */
esp_err_t run_photographer(void);


#endif