/**
 * This example takes a picture every 5s and print its size on serial monitor.
 */

#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// support IDF 5.x
#ifndef portTICK_RATE_MS
#define portTICK_RATE_MS portTICK_PERIOD_MS
#endif

#include "defs.h"
#include "camera.h"
#include "webserver.h"


void app_main(void)
{
    server_up();
}
