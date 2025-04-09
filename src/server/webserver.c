#include <stdlib.h>
#include <esp_http_server.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "driver/gpio.h"
#include <stddef.h>
#include <string.h>


#include "defs.h"

#include "server/wifi.h"
#include "server/setting_mdns.h"
#include "server/webserver.h"
#include "server/index_html.h"

#include "img_processing/camera.h"
#include "img_processing/find_sun.h"
#include "img_processing/follow_obj_in_img.h"

#ifndef TAG
#define TAG "my_webserver"
#endif



static esp_err_t get_req_handler(httpd_req_t *req)
{
    return httpd_resp_send(req, (const char *)frontend_index_html, frontend_index_html_len);
}


static esp_err_t form_json(
    char *metadata,
    const uint16_t metadata_size,
    const max_brightness_pixels_t *sun_positions
) {
    snprintf(metadata, metadata_size, 
             "{\"count\":%zu,\"center\":{\"x\":%zu,\"y\":%zu},\"coords\":[", 
             sun_positions->count,
             sun_positions->center_coord.x,
             sun_positions->center_coord.y);
    char coord_buf[32];
    for (uint8_t i = 0; i < sun_positions->count; i++) {
        snprintf(coord_buf, sizeof(coord_buf), 
                 "{\"x\":%zu,\"y\":%zu}%s",
                 sun_positions->coords[i].x,
                 sun_positions->coords[i].y,
                 i < sun_positions->count - 1 ? "," : "");
        strcat(metadata, coord_buf);
    }
    strcat(metadata, "]}");
    return ESP_OK;
}


static esp_err_t req_img_handler(httpd_req_t *req)
{
    // ESP_LOGI(TAG, "got jpg uri req");
    camera_fb_t *frame = current_frame; // esp_camera_fb_get();
    if (!frame) {
        ESP_LOGE(TAG, "could't take frame");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    esp_err_t err = ESP_FAIL;
    if (frame->format == PIXFORMAT_RGB565) {
        max_brightness_pixels_t *sun_positions = mark_sun(frame);

        uint8_t *jpg_buf = NULL;
        size_t jpg_buf_len = 0;
        if (frame2jpg(frame, 80, &jpg_buf, &jpg_buf_len)) {
            const uint16_t metadata_size = 512;
            char metadata[metadata_size];
            form_json(metadata, metadata_size, sun_positions);

            httpd_resp_set_type(req, "image/jpeg");
            httpd_resp_set_hdr(req, "sun-coords", metadata);
            
            err = httpd_resp_send(req, (const char *)jpg_buf, jpg_buf_len);
            free(jpg_buf);
        } else {
            ESP_LOGE(TAG, "JPEG conversion failed");
            httpd_resp_send_500(req);
        }
        free(sun_positions->coords);
        free(sun_positions);
    } else {
        ESP_LOGE(TAG, "Unsupported format");
        httpd_resp_send_500(req);
    }

    if (err == ESP_OK)  
        ESP_LOGI(TAG, " -- sent picture and metadata -- ");

    // esp_camera_fb_return(frame);
    return err;
}


bool pause_photographer = 0;    // extern declared in img_processing/follow_object_on_img.h

static esp_err_t pause_handler(httpd_req_t *req)
{
    pause_photographer = 1;
    httpd_resp_send(req, "Paused", 6);
    return ESP_OK;
}

static esp_err_t rect_handler(httpd_req_t *req)
{
    char buf[128];
    int ret = httpd_req_recv(req, buf, sizeof(buf) - 1);
    if (ret <= 0) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    buf[ret] = '\0';

    uint8_t topLeftX, topLeftY, width, height;
    sscanf(buf, "{\"topLeftX\":%hhu,\"topLeftY\":%hhu,\"width\":%hhu,\"height\":%hhu}", 
           &topLeftX, &topLeftY, &width, &height);

    ESP_LOGI(TAG, "Rectangle coordinates: Top: %d Left: %d, width: %d, height: %d)", 
             topLeftX, topLeftY, width, height);

    httpd_resp_send(req, "OK", 2);
    pause_photographer = 0;
    return ESP_OK;
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
    .handler = req_img_handler,
    .user_ctx = NULL
};

static httpd_uri_t uri_pause = {
    .uri = "/pause",
    .method = HTTP_GET,
    .handler = pause_handler,
    .user_ctx = NULL
};

static httpd_uri_t uri_set_rect = {
    .uri = "/set-rect",
    .method = HTTP_POST,
    .handler = rect_handler,
    .user_ctx = NULL
};

static httpd_handle_t setup_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_camera);
        httpd_register_uri_handler(server, &uri_pause);
        httpd_register_uri_handler(server, &uri_set_rect);
    }

    return server;
}

esp_err_t server_up(void)
{
    esp_err_t err = ESP_FAIL;
    esp_err_t wifi_err = local_start_wifi();
    set_mdns();

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