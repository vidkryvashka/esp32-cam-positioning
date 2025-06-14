#ifndef DEFS
#define DEFS

#include <string.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <esp_heap_caps.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"



#define BOARD "esp32"

#define PHOTOGRAPHER_DELAY_MS 1000

// #define MODE_FIND_SUN   0
// #define MODE_FAST9      1

typedef enum {
    MODE_FIND_SUN = 0,
    MODE_FAST9
} analysis_mode_t;

#define ANALISIS_MODE MODE_FAST9

#define MEMORY_LOG_PROBABILITY_DIVIDER 10


/** 
 * @brief inits gpio, nvs_flash
 */
esp_err_t init_esp_things(void);


/**
 * @brief Blinks with led. Led pin defined as LED_PIN
 * 
 * @param periods used for defined BLINK_PERIOD multiplication or division
 * @param count how many times LED will blint with that period
 */
void led_blink(float periods, uint8_t count);


/**
 * @brief to send by servo_queue and controll servos
 */
typedef struct {
    int16_t pan;
    int16_t tilt;
} angles_diff_t;


/**
 * @brief logs heap memory info
 * use xPortGetCoreID() as param;
 * @param core_id
 */
void log_memory(BaseType_t core_id);


#endif