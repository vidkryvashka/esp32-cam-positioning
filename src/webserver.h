#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h> //Requires by memset
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// #include "esp_system.h" // deprecated, replaced with "esp_spi_flash.h"
#include "esp_spi_flash.h"
// #include "spi_flash_mmap.h"
#include <esp_http_server.h>

#include "esp_wifi.h"
#include "esp_event.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "driver/gpio.h"
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/api.h>
#include <lwip/netdb.h>

#include "defs.h"
#include "camera.h"

#include "index_html.h"

// #define EXAMPLE_ESP_WIFI_SSID CONFIG_ESP_WIFI_SSID
// #define EXAMPLE_ESP_WIFI_PASS CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_MAXIMUM_RETRY CONFIG_ESP_MAXIMUM_RETRY

/* FreeRTOS event group to signal when we are connected */
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static int s_retry_num = 0;


static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
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
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}


static void connect_wifi(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());   // тут в новому коді виникає помилка
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

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_ESP_WIFI_SSID_1,
            .password = CONFIG_ESP_WIFI_PASSWORD_1,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 CONFIG_ESP_WIFI_SSID_2, CONFIG_ESP_WIFI_PASSWORD_1);
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 CONFIG_ESP_WIFI_SSID_2, CONFIG_ESP_WIFI_PASSWORD_1);
    }
    else
    {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
    vEventGroupDelete(s_wifi_event_group);
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

#endif


void server_up(void) {
        // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    connect_wifi();

    // GPIO initialization
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    ESP_LOGI(TAG, "LED Control Web Server is running ... ...\n");
    setup_server();
    ESP_LOGI(TAG, "\nserver is up, good boiii\n");

    if(ESP_OK != init_camera()) {
        return;
    }


}



//// new not so working code

// #ifndef WEBSERVER_H
// #define WEBSERVER_H

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h> //Requires by memset
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// // #include "esp_system.h" // deprecated, replaced with "esp_spi_flash.h"
// #include "esp_spi_flash.h"
// // #include "spi_flash_mmap.h"
// #include <esp_http_server.h>

// #include "esp_wifi.h"
// #include "esp_event.h"
// #include "freertos/event_groups.h"
// #include "esp_log.h"
// #include "nvs_flash.h"
// #include "esp_netif.h"
// #include "driver/gpio.h"
// #include <lwip/sockets.h>
// #include <lwip/sys.h>
// #include <lwip/api.h>
// #include <lwip/netdb.h>

// #include "defs.h"
// #include "camera.h"

// #include "index_html.h"

// // #define EXAMPLE_ESP_WIFI_SSID CONFIG_ESP_WIFI_SSID
// // #define EXAMPLE_ESP_WIFI_PASS CONFIG_ESP_WIFI_PASSWORD
// #define EXAMPLE_ESP_MAXIMUM_RETRY CONFIG_ESP_MAXIMUM_RETRY

// /* FreeRTOS event group to signal when we are connected */
// static EventGroupHandle_t s_wifi_event_group;

// /* The event group allows multiple bits for each event, but we only care about two events:
//  * - we are connected to the AP with an IP
//  * - we failed to connect after the maximum amount of retries */
// #define WIFI_CONNECTED_BIT BIT0
// #define WIFI_FAIL_BIT BIT1

// static int s_retry_num = 0;


// static void event_handler(void *arg, esp_event_base_t event_base,
//                           int32_t event_id, void *event_data)
// {
//     if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
//     {
//         esp_wifi_connect();
//     }
//     else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
//     {
//         if (s_retry_num < CONFIG_ESP_MAXIMUM_RETRY)
//         {
//             esp_wifi_connect();
//             s_retry_num++;
//             ESP_LOGI(TAG, "retry to connect to the AP");
//         }
//         else
//         {
//             xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
//         }
//         ESP_LOGI(TAG, "connect to the AP fail");
//     }
//     else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
//     {
//         ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
//         ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
//         s_retry_num = 0;
//         xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
//     }
// }

// static void form_wifi_points(wifi_sta_config_t *wifi_points)
// {
//     wifi_sta_config_t wifi_point_0 = {
//         .ssid = "lastivky_romashky", // CONFIG_ESP_WIFI_SSID_0,
//         .password = "vyshni_chereshni", // CONFIG_ESP_WIFI_PASSWORD_0,
//         .threshold.authmode = WIFI_AUTH_WPA2_PSK,
//     };

//     wifi_sta_config_t wifi_point_1 = {
//         .ssid = CONFIG_ESP_WIFI_SSID_1,
//         .password = CONFIG_ESP_WIFI_PASSWORD_1,
//         .threshold.authmode = WIFI_AUTH_WPA2_PSK,
//     };

//     wifi_sta_config_t wifi_point_2 = {
//         .ssid = CONFIG_ESP_WIFI_SSID_2,
//         .password = CONFIG_ESP_WIFI_PASSWORD_2,
//         .threshold.authmode = WIFI_AUTH_WPA2_PSK,
//     };

//     wifi_sta_config_t wifi_point_3 = {
//         .ssid = CONFIG_ESP_WIFI_SSID_3,
//         .password = CONFIG_ESP_WIFI_PASSWORD_3,
//         .threshold.authmode = WIFI_AUTH_WPA2_PSK,
//     };

//     wifi_sta_config_t wifi_point_4 = {
//         .ssid = CONFIG_ESP_WIFI_SSID_4,
//         .password = CONFIG_ESP_WIFI_PASSWORD_4,
//         .threshold.authmode = WIFI_AUTH_WPA2_PSK,
//     };

//     wifi_points[0] = wifi_point_0;
//     wifi_points[1] = wifi_point_1;
//     wifi_points[2] = wifi_point_2;
//     wifi_points[3] = wifi_point_3;
//     wifi_points[4] = wifi_point_4;
// }


// static esp_err_t connect_wifi(void)
// {
//     esp_err_t err = ESP_FAIL;
//     s_wifi_event_group = xEventGroupCreate();

//     ESP_ERROR_CHECK(esp_netif_init());

//     ESP_ERROR_CHECK(esp_event_loop_create_default());   // тут помилка при підключенні до правильного вайфай
//     esp_netif_create_default_wifi_sta();

//     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     ESP_ERROR_CHECK(esp_wifi_init(&cfg));

//     esp_event_handler_instance_t instance_any_id;
//     esp_event_handler_instance_t instance_got_ip;
//     ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
//                                                         ESP_EVENT_ANY_ID,
//                                                         &event_handler,
//                                                         NULL,
//                                                         &instance_any_id)
//     );
//     ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
//                                                         IP_EVENT_STA_GOT_IP,
//                                                         &event_handler,
//                                                         NULL,
//                                                         &instance_got_ip)
//     );

//     wifi_sta_config_t wifi_points[CONFIG_ESP_WIFI_POINTS_NUMBER];
//     form_wifi_points(wifi_points);
//     ESP_LOGI(TAG, "formed_wifi_points");

//     EventBits_t event_bits;

//     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

//     ESP_LOGI(TAG, "made esp_wifi_set_mode");
//     ESP_LOGI(TAG, "bits: conn - %d failed - %d", WIFI_CONNECTED_BIT, WIFI_FAIL_BIT);

//     for (int i = 0, connected = 0; !connected && i < CONFIG_ESP_WIFI_POINTS_NUMBER; ++i)
//     {
//         wifi_config_t wifi_config = {
//             .sta = wifi_points[i]
//         };
//         ESP_LOGI(TAG, "try ssid: %s passwd: %s i=%d", wifi_points[i].ssid, wifi_points[i].password, i);
//         esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
//         esp_wifi_start();
//         ESP_LOGI(TAG, "started, waiting for bits");
//         event_bits = xEventGroupWaitBits(s_wifi_event_group,
//                                          WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
//                                          pdTRUE,
//                                          pdFALSE,
//                                          portMAX_DELAY);

//         if (event_bits & WIFI_CONNECTED_BIT)
//         {
//             ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
//                      wifi_points[i].ssid, wifi_points[i].password);
//             connected = 1;
//             err = ESP_OK;
//         }
//         else if (event_bits & WIFI_FAIL_BIT)
//         {
//             ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s i=%d",
//                      wifi_points[i].ssid, wifi_points[i].password, i);
//         }
//         else
//         {
//             ESP_LOGE(TAG, "UNEXPECTED EVENT IN WIFI CONNECTION, wifi_points i = %d", i);
//         }
//         // xEventGroupSetBits(s_wifi_event_group, event_bits);
//         // xEventGroupClearBits(event_bits, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT);
//     }
//     vEventGroupDelete(s_wifi_event_group);

//     return err;
// }


// static esp_err_t send_web_page(httpd_req_t *req) {
//     return httpd_resp_send(req, (const char *)rc_index_html, rc_index_html_len);
// }


// static esp_err_t get_req_handler(httpd_req_t *req)
// {
//     return send_web_page(req);
// }


// static esp_err_t jpg_handler(httpd_req_t *req) {
//     ESP_LOGI(TAG, "got jpg uri req");
//     camera_fb_t *pic = esp_camera_fb_get();
//     size_t jpg_buf_len = 0;
//     uint8_t *jpg_buf = NULL;

//     if (pic->format == PIXFORMAT_RGB565) {
//         bool converted = frame2jpg(pic, 80, &jpg_buf, &jpg_buf_len);
//         if (!converted) {
//             httpd_resp_send_500(req);
//             return ESP_FAIL;
//         }

//         // Set response content type to image/jpeg
//         httpd_resp_set_type(req, "image/jpeg");
//         // Send the JPEG buffer
//         httpd_resp_send(req, (const char *)jpg_buf, jpg_buf_len);
//         ESP_LOGI(TAG, " -- sent picture -- ");

//         // Free the buffer
//         free(jpg_buf);
//         esp_camera_fb_return(pic);
//         return ESP_OK;
//     }
//     httpd_resp_send_500(req);
//     return ESP_FAIL;
// }


// static httpd_uri_t uri_get = {
//     .uri = "/",
//     .method = HTTP_GET,
//     .handler = get_req_handler,
//     .user_ctx = NULL
// };


// static httpd_uri_t uri_camera = {
//     .uri = "/camera",
//     .method = HTTP_GET,
//     .handler = jpg_handler,
//     .user_ctx = NULL
// };


// static httpd_handle_t setup_server(void)
// {
//     httpd_config_t config = HTTPD_DEFAULT_CONFIG();
//     httpd_handle_t server = NULL;

//     if (httpd_start(&server, &config) == ESP_OK)
//     {
//         httpd_register_uri_handler(server, &uri_get);
//         httpd_register_uri_handler(server, &uri_camera);
//     }

//     return server;
// }

// #endif


// esp_err_t server_up(void) {
//         // Initialize NVS
//     esp_err_t err;
//     esp_err_t ret = nvs_flash_init();
//     if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
//     {
//         ESP_ERROR_CHECK(nvs_flash_erase());
//         ret = nvs_flash_init();
//     }
//     ESP_ERROR_CHECK(ret);

//     // ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
//     // err = connect_wifi();
//     // ESP_ERROR_CHECK(err);
//     ESP_ERROR_CHECK(connect_wifi());
//     if (!connect_wifi()) {
//         ESP_LOGE(TAG, "wifi not connected");
//         return ESP_FAIL;
//     }

//     // GPIO initialization
//     gpio_reset_pin(LED_PIN);
//     gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
//     ESP_LOGI(TAG, "LED Control Web Server is running ... ...\n");
//     setup_server();
//     ESP_LOGI(TAG, "\nserver is up, good boiii\n");

//     if (init_camera() != ESP_OK) {
//         return ESP_FAIL;
//     }

//     return err;
// }