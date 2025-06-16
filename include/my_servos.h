#ifndef MY_SERVOS_H
#define MY_SERVOS_H

#include "defs.h"
#include "freertos/queue.h"


#define SERVO_PAN_CHANNEL   LEDC_CHANNEL_1  // to pin 14
#define SERVO_TILT_CHANNEL  LEDC_CHANNEL_2  // to pin 15


#define ANGLE_THRESHOLD     5   // degrees ignore

extern bool are_servos_inited;

#define SERVO_QUEUE_LEN     5
extern QueueHandle_t servo_queue;


esp_err_t init_my_servos(void);

esp_err_t run_servo_manager(void);

#endif