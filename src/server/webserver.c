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
#include "esp_attr.h"

#include "my_servos.h"

#include "esp_attr.h"


#if ANALISIS_MODE == MODE_FIND_SUN
    #include "img_processing/find_sun.h"
#endif

#define TAG "my_webserver"


static uint16_t sending_period = PHOTOGRAPHER_DELAY_MS; // ms
static httpd_handle_t server = NULL;
#define MAX_WS_CLIENTS 3
static int ws_camera_clients[MAX_WS_CLIENTS] = {-1, -1, -1};
static int ws_servo_clients[MAX_WS_CLIENTS] = {-1, -1, -1};

static keypoints_shell_t *keypoints_shell_local_ref;

static esp_err_t form_metadata_json(
    char *dest_json,
    const uint16_t metadata_json_len,
    const uint16_t frame_width,
    const uint16_t frame_height
) {
    esp_err_t err = ESP_OK;
    char format[] = "{\"type\":\"metadata\",\"img-size\":{\"width\":%d,\"height\":%d},\"center\":{\"x\":%d,\"y\":%d},\"count\":%zu,\"coords\":[";
    pixels_cloud_t *pixels_cloud = &keypoints_shell_local_ref->pixels_cloud;
    vector_t *coords = pixels_cloud->coords;
    snprintf(dest_json, metadata_json_len,
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
    httpd_ws_frame_t *ws_pkt,
    int *clients
) {
    uint8_t num_sent = 0;
    for (int i = 0; i < MAX_WS_CLIENTS; i++) {
        if (clients[i] != -1) {
            esp_err_t ret = httpd_ws_send_frame_async(server, clients[i], ws_pkt);
            if (ret != ESP_OK) {
                ESP_LOGW(TAG, "Client №%d %d disconnected", i, clients[i]);
                clients[i] = -1;
            } else {
                num_sent ++;
            }
        }
    }
    return num_sent;
}


static int8_t send_meta(
    void
) {
    if (!current_frame)
        return 0;

    uint16_t frame_width = current_frame->width, frame_height = current_frame->height;
    const uint16_t metadata_json_len =
            /*strlen(json_begin_format)*/ (102) +
            /*coords_arr*/ (30 * sizeof(char) * keypoints_shell_local_ref->pixels_cloud.coords->size) +
            /*aware boundries*/ 16;
    ESP_LOGI(TAG, "metadata size %d ", metadata_json_len);
    // char metadata_json[metadata_json_len];
    char *metadata_json = heap_caps_malloc(metadata_json_len * sizeof(char) * metadata_json_len, MALLOC_CAP_SPIRAM);
    form_metadata_json(metadata_json, metadata_json_len, frame_width, frame_height);
    httpd_ws_frame_t ws_metadata_pkt = {
        .final = true,
        .fragmented = false,
        .type = HTTPD_WS_TYPE_TEXT,
        .payload = (uint8_t *)metadata_json,
        .len = strlen(metadata_json)
    };
    ESP_LOGI(TAG, "sending metadata ");

    uint8_t num_sent = ws_send_pkt(&ws_metadata_pkt, ws_camera_clients);

    if (metadata_json)
        heap_caps_free(metadata_json);
    return num_sent;
}


static int8_t send_img(
    uint8_t *jpg_buf,
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

    return ws_send_pkt(&ws_img_pkt, ws_camera_clients);
}


static void send_frames_task(
    void *pvParameters
) {
    while (true) {
        if (ws_camera_clients[0] == -1 && ws_camera_clients[1] == -1 && ws_camera_clients[2] == -1) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        if (xSemaphoreTake(frame_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
            ESP_LOGE(TAG, "send_frames_task failed to take frame_mutex");
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        if (!current_frame) {
            ESP_LOGE(TAG, "Could not access current_frame ");
            xSemaphoreGive(frame_mutex);
            vTaskDelay(pdMS_TO_TICKS(PHOTOGRAPHER_DELAY_MS * 3));
            continue;
        }

        uint8_t *jpg_buf = NULL;
        size_t jpg_buf_len = 0;
        bool frame_mutex_given = 0;
        
        int8_t num_meta_sent = send_meta();

        if (current_frame && frame2jpg(current_frame, 12, &jpg_buf, &jpg_buf_len)) {
            if (jpg_buf_len >= 128*1024) {
                ESP_LOGE(TAG, "\n\t--- send_frames_task frame2jpg corrupted memory \t abort \n ");
                abort();
            }
            int8_t num_img_sent = send_img(jpg_buf, jpg_buf_len);

            xSemaphoreGive(frame_mutex);
            frame_mutex_given = 1;
            
            free(jpg_buf);
            ESP_LOGI(TAG, "ws sent jsons %d\timgs %d ", num_meta_sent, num_img_sent);
        } else {
            ESP_LOGE(TAG, "send_frames_task JPEG conversion failed");
        }
        if (!frame_mutex_given) {
            xSemaphoreGive(frame_mutex);
            frame_mutex_given = 1;
        }
        vTaskDelay(pdMS_TO_TICKS(sending_period));
    }
}


static esp_err_t ws_camera_handler(
    httpd_req_t *req
) {
    if (req->method == HTTP_GET) {
        int sockfd = httpd_req_to_sockfd(req);
        for (int i = 0; i < MAX_WS_CLIENTS; i++) {
            if (ws_camera_clients[i] == -1) {
                ws_camera_clients[i] = sockfd;
                ESP_LOGI(TAG, "New Camera WS client №%d: %d", i, sockfd);
                break;
            }
        }
    }
    return ESP_OK;
}


static esp_err_t ws_servo_handler(
    httpd_req_t *req
) {
    if (req->method == HTTP_GET) {
        int sockfd = httpd_req_to_sockfd(req);
        for (int i = 0; i < MAX_WS_CLIENTS; i++) {
            if (ws_servo_clients[i] == -1) {
                ws_servo_clients[i] = sockfd;
                ESP_LOGI(TAG, "New Servo WS client №%d: %d", i, sockfd);
                break;
            }
        }
    } else {
        httpd_ws_frame_t ws_pkt;
        uint8_t buf[32] = {0};
        memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
        ws_pkt.payload = buf;
        ws_pkt.type = HTTPD_WS_TYPE_TEXT;
        esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 32);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "httpd_ws_recv_frame failed: %d", ret);
            return ret;
        }
        if (ws_pkt.len > 0) {
            buf[ws_pkt.len] = '\0';
            char *key = strtok((char *)buf, ",");
            char *value_str = strtok(NULL, ",");
            if (key && value_str) {
                int value = atoi(value_str);
                ESP_LOGI(TAG, "Received %s: %d", key, value);
                angles_diff_t angles_diff = {0,0};
                if (strcmp(key, "Pan") == 0) {
                    angles_diff.pan = value;
                } else if (strcmp(key, "Tilt") == 0) {
                    angles_diff.tilt = value;
                }

                if (are_servos_inited && servo_queue)
                    if (xQueueSend(servo_queue, (void *)&angles_diff, 10) != pdTRUE)
                        ESP_LOGE(TAG, "\t\t--- servo_actions couldn't xQueueSend, queue fill ");
            }
        }
    }
    return ESP_OK;
}


static esp_err_t get_req_handler(
    httpd_req_t *req
) {
    ESP_LOGI(TAG, " -- sent page -- ");
    return httpd_resp_send(req, (const char *)frontend_index_html, frontend_index_html_len);
}


static httpd_uri_t uri_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = get_req_handler,
    .user_ctx = NULL
};

static httpd_uri_t uri_ws_camera = {
    .uri = "/ws",
    .method = HTTP_GET,
    .handler = ws_camera_handler,
    .user_ctx = NULL,
    .is_websocket = true
};

static httpd_uri_t uri_ws_servo = {
    .uri = "/ServoInput",
    .method = HTTP_GET,
    .handler = ws_servo_handler,
    .user_ctx = NULL,
    .is_websocket = true
};


static esp_err_t setup_server(
    void
) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.core_id = 1;
    config.stack_size = 12288; // 8192;
    keypoints_shell_local_ref = get_keypoints_shell_reference();

    esp_err_t server_err = httpd_start(&server, &config);

    httpd_register_uri_handler(server, &uri_get);
    httpd_register_uri_handler(server, &uri_ws_camera);
    httpd_register_uri_handler(server, &uri_ws_servo);
    xTaskCreatePinnedToCore(send_frames_task,
                            "send_frames_task",
                            8096,
                            NULL, 5,
                            NULL,
                            1);

    return server_err;
}


esp_err_t server_up(
    void
)
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