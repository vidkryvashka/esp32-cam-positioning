#include <stdlib.h>
#include <string.h> //Requires by memset
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "spi_flash_mmap.h" // #include "esp_spi_flash.h" // #include "esp_system.h" replacement chain, deprecated
#include <esp_http_server.h>

// #include "esp_wifi.h"
// #include "esp_event.h"
// #include "freertos/event_groups.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "driver/gpio.h"
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/api.h>
#include <lwip/netdb.h>

#include "defs.h"

#include "server/wifi.h"
#include "server/webserver.h"
#include "server/index_html.h"
#include "camera.h"
#include "find_sun.h"

#ifndef TAG
#define TAG "esp_webserver"
#endif

static esp_err_t send_web_page(httpd_req_t *req)
{
    return httpd_resp_send(req, (const char *)rc_index_html, rc_index_html_len);
}


static esp_err_t get_req_handler(httpd_req_t *req)
{
    return send_web_page(req);
}


static esp_err_t jpg_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "got jpg uri req");

    camera_fb_t *frame = esp_camera_fb_get();
    if (!frame) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    esp_err_t err = ESP_FAIL;
    if (frame->format == PIXFORMAT_RGB565) {

        esp_err_t err_marked_sun = mark_sun(frame);

        uint8_t *jpg_buf = NULL;
        size_t jpg_buf_len = 0;
        if (frame2jpg(frame, 80, &jpg_buf, &jpg_buf_len)) {
            httpd_resp_set_type(req, "image/jpeg");
            err = httpd_resp_send(req, (const char *)jpg_buf, jpg_buf_len);
            // ESP_LOGI(TAG, "Free heap: %lu , min_free: %lu bytes", esp_get_free_heap_size(), esp_get_minimum_free_heap_size());
            free(jpg_buf);
        } else {
            ESP_LOGE(TAG, "JPEG conversion failed");
            httpd_resp_send_500(req);
        }
    } else {
        ESP_LOGE(TAG, "Unsupported format");
        httpd_resp_send_500(req);
    }

    if (err == ESP_OK)  
        ESP_LOGI(TAG, " -- sent picture -- ");

    esp_camera_fb_return(frame);
    return err;
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