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
#if DEFS_MARK_SUN == 1
    #include "img_processing/find_sun.h"
#endif
#include "img_processing/photographer.h"

#ifndef TAG
#define TAG "my_webserver"
#endif



static esp_err_t get_req_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, " -- sent page -- "); // almost
    return httpd_resp_send(req, (const char *)frontend_index_html, frontend_index_html_len);
}

#if DEFS_MARK_SUN == 1
static esp_err_t form_sun_json(
    char *dest_json,
    const uint16_t metadata_size,
    const camera_fb_t *frame
) {
    max_brightness_pixels_t *sun_positions = mark_sun(frame);
    snprintf(dest_json, metadata_size, 
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
        strcat(dest_json, coord_buf);
    }
    strcat(dest_json, "]}");
    free(sun_positions->coords);
    free(sun_positions);
    return ESP_OK;
}
#endif


static esp_err_t req_img_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "got img req");
    if (xSemaphoreTake(frame_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to take frame_mutex");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    camera_fb_t *frame = current_frame; // esp_camera_fb_get();
    if (!frame) {
        ESP_LOGE(TAG, "could't access frame");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    bool is_sun_hdr = 0;
#if DEFS_MARK_SUN == 1
    const uint16_t metadata_size = 512;
    char metadata_json[metadata_size];
    form_sun_json(metadata_json, metadata_size, frame);
#endif

    esp_err_t resp_err = ESP_FAIL;
    if (frame->format == PIXFORMAT_RGB565) {
        uint8_t *jpg_buf = NULL;
        size_t jpg_buf_len = 0;
        if (frame2jpg(frame, 80, &jpg_buf, &jpg_buf_len)) {
            httpd_resp_set_type(req, "image/jpeg");
#if DEFS_MARK_SUN == 1
            is_sun_hdr = ! httpd_resp_set_hdr(req, "sun-coords", metadata_json);
#endif
            resp_err = httpd_resp_send(req, (const char *)jpg_buf, jpg_buf_len);
            free(jpg_buf);
        } else {
            ESP_LOGE(TAG, "JPEG conversion failed");
            httpd_resp_send_500(req);
        }
    } else {
        ESP_LOGE(TAG, "Unsupported format");
        httpd_resp_send_500(req);
    }
    xSemaphoreGive(frame_mutex);

    if (resp_err == ESP_OK)
        ESP_LOGI(TAG, " -- sent picture %s-- ",
            is_sun_hdr ? "and sun metadata " : "");

    // esp_camera_fb_return(frame);
    return resp_err;
}


// idk why, must be inited here to use on both sides, unlike frame
// volatile bool pause_photographer = 0;    // extern declared in img_processing/photographer.h

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

    rectangle_coords_t rect_coords;
    if (sscanf(buf, "{\"topLeftX\":%hhu,\"topLeftY\":%hhu,\"width\":%hhu,\"height\":%hhu}", 
           &rect_coords.top_left.x, &rect_coords.top_left.y, &rect_coords.width, &rect_coords.height
    ) >= 0)
        httpd_resp_send(req, "Got rectangle coords", 20);


    // ESP_LOGI(TAG, "Rectangle coordinates: Top: %d Left: %d, width: %d, height: %d)", 
    //          top_left.x, top_left.y, width, height);


    if (xSemaphoreTake(frame_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to take frame_mutex on fragment");
    } else {
        esp_camera_fb_return(current_frame);
        current_frame = write_fragment(rect_coords);
        // pause_photographer = 0;
        xSemaphoreGive(frame_mutex);
    }
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
    // config.stack_size = 4096; // by default, to know
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