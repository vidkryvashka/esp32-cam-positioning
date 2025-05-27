#ifndef MY_SERVOS_H
#define MY_SERVOS_H

#include "defs.h"


#define SERVO_PLUG_CH   0   // if calls 0 chan no matter which pin is under it, mutexes start race condition / deadlock
#define SERVO_PAN_CH    1
#define SERVO_TILT_CH   2


esp_err_t init_my_servos();

esp_err_t deinit_my_servos();


float my_servo_get_angle(uint8_t servo_chan);

esp_err_t my_servo_set_angle(uint8_t servo_chan, float angle);


#endif