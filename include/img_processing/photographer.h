#ifndef FOLLOW_OBJ_IN_IMG_H
#define FOLLOW_OBJ_IN_IMG_H


#include "my_vector.h"
#include "img_processing/camera.h"
#include "freertos/semphr.h"

#if ANALISIS_MODE == MODE_FIND_SUN
    #include "img_processing/find_sun.h"
#elif ANALISIS_MODE == MODE_FAST9
    #include "img_processing/ORB_defs.h"
#endif

extern volatile bool pause_photographer; // initiated in photographer.c, used in photographer_task, then in webserver.c
// extern volatile SemaphoreHandle_t pause_photographer_mutex; // like an idea

#define PHOTOGRAPHER_DELAY_MS 500


typedef struct {
#if ANALISIS_MODE == MODE_FIND_SUN
    max_brightness_pixels_t mbp;
    // keypoints_t keypoints;
#elif ANALISIS_MODE == MODE_FAST9
    keypoints_t keypoints;
    vector_t *keypoints;
#endif
    bool need2ORB;
} keypoints_shell_t;


/**
 * @brief to private variable in photographer.c
 * 
 * @return keypoints_shell_t* 
 */
keypoints_shell_t* get_keypoints_shell_reference();


/**
 * @brief writes cropped current_frame into itself by rect_coords scema, changes translation unit global variables
 * 
 * @param rect_coords 
 * @return camera_fb_t* pointer to fragment
 */
camera_fb_t* operate_fragment(
    const rectangle_coords_t *rect_coords
);


esp_err_t run_photographer();


#endif