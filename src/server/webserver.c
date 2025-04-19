#include <stdlib.h>
#include <esp_http_server.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <esp_netif.h>
#include <driver/gpio.h>
#include <string.h>
#include <sys/time.h>

#include "defs.h"
#include "server/wifi.h"
#include "server/setting_mdns.h"
#include "server/webserver.h"
#include "server/index_html.h"
#include "img_processing/camera.h"
#include "img_processing/photographer.h"

#if DEFS_MARK_SUN == 1
    #include "img_processing/find_sun.h"
#endif

#ifndef TAG
    #define TAG "my_webserver"
#endif

static uint64_t sending_period = PHOTOGRAPHER_DELAY_MS; // 1000;  // ms
static httpd_handle_t server = NULL;
static volatile int ws_sockfd = -1;


#if DEFS_MARK_SUN == 1
static esp_err_t form_sun_json(char *dest_json, const uint16_t metadata_size, const camera_fb_t *frame) {
    max_brightness_pixels_t *sun_positions = mark_sun(frame);
    snprintf(dest_json, metadata_size, 
             "{\"type\":\"sun_coords\",\"count\":%zu,\"center\":{\"x\":%zu,\"y\":%zu},\"coords\":[", 
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


static void send_image_task(void *arg) {
    while (true) {
        if (ws_sockfd == -1) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        if (xSemaphoreTake(frame_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
            ESP_LOGE(TAG, "Failed to take frame_mutex");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        camera_fb_t *frame = current_frame;
        if (!frame) {
            ESP_LOGE(TAG, "Could not access frame");
            xSemaphoreGive(frame_mutex);
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        uint8_t *jpg_buf = NULL;
        size_t jpg_buf_len = 0;
        if (frame->format == PIXFORMAT_RGB565 && frame2jpg(frame, 80, &jpg_buf, &jpg_buf_len)) {
#if DEFS_MARK_SUN == 1
            const uint16_t metadata_size = 512;
            char metadata_json[metadata_size];
            form_sun_json(metadata_json, metadata_size, frame);
            httpd_ws_frame_t ws_sun_coords_pkt = {
                .final = true,
                .fragmented = false,
                .type = HTTPD_WS_TYPE_TEXT,
                .payload = (uint8_t *)metadata_json,
                .len = strlen(metadata_json)
            };
            httpd_ws_send_frame_async(server, ws_sockfd, &ws_sun_coords_pkt);
#endif
            
            httpd_ws_frame_t ws_img_pkt = {
                .final = true,
                .fragmented = false,
                .type = HTTPD_WS_TYPE_BINARY,
                .payload = jpg_buf,
                .len = jpg_buf_len
            };
            esp_err_t ret = httpd_ws_send_frame_async(server, ws_sockfd, &ws_img_pkt);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Failed to send WebSocket frame: %d", ret);
                ws_sockfd = -1; // Client disconnected
            } else {
                ESP_LOGI(TAG, " -- sent image ");
            }
            free(jpg_buf);
        } else {
            ESP_LOGE(TAG, "JPEG conversion failed");
        }
        xSemaphoreGive(frame_mutex);
        vTaskDelay(pdMS_TO_TICKS(sending_period));

        while (pause_photographer)
            vTaskDelay(pdMS_TO_TICKS(500));
    }
}


static esp_err_t ws_handler(httpd_req_t *req) {
    if (req->method == HTTP_GET) {
        ESP_LOGI(TAG, "WebSocket handshake");
        ws_sockfd = httpd_req_to_sockfd(req);
        return ESP_OK;
    }
    return ESP_OK;
}


static esp_err_t get_req_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, " -- sent page -- ");
    return httpd_resp_send(req, (const char *)frontend_index_html, frontend_index_html_len);
}


static esp_err_t pause_handler(httpd_req_t *req) {
    pause_photographer = 1;
    httpd_resp_send(req, "Paused", 6);
    return ESP_OK;
}


static esp_err_t rect_handler(httpd_req_t *req) {
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
    ) >= 0) {
        httpd_resp_send(req, "Got rectangle coords", 20);
    }

    // ESP_LOGI(TAG, "Rectangle coordinates: Top: %d Left: %d, width: %d, height: %d)", 
    //          top_left.x, top_left.y, width, height);

    if (xSemaphoreTake(frame_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to take frame_mutex on fragment");
        return ESP_FAIL;
    }

    esp_camera_fb_return(current_frame);
    current_frame = write_fragment(rect_coords);
    xSemaphoreGive(frame_mutex);

    return ESP_OK;
}


static httpd_uri_t uri_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = get_req_handler,
    .user_ctx = NULL
};

static httpd_uri_t uri_ws = {
    .uri = "/ws",
    .method = HTTP_GET,
    .handler = ws_handler,
    .user_ctx = NULL,
    .is_websocket = true
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

static esp_err_t setup_server(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    // config.stack_size = 8192; // bigger for WebSocket

    esp_err_t server_err = httpd_start(&server, &config);

    httpd_register_uri_handler(server, &uri_get);
    httpd_register_uri_handler(server, &uri_ws);
    httpd_register_uri_handler(server, &uri_pause);
    httpd_register_uri_handler(server, &uri_set_rect);
    xTaskCreate(send_image_task, "send_image_task", 4096, NULL, 5, NULL);

    return server_err;
}

esp_err_t server_up(void) {
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