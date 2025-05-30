#include <stdlib.h>
#include <string.h>
#include <esp_log.h>
#include <sys/time.h>
#include <nvs_flash.h>
#include <esp_netif.h>
#include <driver/gpio.h>
#include <esp_http_server.h>

#include "defs.h"
#include "my_vector.h"
#include "server/wifi.h"
#include "server/setting_mdns.h"
#include "server/webserver.h"
#include "server/index_html.h"
#include "img_processing/camera.h"
#include "img_processing/photographer.h"

#include "my_servos.h"


#if DEFS_MARK_SUN == 1
    #include "img_processing/find_sun.h"
#endif

#define TAG "my_webserver"\

static uint16_t sending_period = PHOTOGRAPHER_DELAY_MS; // 1000;  // ms
static httpd_handle_t server = NULL;
static volatile int ws_sockfd = -1;

static camera_fb_t *frame2send;

static keypoints_shell_t *keypoints_shell_local_ref;


static esp_err_t form_metadata_json(
    char *dest_json,
    const uint16_t metadata_size,
    const camera_fb_t *frame
) {
    // vector_t *coords = NULL;
    char format[] = "{\"type\":\"metadata\",\"img-size\":{\"width\":%d,\"height\":%d},\"center\":{\"x\":%d,\"y\":%d},\"count\":%zu,\"coords\":[";
#if DEFS_MARK_SUN == 1
    max_brightness_pixels_t *sun_positions = mark_sun(frame);   // defer free_mbp(sun_positions);
    const vector_t *coords = sun_positions->coords;
    snprintf(dest_json, metadata_size,
            format,
            frame->width, frame->height,
            sun_positions->center_coord.x,
            sun_positions->center_coord.y,
            coords->size
    );
#else
    const vector_t *coords = keypoints_shell_local_ref->keypoints;
    snprintf(dest_json, metadata_size,
            format,
            frame->width, frame->height,
            -1, -1,
            coords != NULL ? coords->size : 0
    );
#endif
    if (coords) {
        char coords_buf[2 << 8];
        // vector_print(coords);
        ESP_LOGI(TAG, "coords n: %d ", coords->size);
        for (size_t i = 0; i < coords->size; ++i) {
            pixel_coord_t *coord = vector_get(coords, i);
            snprintf(coords_buf, sizeof(coords_buf), 
                    "{\"x\":%zu,\"y\":%zu}%s",
                    coord->x,
                    coord->y,
                    i < coords->size - 1 ? "," : "");
            strcat(dest_json, coords_buf);
        }
    } else
        ESP_LOGE(TAG, "form_metadata_json no coords ");
    strcat(dest_json, "]}");
#if DEFS_MARK_SUN == 1
    free_mbp(sun_positions);
#else
    ;
#endif

    ESP_LOGI(TAG, "in the end of form meta ");
    return ESP_OK;
}


static void send_image_task(
    void *pvParameters
) {
    while (true) {
        if (ws_sockfd == -1) {
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        if (xSemaphoreTake(frame_mutex, pdMS_TO_TICKS(PHOTOGRAPHER_DELAY_MS * 2)) != pdTRUE) {
            ESP_LOGE(TAG, "send_image_task failed to take frame_mutex");
            vTaskDelay(pdMS_TO_TICKS(PHOTOGRAPHER_DELAY_MS * 2));
            continue;
        }

        if (!frame2send) {
            ESP_LOGE(TAG, "Could not access frame2send ");
            xSemaphoreGive(frame_mutex);
            vTaskDelay(pdMS_TO_TICKS(PHOTOGRAPHER_DELAY_MS * 3));
            continue;
        }

        uint8_t *jpg_buf = NULL;
        size_t jpg_buf_len = 0;
        if (frame2jpg(frame2send, 80, &jpg_buf, &jpg_buf_len)) {
            const uint16_t metadata_size = 2 << 9;
            char metadata_json[metadata_size];
            form_metadata_json(metadata_json, metadata_size, frame2send);
            ESP_LOGI(TAG, "formed metadata_json ");
            httpd_ws_frame_t ws_metadata_pkt = {
                .final = true,
                .fragmented = false,
                .type = HTTPD_WS_TYPE_TEXT,
                .payload = (uint8_t *)metadata_json,
                .len = strlen(metadata_json)
            };
            ESP_LOGI(TAG, "sent metadata ");

            esp_err_t ret_err = httpd_ws_send_frame_async(server, ws_sockfd, &ws_metadata_pkt);
            if (ret_err != ESP_OK)
                ESP_LOGE(TAG, "Failed to send WebSocket metadata: %d", ret_err);

            httpd_ws_frame_t ws_img_pkt = {
                .final = true,
                .fragmented = false,
                .type = HTTPD_WS_TYPE_BINARY,
                .payload = jpg_buf,
                .len = jpg_buf_len
            };
            ret_err = httpd_ws_send_frame_async(server, ws_sockfd, &ws_img_pkt);
            if (ret_err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to send WebSocket frame2send: %d", ret_err);
                ws_sockfd = -1; // Client disconnected
            } else {
                ESP_LOGI(TAG, " -- sent pkts ");
            }
            free(jpg_buf);
        } else {
            ESP_LOGE(TAG, "JPEG conversion failed");
        }
        xSemaphoreGive(frame_mutex);
        frame2send = current_frame;
        vTaskDelay(pdMS_TO_TICKS(sending_period));

        while (pause_photographer)
            vTaskDelay(pdMS_TO_TICKS(500));
    }
}


static esp_err_t ws_handler(httpd_req_t *req)
{
    if (req->method == HTTP_GET) {
        ESP_LOGI(TAG, "WebSocket handshake");
        ws_sockfd = httpd_req_to_sockfd(req);
        return ESP_OK;
    }
    return ESP_OK;
}


static esp_err_t get_req_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, " -- sent page -- ");
    return httpd_resp_send(req, (const char *)frontend_index_html, frontend_index_html_len);
}


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
    if (sscanf(buf, "{\"topLeftX\":%hu,\"topLeftY\":%hu,\"width\":%hu,\"height\":%hu}",
           &rect_coords.top_left.x, &rect_coords.top_left.y, &rect_coords.width, &rect_coords.height
    ) >= 0) {
        char resp[] = "Got rectangle coords";
        httpd_resp_send(req, resp, strlen(resp));
    }

    if (xSemaphoreTake(frame_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to take frame_mutex on fragment");
        return ESP_FAIL;
    } else
        frame2send = decorate_fragment(&rect_coords);
    xSemaphoreGive(frame_mutex);

    // servo_actions();
    log_memory(xPortGetCoreID());

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
    config.core_id = 1;
    config.stack_size = 2 << 13; // bigger for WebSocket

    frame2send = current_frame;
    keypoints_shell_local_ref = get_keypoints_shell_reference();

    esp_err_t server_err = httpd_start(&server, &config);

    httpd_register_uri_handler(server, &uri_get);
    httpd_register_uri_handler(server, &uri_ws);
    httpd_register_uri_handler(server, &uri_pause);
    httpd_register_uri_handler(server, &uri_set_rect);
    xTaskCreatePinnedToCore(send_image_task, "send_image_task", 4096, NULL, 5, NULL, 1);

    return server_err;
}

esp_err_t server_up()
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