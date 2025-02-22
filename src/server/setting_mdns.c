#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_netif_ip_addr.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "mdns.h"
#include "driver/gpio.h"

#include "defs.h"

#include "server/setting_mdns.h"

#ifndef TAG
#define TAG "esp_mdns"
#endif

#define MDNS_HOSTNAME "esp32"
#define MDNS_INSTANCE "esp32_mdns_instance"
// #define BUTTON_GPIO 32

static void initialise_mdns(void) {
    char *hostname = strdup(MDNS_HOSTNAME);

    ESP_ERROR_CHECK(mdns_init());
    ESP_ERROR_CHECK(mdns_hostname_set(hostname));
    ESP_LOGI(TAG, "   --  mDNS hostname set to: [%s]", hostname);
    ESP_ERROR_CHECK(mdns_instance_name_set(MDNS_INSTANCE));

    mdns_txt_item_t serviceTxtData[] = {
        {"board", BOARD},
        {"u", "user"},
        {"p", "password"}
    };

    ESP_ERROR_CHECK(mdns_service_add(MDNS_INSTANCE, "_http", "_tcp", 80, serviceTxtData, 3));
    free(hostname);
}

static void query_mdns_service(const char *service_name, const char *proto) {
    // ESP_LOGI(TAG, "Query PTR: %s.%s.local", service_name, proto);
    mdns_result_t *results = NULL;
    esp_err_t err = mdns_query_ptr(service_name, proto, 3000, 20, &results);
    if (err) {
        ESP_LOGE(TAG, "Query Failed: %s", esp_err_to_name(err));
        return;
    }
    if (!results) {
        // ESP_LOGW(TAG, "No results found!"); // floods idk why, works well and screams
        return;
    }

    mdns_result_t *r = results;
    while (r) {
        if (r->hostname) {
            ESP_LOGI(TAG, "   --  Host: %s, Port: %u", r->hostname, r->port);
        }
        r = r->next;
    }
    mdns_query_results_free(results);
}

static void mdns_task(void *pvParameters) {
    while (1) {
        query_mdns_service("_http", "_tcp");
        vTaskDelay(pdMS_TO_TICKS(3000));            // /* before */ vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void set_mdns(void) {
    initialise_mdns();

    ESP_LOGI(TAG, "   --  got hostname http://%s.local", MDNS_HOSTNAME);

    xTaskCreate(&mdns_task, "mdns_task", 4096, NULL, 5, NULL);
}
