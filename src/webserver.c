#include "webserver.h"
#include "index_html.h"

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

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
    wifi_sta_config_t wifi_points[CONFIG_ESP_WIFI_POINTS_NUMBER];
    form_wifi_points(wifi_points);

    for (
        int index = 0, connected = 0;
        !connected && index < CONFIG_ESP_WIFI_POINTS_NUMBER;
        ++ index
    ) {
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
        // vTaskDelay(pdMS_TO_TICKS(100));

    }

    return err;
}


static esp_err_t connect_wifi(void)
{
    esp_err_t err = ESP_FAIL;
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

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

    vEventGroupDelete(s_wifi_event_group);
    return err;

}


static esp_err_t send_web_page(httpd_req_t *req) {
    return httpd_resp_send(req, (const char *)rc_index_html, rc_index_html_len);
}


static esp_err_t get_req_handler(httpd_req_t *req)
{
    return send_web_page(req);
}


static esp_err_t jpg_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "got jpg uri req");
    camera_fb_t *pic = esp_camera_fb_get();
    size_t jpg_buf_len = 0;
    uint8_t *jpg_buf = NULL;

    if (pic->format == PIXFORMAT_RGB565) {
        bool converted = frame2jpg(pic, 80, &jpg_buf, &jpg_buf_len);
        if (!converted) {
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }

        // Set response content type to image/jpeg
        httpd_resp_set_type(req, "image/jpeg");
        // Send the JPEG buffer
        httpd_resp_send(req, (const char *)jpg_buf, jpg_buf_len);
        ESP_LOGI(TAG, " -- sent picture -- ");

        // Free the buffer
        free(jpg_buf);
        esp_camera_fb_return(pic);
        return ESP_OK;
    }
    httpd_resp_send_500(req);
    return ESP_FAIL;
}


static httpd_uri_t uri_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = get_req_handler,
    .user_ctx = NULL
};


static httpd_uri_t uri_camera = {
    .uri = "/camera",
    .method = HTTP_GET,
    .handler = jpg_handler,
    .user_ctx = NULL
};


static httpd_handle_t setup_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_camera);
    }

    return server;
}


esp_err_t server_up(void) {
    esp_err_t err = ESP_FAIL;
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    esp_err_t wifi_err = connect_wifi();

    if (!wifi_err) {
        setup_server();
        ESP_LOGI(TAG, "\n Server must be up, good boy \n");
        err = ESP_OK;
        led_blink(0.3, 10);
    } else {
        ESP_LOGE(TAG, "\n Server not started \n");
        led_blink(3, 3);
    }
    
    return err;
}