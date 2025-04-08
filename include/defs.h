#ifndef DEFS
#define DEFS

#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"


#define BOARD "esp32"

// idk why on is 0 and off is 1
#define LED_ON()    gpio_set_level(LED_PIN, 0)
#define LED_OFF()   gpio_set_level(LED_PIN, 1)


/** 
 * @brief inits gpio, nvs_flash
 */
esp_err_t init_esp_things(void);

esp_err_t run_photographer();

// bool esp_camera_inited = false;


/**
 * @brief Blinks with led. Led pin defined as LED_PIN
 * 
 * @param periods used for defined BLINK_PERIOD multiplication or division
 * @param count how many times LED will blint with that period
 */
void led_blink(float periods, uint8_t count);


#endif