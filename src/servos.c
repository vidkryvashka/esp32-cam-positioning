#include "my_servos.h"
#include "iot_servo.h"

#define TAG "my_servos"


#define SERVO_PLUG_PIN  255
#define SERVO_PAN_PIN   14
#define SERVO_TILT_PIN  15

static servo_config_t servo_cfg = {
    .max_angle = 180,
    .min_width_us = 500,
    .max_width_us = 2500,
    .freq = 50,
    .timer_number = LEDC_TIMER_0,
    .channels = {
        .servo_pin = {
            SERVO_PLUG_CH,
            SERVO_PAN_PIN,
            SERVO_TILT_PIN
        },
        .ch = {
            SERVO_PLUG_CH,
            SERVO_PAN_CH,
            SERVO_TILT_CH
        },
    },
    .channel_number = 3,
};


esp_err_t init_my_servos()
{
    esp_err_t err = iot_servo_init(LEDC_LOW_SPEED_MODE, &servo_cfg);

    return err;
}


float my_servo_get_angle(uint8_t servo_chan)
{
    float angle = -400.0f;
    esp_err_t err = iot_servo_read_angle(LEDC_LOW_SPEED_MODE, servo_chan, &angle);
    if (err) {
        ESP_LOGE(TAG, "failed to get angle\n");
        return -400.0f;
    }
    return angle;
}


esp_err_t deinit_my_servos()
{
    esp_err_t err = iot_servo_deinit(LEDC_LOW_SPEED_MODE);
    
    return err;
}


esp_err_t my_servo_set_angle(uint8_t servo_chan, float angle)
{
    esp_err_t err = iot_servo_write_angle(LEDC_LOW_SPEED_MODE, servo_chan, angle);
    ESP_LOGI(TAG, "chan %d called set_angle, %s ", servo_chan, !err ? "seccess" : "fail");

    return err;
}









// void servo_actions()
// {
//     float servo0angle = my_servo_get_angle(SERVO_PAN_CH);
//     float servo1angle = my_servo_get_angle(SERVO_TILT_CH);
//     ESP_LOGI(TAG, "servos angles: pan: %.0f tilt: %.0f\n", servo0angle, servo1angle);
//     
//     my_servo_set_angle(SERVO_PAN_CH, 0);
//     // ESP_LOGI(TAG, "the plug ");
// }