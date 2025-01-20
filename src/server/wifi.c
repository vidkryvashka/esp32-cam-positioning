#include "server/wifi.h"
#include "server/setting_mdns.h"

#ifndef TAG
#define TAG "esp_wifi"
#endif

/* FreeRTOS event group to signal when we are connected */
static EventGroupHandle_t s_wifi_event_group;

static int s_retry_num = 0;

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    // ESP_LOGI(TAG, "Івент затронуто: base=%s, id=%ld", event_base, event_id);
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < CONFIG_ESP_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP ");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail ");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}


static void form_wifi_points(wifi_sta_config_t *wifi_points)
{
    wifi_sta_config_t wifi_point_0 = {
        .ssid = CONFIG_ESP_WIFI_SSID_0,
        .password = CONFIG_ESP_WIFI_PASSWORD_0,
        .threshold.authmode = WIFI_AUTH_WPA2_PSK,
    };

    wifi_sta_config_t wifi_point_1 = {
        .ssid = CONFIG_ESP_WIFI_SSID_1,
        .password = CONFIG_ESP_WIFI_PASSWORD_1,
        .threshold.authmode = WIFI_AUTH_WPA2_PSK,
    };

    wifi_sta_config_t wifi_point_2 = {
        .ssid = CONFIG_ESP_WIFI_SSID_2,
        .password = CONFIG_ESP_WIFI_PASSWORD_2,
        .threshold.authmode = WIFI_AUTH_WPA2_PSK,
    };

    wifi_sta_config_t wifi_point_3 = {
        .ssid = CONFIG_ESP_WIFI_SSID_3,
        .password = CONFIG_ESP_WIFI_PASSWORD_3,
        .threshold.authmode = WIFI_AUTH_WPA2_PSK,
    };

    wifi_sta_config_t wifi_point_4 = {
        .ssid = CONFIG_ESP_WIFI_SSID_4,
        .password = CONFIG_ESP_WIFI_PASSWORD_4,
        .threshold.authmode = WIFI_AUTH_WPA2_PSK,
    };

    wifi_points[0] = wifi_point_0;
    wifi_points[1] = wifi_point_1;
    wifi_points[2] = wifi_point_2;
    wifi_points[3] = wifi_point_3;
    wifi_points[4] = wifi_point_4;
}


static esp_err_t find_wifi() {
    esp_err_t err = ESP_FAIL;
    wifi_sta_config_t wifi_points[CONFIG_ESP_WIFI_MAX_POINTS_NUMBER];
    form_wifi_points(wifi_points);

    for (
        int index = 0, connected = 0;
        // !connected && index < CONFIG_ESP_WIFI_POINTS_NUMBER;
        !connected;
        ++ index
    ) {
    
        if (index > CONFIG_ESP_WIFI_POINTS_NUMBER)
            index = 0;
        
        wifi_config_t wifi_config = {
            .sta = wifi_points[index],
        };
        ESP_LOGI(TAG, "Trying to connect to SSID: %s, Password: %s (index: %d)",
                 wifi_points[index].ssid, wifi_points[index].password, index);

        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

        ESP_ERROR_CHECK(esp_wifi_start());

        s_retry_num = 0;
        EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                               WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                               pdTRUE,
                                               pdFALSE,
                                               portMAX_DELAY);

        if (bits & WIFI_CONNECTED_BIT) {
            ESP_LOGI(TAG, "Successfully connected to SSID: %s", wifi_points[index].ssid);
            connected = 1;
            err = ESP_OK;
        }
        else if (bits & WIFI_FAIL_BIT) {
            ESP_LOGW(TAG, "Failed to connect to SSID: %s", wifi_points[index].ssid);
            led_blink(1, 2);
        } else {
            ESP_LOGE(TAG, "Unexpected event during Wi-Fi connection.");
        }

        if (!connected) {
            ESP_ERROR_CHECK(esp_wifi_stop());
        }
    }

    return err;
}

esp_err_t connect_wifi(void)
{
    esp_err_t err = ESP_FAIL;
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *netif = esp_netif_create_default_wifi_sta();
    esp_netif_set_hostname(netif, "esp32");

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id)
    );
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip)
    );

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    err = find_wifi();
    if (err) {
        ESP_LOGE(TAG, "All Wi-Fi connection attempts failed.");
    }
    set_mdns();

    vEventGroupDelete(s_wifi_event_group);
    return err;

}