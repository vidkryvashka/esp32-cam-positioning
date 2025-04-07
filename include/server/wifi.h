#ifndef WIFI_H
#define WIFI_H

#include "defs.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "freertos/event_groups.h"


/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

/**
 * @brief connects to first avialable wifi point configured in sdkconfig or menuconfig
 * @return esp_err_t
 * 
 *          - ESP_OK if connected
 * 
 *          - ESP_ERR if not connected to any configured points
 */
esp_err_t local_start_wifi(void);

#endif