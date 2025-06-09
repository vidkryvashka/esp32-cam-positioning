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


// #if DEFS_MARK_SUN == 1
#if ANALISIS_MODE == MODE_FIND_SUN
    #include "img_processing/find_sun.h"
#endif


#define TAG "my_webserver"


static uint16_t sending_period = PHOTOGRAPHER_DELAY_MS; // 1000;  // ms
static httpd_handle_t server = NULL;
#define MAX_WS_CLIENTS 3
static int ws_clients[MAX_WS_CLIENTS] = {-1, -1, -1};

static camera_fb_t *frame2send;

static keypoints_shell_t *keypoints_shell_local_ref;


static esp_err_t form_metadata_json(
    char *dest_json,
    const uint16_t metadata_size,
    const uint16_t frame_width,
    const uint16_t frame_height
) {
    esp_err_t err = ESP_OK;
    char format[] = "{\"type\":\"metadata\",\"img-size\":{\"width\":%d,\"height\":%d},\"center\":{\"x\":%d,\"y\":%d},\"count\":%zu,\"coords\":[";
    pixels_cloud_t *pixels_cloud = &keypoints_shell_local_ref->pixels_cloud;
    vector_t *coords = pixels_cloud->coords;
    snprintf(dest_json, metadata_size,
            format,
            frame_width, frame_height,
            pixels_cloud->center_coord.x,
            pixels_cloud->center_coord.y,
            coords != NULL ? coords->size : 0
    );
    if (coords) {
        const uint8_t coords_buf_size = 24;
        char coords_buf[coords_buf_size];
        for (size_t i = 0; i < coords->size; i++) {
            pixel_coord_t *coord = vector_get(coords, i);
            snprintf(coords_buf, coords_buf_size, 
                    "{\"x\":%zu,\"y\":%zu}%s",
                    coord->x,
                    coord->y,
                    i < coords->size - 1 ? "," : "");
            strncat(dest_json, coords_buf, coords_buf_size);
            bzero(coords_buf, coords_buf_size);
        }
    } else {
        ESP_LOGE(TAG, "form_metadata_json no coords ");
        err = ESP_FAIL;
    }
    strcat(dest_json, "]}\0");

    return err;
}


static uint8_t ws_send_pkt(
    httpd_ws_frame_t *ws_pkt
) {
    uint8_t num_sent = 0;
    for (int i = 0; i < MAX_WS_CLIENTS; i++) {
        if (ws_clients[i] != -1) {
            esp_err_t ret = httpd_ws_send_frame_async(server, ws_clients[i], ws_pkt);
            if (ret != ESP_OK) {
                ESP_LOGW(TAG, "Client №%d %d disconnected", i, ws_clients[i]);
                ws_clients[i] = -1;
            } else
                num_sent ++;
        }
    }
    return num_sent;
}


static int8_t send_meta(
    void
) {
    if (!frame2send)
        return 0;

    uint16_t frame_width = frame2send->width, frame_height = frame2send->height;
    const uint16_t metadata_size = /*strlen(json_begin_format)*/102 + 29 * sizeof(char) * keypoints_shell_local_ref->pixels_cloud.coords->size + 16;
    ESP_LOGI(TAG, "metadata size %d ", metadata_size);
    char metadata_json[metadata_size];
    form_metadata_json(metadata_json, metadata_size, frame_width, frame_height);
    httpd_ws_frame_t ws_metadata_pkt = {
        .final = true,
        .fragmented = false,
        .type = HTTPD_WS_TYPE_TEXT,
        .payload = (uint8_t *)metadata_json,
        .len = strlen(metadata_json)
    };
    ESP_LOGI(TAG, "sending metadata ");

    return ws_send_pkt(&ws_metadata_pkt);
}


static int8_t send_img(
    uint8_t *jpg_buf,   // discards const
    const size_t jpg_buf_len
) {
    ESP_LOGI(TAG, "jpg_buf_len = %d ", jpg_buf_len);
        
    httpd_ws_frame_t ws_img_pkt = {
        .final = true,
        .fragmented = false,
        .type = HTTPD_WS_TYPE_BINARY,
        .payload = jpg_buf,
        .len = jpg_buf_len
    };

    return ws_send_pkt(&ws_img_pkt);
}


static void send_frames_task(
    void *pvParameters
) {
    while (true) {

        if (ws_clients[0] == -1 && ws_clients[1] == -1 && ws_clients[2] == -1) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        if (xSemaphoreTake(frame_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
            ESP_LOGE(TAG, "send_frames_task failed to take frame_mutex");
            vTaskDelay(pdMS_TO_TICKS(100));
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
        bool frame_mutex_given = 0;
        
        int8_t num_meta_sent = send_meta();

        if (frame2send && frame2jpg(frame2send, 12, &jpg_buf, &jpg_buf_len)) {
            int8_t num_img_sent = send_img(jpg_buf, jpg_buf_len);

            xSemaphoreGive(frame_mutex);
            frame_mutex_given = 1;
            
            // ESP_LOGI(TAG, "free jpeg_buff ");
            if (jpg_buf_len < 128*1024) {       // firstly 128*1024 given and shrinked in frame2jpg
                free(jpg_buf);                 // sometimes frame2jpg determines jpg_buf_len fucking huge and writes jpg_buf idk where.
            } else {
                ESP_LOGE(TAG, "Invalid jpg_buf_len = %d, skipping free! ", jpg_buf_len);
                log_memory(xPortGetCoreID());
            }
            ESP_LOGI(TAG, "ws sent jsons %d\timgs %d ", num_meta_sent, num_img_sent);
        } else {
            ESP_LOGE(TAG, "send_frames_task JPEG conversion failed");
        }
        if (!frame_mutex_given) {
            xSemaphoreGive(frame_mutex);
            frame_mutex_given = 1;
        }
        vTaskDelay(pdMS_TO_TICKS(sending_period));
        frame2send = current_frame;

        while (pause_photographer)
            vTaskDelay(pdMS_TO_TICKS(200));
    }
}


static esp_err_t ws_handler(httpd_req_t *req)
{
    if (req->method == HTTP_GET) {
        int sockfd = httpd_req_to_sockfd(req);
        for (int i = 0; i < MAX_WS_CLIENTS; i++) {
            if (ws_clients[i] == -1) {
                ws_clients[i] = sockfd;
                ESP_LOGI(TAG, "New WS client №%d: %d", i, sockfd);
                break;
            }
        }
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


// esp_err_t rand_angles_send();
// // to test servos from web page
// void servo_actions()
// {
//     // uint8_t pan_angle = my_servo_get_angle(SERVO_PAN_CH);
//     // uint8_t tilt_angle = my_servo_get_angle(SERVO_TILT_CH);
//     // ESP_LOGI(TAG, "servos angles pan: %d tilt: %d ", pan_angle, tilt_angle);
// 
//     // angles_diff_t angles_diff = {-4, 5};
//     // if (xQueueSend(servo_queue, (void *)&angles_diff, 10) != pdTRUE) {
//     //     ESP_LOGE(TAG, "servo_actions couldn't xQueueSend, queue fill ");
//     // }
// 
//     // rand_angles_send();
// 
//     ESP_LOGI(TAG, "servo_actions end ");
// }

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
        frame2send = operate_fragment(&rect_coords);
    xSemaphoreGive(frame_mutex);
    pause_photographer = 0;

    log_memory(xPortGetCoreID());
    // servo_actions();

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
    xTaskCreatePinnedToCore(send_frames_task, "send_frames_task", 4096, NULL, 5, NULL, 1);

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