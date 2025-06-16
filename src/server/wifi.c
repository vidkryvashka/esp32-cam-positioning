#include "server/wifi.h"


#define TAG "my_wifi"


#if !CONFIG_ESP_WIFI_SOFTAP_ENABLE
static uint8_t s_retry_num = 0;
static EventGroupHandle_t s_wifi_event_group;   // FreeRTOS event group to signal when we are connected
#endif

static void event_handler(
    void *arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data
) {
#if CONFIG_ESP_WIFI_SOFTAP_ENABLE
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
        ESP_LOGI(TAG, "Device connected to ESP32 SoftAP");
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        ESP_LOGI(TAG, "Device disconnected from ESP32 SoftAP");
    }
#else
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)   // esp_base is (char *) 🤯 can't switch case
        esp_wifi_connect();
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < CONFIG_ESP_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            ++ s_retry_num;
            ESP_LOGI(TAG, "retry to connect to the AP ");
        }
        else
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        ESP_LOGI(TAG, "connect to the AP fail ");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
#endif
}

#if !CONFIG_ESP_WIFI_SOFTAP_ENABLE
static void form_wifi_points(
    wifi_sta_config_t *wifi_points
) {
    const wifi_sta_config_t wifi_points_src[CONFIG_ESP_WIFI_MAX_POINTS_NUMBER] = {
        {CONFIG_ESP_WIFI_SSID_0, CONFIG_ESP_WIFI_PASSWD_0, .threshold.authmode = WIFI_AUTH_WPA2_PSK},
        {CONFIG_ESP_WIFI_SSID_1, CONFIG_ESP_WIFI_PASSWD_1, .threshold.authmode = WIFI_AUTH_WPA2_PSK},
        {CONFIG_ESP_WIFI_SSID_2, CONFIG_ESP_WIFI_PASSWD_2, .threshold.authmode = WIFI_AUTH_WPA2_PSK},
        {CONFIG_ESP_WIFI_SSID_3, CONFIG_ESP_WIFI_PASSWD_3, .threshold.authmode = WIFI_AUTH_WPA2_PSK},
        {CONFIG_ESP_WIFI_SSID_4, CONFIG_ESP_WIFI_PASSWD_4, .threshold.authmode = WIFI_AUTH_WPA2_PSK},
    };
    
    memcpy(wifi_points, wifi_points_src, sizeof(wifi_points_src));
}


static esp_err_t find_wifi(
    void
) {
    esp_err_t err = ESP_FAIL;
    wifi_sta_config_t *wifi_points = (wifi_sta_config_t *)heap_caps_malloc(
        CONFIG_ESP_WIFI_MAX_POINTS_NUMBER * sizeof(wifi_sta_config_t), 
        MALLOC_CAP_SPIRAM
    );
    if (!wifi_points) {
        ESP_LOGE(TAG, "Failed to allocate SPIRAM for wifi_points");
        return ESP_FAIL;
    }
    form_wifi_points(wifi_points);
    
    for (uint8_t index = 0, connected = 0;
        !connected;
        index++
    ) {
        if (index >= CONFIG_ESP_WIFI_POINTS_NUMBER)
            index = 0;
        
        wifi_config_t wifi_config = {
            .sta = wifi_points[index]
        };
        ESP_LOGI(TAG, "Trying to connect to SSID: %s, Password: %s (index: %d) ",
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
            ESP_LOGI(TAG, "Successfully connected to SSID: %s ", wifi_points[index].ssid);
            connected = 1;
            err = ESP_OK;
        }
        else if (bits & WIFI_FAIL_BIT) {
            ESP_LOGW(TAG, "Failed to connect to SSID: %s ", wifi_points[index].ssid);
            led_blink(1, 2);
        } else
            ESP_LOGE(TAG, "Unexpected event during Wi-Fi connection. ");

        if (!connected)
            ESP_ERROR_CHECK(esp_wifi_stop());
    }
    free(wifi_points);
    return err;
}


/**
 * @brief connects to first avialable wifi point configured in sdkconfig or menuconfig
 * @return esp_err_t OK if connected
 * 
 *          - ESP_OK if connected
 * 
 *          - ESP_ERR if not connected to any configured points
 */
static esp_err_t connect_wifi(void)
{
    esp_err_t err = ESP_OK;
    s_wifi_event_group = xEventGroupCreate();

    err += esp_netif_init();

    err += esp_event_loop_create_default();
    esp_netif_t *netif = esp_netif_create_default_wifi_sta();
    esp_netif_set_hostname(netif, "esp32");

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    err += esp_wifi_init(&cfg);

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    err += esp_event_handler_instance_register(WIFI_EVENT,
                                               ESP_EVENT_ANY_ID,
                                               &event_handler,
                                               NULL,
                                               &instance_any_id);
    err += esp_event_handler_instance_register(IP_EVENT,
                                               IP_EVENT_STA_GOT_IP,
                                               &event_handler,
                                               NULL,
                                               &instance_got_ip);

    err += esp_wifi_set_mode(WIFI_MODE_STA);

    err += find_wifi();
    if (err) {
        ESP_LOGE(TAG, "All Wi-Fi connection attempts failed.");
    }

    vEventGroupDelete(s_wifi_event_group);
    return err;
}

#else


/**
 * @brief creates own wifi pioint to which connects client (laptop / phone)
 * @return
 * 
 *          - ESP_OK if everything good
 * 
 *          - ESP_ERR if anything failed
 */
static esp_err_t init_softap(void)
{
    esp_err_t err_sum = ESP_OK;
    err_sum += esp_netif_init();
    err_sum += esp_event_loop_create_default();
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    err_sum += esp_wifi_init(&cfg);

    err_sum += esp_event_handler_register(WIFI_EVENT,
                                          ESP_EVENT_ANY_ID,
                                          &event_handler,
                                          NULL
    );

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = CONFIG_ESP_WIFI_SOFTAP_SSID,
            .ssid_len = strlen(CONFIG_ESP_WIFI_SOFTAP_SSID),
            .password = CONFIG_ESP_WIFI_SOFTAP_PASSWD,
            .channel = 11, // without this line doesn't even ping  // EXAMPLE_ESP_WIFI_CHANNEL,
            .max_connection = CONFIG_ESP_WIFI_SOFTAP_MAX_CLIENTS,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .required = true,
            },
        },
    };

    err_sum += esp_wifi_set_mode(WIFI_MODE_AP);
    err_sum += esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);
    err_sum += esp_wifi_start();

    char log_err_sum[3];
    sprintf(log_err_sum, "%d", err_sum);
    ESP_LOGI(TAG, "SoftAP started with SSID: %s, Password: %s, %s %s",
        CONFIG_ESP_WIFI_SOFTAP_SSID, CONFIG_ESP_WIFI_SOFTAP_PASSWD,
        !err_sum ? "err_sum" : "", !err_sum ? log_err_sum : "");

    return err_sum;
}

#endif

esp_err_t local_start_wifi(void)
{
    esp_err_t err;

#if CONFIG_ESP_WIFI_SOFTAP_ENABLE
    #if !CONFIG_ESP_WIFI_SOFTAP_SUPPORT
        #error "enabled unsupported wifi softAP"
    #endif
    ESP_LOGI(TAG, "Starting SoftAP");
    err = init_softap();
#else
    ESP_LOGI(TAG, "Starting connection");
    err = connect_wifi();
#endif
    
    return err;
}