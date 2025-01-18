#include "wifi.h"
#include "camera.h"
#include "webserver.h"
#include "index_html.h"

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
    esp_err_t err = ESP_FAIL;
    ESP_LOGI(TAG, "got jpg uri req");
    camera_fb_t *pic = esp_camera_fb_get();

    if (pic->format == PIXFORMAT_RGB565) {
        uint8_t *jpg_buf = NULL;
        size_t jpg_buf_len = 0;
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
        err = ESP_OK;
    }
    esp_camera_fb_return(pic);
    httpd_resp_send_500(req);
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