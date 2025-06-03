#include "freertos/FreeRTOS.h"
#include "driver/ledc.h"
#include "esp_log.h"

#include "my_servos.h"

#define TAG "MyServos"

#define SERVO_PAN_GPIO  GPIO_NUM_14
#define SERVO_TILT_GPIO GPIO_NUM_15
#define SERVO_FREQ_HZ   50
#define DUTY_RESOLUTION LEDC_TIMER_12_BIT

static bool is_initialized = false;


static uint32_t angle2duty(
    uint8_t angle
) {
    if (angle > 180)
        angle = 180;
    
    // Map angle (0° -> 0.5ms, 180° -> 2.5ms)
    double pulse_width = 0.5 + ((double)angle / 180.0) * 2.0;
    // Convert pulse width to duty cycle (12-bit resolution, 50Hz)
    uint32_t duty = pulse_width / 20.0 * (1 << LEDC_TIMER_12_BIT);

    return duty;
}

static uint8_t duty2angle(
    uint32_t duty
) {
    if (duty > 1 << LEDC_TIMER_12_BIT) {
        ESP_LOGE(TAG, "\t--- duty2angle got duty %lu so it's %d", duty, (1 << LEDC_TIMER_12_BIT));
        duty = 1 << LEDC_TIMER_12_BIT;
    }

    double pulse_width = duty / (1 << LEDC_TIMER_12_BIT) * 20.0;
    uint8_t angle = (pulse_width - 0.5) / 2.0  * 180.0;

    if (angle > 180) {
        ESP_LOGE(TAG, "duty2angle made angle %d ", angle);
        angle = 180;
    }

    return angle;
}


esp_err_t init_my_servos()
{
    esp_err_t ret = ESP_OK;

    if (is_initialized) {
        ESP_LOGW(TAG, "Servos already initialized");
        return ESP_OK;
    }

    // Configure PWM timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_1,
        .duty_resolution = DUTY_RESOLUTION,
        .freq_hz = SERVO_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ret = ledc_timer_config(&ledc_timer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure LEDC timer: %s ", esp_err_to_name(ret));
        return ret;
    }

    // Configure PWM channel for pan servo (channel 0, GPIO 14)
    ledc_channel_config_t pan_channel = {
        .channel = SERVO_PAN_CH,
        .duty = angle2duty(0),
        .gpio_num = SERVO_PAN_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_1
    };
    ret = ledc_channel_config(&pan_channel);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure pan channel: %s ", esp_err_to_name(ret));
        return ret;
    }

    // Configure PWM channel for tilt servo (channel 1, GPIO 15)
    ledc_channel_config_t tilt_channel = {
        .channel = SERVO_TILT_CH,
        .duty = angle2duty(0),
        .gpio_num = SERVO_TILT_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_1
    };
    ret = ledc_channel_config(&tilt_channel);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure tilt channel: %s ", esp_err_to_name(ret));
        return ret;
    }
    ledc_fade_func_install(0);

    is_initialized = true;
    ESP_LOGI(TAG, "Servos initialized successfully ");
    return ESP_OK;
}


esp_err_t deinit_my_servos()
{
    if (!is_initialized) {
        ESP_LOGW(TAG, "Servos not initialized ");
        return ESP_OK;
    }

    esp_err_t ret = ESP_OK;

    // Stop PWM signals
    ret = ledc_stop(LEDC_LOW_SPEED_MODE, SERVO_PAN_CH, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to stop pan channel: %s ", esp_err_to_name(ret));
        return ret;
    }

    ret = ledc_stop(LEDC_LOW_SPEED_MODE, SERVO_TILT_CH, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to stop tilt channel: %s ", esp_err_to_name(ret));
        return ret;
    }

    is_initialized = false;
    ESP_LOGI(TAG, "Servos deinitialized successfully ");
    return ESP_OK;
}


uint8_t my_servo_get_angle(
    uint8_t servo_chan
) {
    if (!is_initialized) {
        ESP_LOGE(TAG, "Servos not initialized ");
        return 0.0f;
    }

    if (servo_chan != SERVO_PAN_CH && servo_chan != SERVO_TILT_CH) {
        ESP_LOGE(TAG, "Invalid servo channel: %d ", servo_chan);
        return 0.0f;
    }

    uint32_t duty = ledc_get_duty(LEDC_LOW_SPEED_MODE, servo_chan);

    return duty2angle(duty);
}



static esp_err_t karusel(uint8_t servo_chan) {
    const uint8_t angle_step = 1;
    const uint16_t period_ms = 200;
    for (uint8_t angle = 0 ; angle <= 180 ; angle += angle_step) {
        ESP_LOGI(TAG, "Moving to %d degrees ", angle);
        ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, servo_chan, angle2duty(angle), 0);

        vTaskDelay(pdMS_TO_TICKS(period_ms));
    }
    for (uint8_t angle = 180 - angle_step ; angle >= angle_step ; angle -= angle_step) {
        ESP_LOGI(TAG, "Moving to %d degrees ", angle);
        ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, servo_chan, angle2duty(angle), 0);

        vTaskDelay(pdMS_TO_TICKS(period_ms));
    }

    return ESP_OK;
}


esp_err_t my_servo_set_angle(
    uint8_t servo_chan, uint8_t angle
) {
    if (!is_initialized) {
        ESP_LOGE(TAG, "Servos not initialized ");
        return ESP_ERR_INVALID_STATE;
    }

    if (servo_chan != SERVO_PAN_CH && servo_chan != SERVO_TILT_CH) {
        ESP_LOGE(TAG, "Invalid servo channel: %d ", servo_chan);
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t duty = angle2duty(angle);
    // ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, servo_chan, duty, 0);  // : target_duty argument is invalid

    karusel(SERVO_PAN_CH);

    ESP_LOGI(TAG, "Set servo channel %d to %d degrees, duty: %lu ", servo_chan, angle, duty);

    vTaskDelay(pdMS_TO_TICKS(1000));
    return ESP_OK;
}


static void servo_manager_task(
    void *pvParameters
) {
    while (1) {
        karusel(SERVO_PAN_CH);
        
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}




esp_err_t run_servo_manager()
{
    // defer deinit_my_servos();

    esp_err_t task_created = xTaskCreatePinnedToCore(
                    &servo_manager_task,
                    "servo_manager_task",
                    1 << 12, // = 4096
                    NULL,
                    5,
                    NULL,
                    0 );
    if (!task_created) {
        ESP_LOGE(TAG, "Failed to create servo_manager_task ");
        return ESP_FAIL;
    }
    return ESP_OK;
}