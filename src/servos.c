#include "freertos/FreeRTOS.h"
#include "driver/ledc.h"
#include "esp_log.h"

#include "my_servos.h"

#define TAG "my_servos"

#define SERVO_PAN_GPIO  GPIO_NUM_14
#define SERVO_TILT_GPIO GPIO_NUM_15
#define SERVO_FREQ_HZ   50
#define DUTY_RESOLUTION LEDC_TIMER_12_BIT

#define ANGLE_DIVIDER   1.5

static bool is_initialized = false;

static uint8_t curr_pan_angle = 90;
static uint8_t curr_tilt_angle = 90;


static uint32_t angle2duty(
    uint8_t angle
) {
    if (angle > 180)
        angle = 180;
    
    // Map angle (0° -> 0.5ms, 180° -> 2.5ms)
    double pulse_width = 0.5 + ((double)angle / 180.0) * 2.0;
    // Convert pulse width to duty cycle (12-bit resolution, 50Hz)
    uint32_t duty = (pulse_width / 20.0) * 4096;

    return duty;
}

// static uint8_t duty2angle(
//     uint32_t duty
// ) {
//     if (duty > 4095) {
//         ESP_LOGE(TAG, "\t--- duty2angle got wrong duty %lu so it's 4095", duty);
//         // duty = 1 << LEDC_TIMER_12_BIT;
//         return 180;
//     }
// 
//     double pulse_width = (double)duty / 4096.0 * 20.0;
//     uint8_t angle = (pulse_width - 0.5) * 180.0 / 2.0;
// 
//     if (angle > 180) {
//         ESP_LOGE(TAG, "duty2angle made angle %d duty %lu, now 180° ", angle, duty);
//         angle = 180;
//     }
// 
//     return angle;
// }


esp_err_t init_my_servos()
{
    esp_err_t ret = ESP_OK;

    servo_queue = xQueueCreate(SERVO_QUEUE_LEN, sizeof(angles_diff_t));

    if (is_initialized) {
        ESP_LOGW(TAG, "Servos already initialized ");
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
        .duty = angle2duty(curr_pan_angle),
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
        .duty = angle2duty(curr_tilt_angle),
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


extern esp_err_t rand_angles_send() {
    angles_diff_t angles_diff = {rand() % 30, rand() % 10};

    if (xQueueSend(servo_queue, (void *)&angles_diff, 10) != pdTRUE) {
        ESP_LOGE(TAG, "rand_angles_send couldn't xQueueSend, queue fill ");
    }

    return ESP_OK;
}


static esp_err_t my_servos_change_angles(
    const angles_diff_t *angles_diff
) {
    if (!is_initialized) {
        ESP_LOGE(TAG, "Servos not initialized ");
        return ESP_ERR_INVALID_STATE;
    }

    // uint8_t curr_pan_angle = duty2angle(ledc_get_duty(LEDC_LOW_SPEED_MODE, SERVO_TILT_CH));
    // uint8_t curr_tilt_angle = duty2angle(ledc_get_duty(LEDC_LOW_SPEED_MODE, SERVO_TILT_CH));

    int16_t target_pan_angle = curr_pan_angle + angles_diff->pan;
    int16_t target_tilt_angle = curr_tilt_angle + angles_diff->tilt;

    if (target_pan_angle < SERVO_PAN_ANGLE_BOTTOM) {
        ESP_LOGI(TAG, "my_servos_change_angle target pan angle too low ");
        target_pan_angle = SERVO_PAN_ANGLE_BOTTOM;
    }
    if (target_pan_angle > SERVO_PAN_ANGLE_TOP) {
        ESP_LOGI(TAG, "my_servos_change_angle target pan angle too big ");
        target_pan_angle = SERVO_PAN_ANGLE_TOP;
    }
    if (target_pan_angle < SERVO_TILT_ANGLE_BOTTOM) {
        ESP_LOGI(TAG, "my_servos_change_angle target tilt angle too low ");
        target_pan_angle = SERVO_TILT_ANGLE_BOTTOM;
    }
    if (target_pan_angle > SERVO_TILT_ANGLE_TOP) {
        ESP_LOGI(TAG, "my_servos_change_angle target tilt angle too big ");
        target_pan_angle = SERVO_TILT_ANGLE_TOP;
    }

    ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE,
                                SERVO_PAN_CH,
                                angle2duty(target_pan_angle/ANGLE_DIVIDER),
                                0);  // : target_duty argument is invalid
    vTaskDelay(pdMS_TO_TICKS(50));
    ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE,
                                SERVO_TILT_CH,
                                angle2duty(target_tilt_angle/ANGLE_DIVIDER),
                                0);  // : target_duty argument is invalid

    ESP_LOGI(TAG, "Set servo pan -> %d° tilt -> %d° ", target_pan_angle, target_tilt_angle);

    return ESP_OK;
}


static void servo_manager_task(
    void *pvParameters
) {
    angles_diff_t angles_diff = {0,0};

    while (1) {
        if (xQueueReceive(servo_queue, (void *)&angles_diff, 0) == pdTRUE) {
            ESP_LOGI(TAG, "\t\t--- got angles_diff %d° %d° ", angles_diff.pan, angles_diff.tilt);
            my_servos_change_angles(&angles_diff);
        ;
            // uint8_t pan_angle = my_servo_get_angle(SERVO_PAN_CH);
            // uint8_t tilt_angle = my_servo_get_angle(SERVO_TILT_CH);
            // ESP_LOGI(TAG, "\t\t-- servos angles pan: %d° tilt: %d° ", pan_angle, tilt_angle);
        }
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}


esp_err_t run_servo_manager()
{
    esp_err_t task_created = xTaskCreatePinnedToCore(
                    &servo_manager_task,
                    "servo_manager_task",
                    1 << 12, // = 4096
                    NULL,
                    7,
                    NULL,
                    0 );
    if (!task_created) {
        ESP_LOGE(TAG, "Failed to create servo_manager_task ");
        return ESP_FAIL;
    }
    return ESP_OK;
}