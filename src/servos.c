#include "freertos/FreeRTOS.h"
#include "driver/ledc.h"
#include "esp_log.h"

#include "my_servos.h"

#define TAG "my_servos"



#define SERVOS_SPEED_MODE   LEDC_HIGH_SPEED_MODE

#define SERVO_PAN_GPIO      GPIO_NUM_14
#define SERVO_TILT_GPIO     GPIO_NUM_15
#define SERVO_PAN_TIMER     LEDC_TIMER_1
#define SERVO_TILT_TIMER    LEDC_TIMER_2

#define SERVO_FREQ_HZ       50
#define DUTY_RESOLUTION     LEDC_TIMER_12_BIT
#define RESOLUTION          4096
#define MY_HPOINT           0

#define SERVO_MIN_PAN   0
#define SERVO_MAX_PAN   180
#define SERVO_MIN_TILT  40
#define SERVO_MAX_TILT  160

#define ANGLE_DIVIDER   1.2

#define LOCAL_LOG_LEVEL     1

#define START_PAN_ANGLE     90
#define START_TILT_ANGLE    70



bool are_servos_inited = 0;


static uint32_t angle2duty(
    double angle
) {
    if (angle > 180.0) {
        ESP_LOGW(TAG, "angle2duty clamping angle %f.0 to 180", angle);
        angle = 180.0;
    }

    // Map angle (0° -> 0.5ms, 180° -> 2.5ms)
    double pulse_width = 0.5 + (double)angle / 90.0; // / 180.0 * 2.0;
    // Convert pulse width to duty cycle (12-bit resolution, 50Hz)
    uint32_t duty = (pulse_width / 20.0) * RESOLUTION;

    if (duty >= RESOLUTION) {
        ESP_LOGW(TAG, "angle2duty clamping duty %lu to %d", duty, RESOLUTION - 1);
        duty = RESOLUTION - 1;
    }

    return duty;
}


static uint8_t duty2angle(
    uint32_t duty
) {
    if (duty >= RESOLUTION) {
        ESP_LOGE(TAG, "duty2angle \t --- duty2angle got wrong duty %lu so it's %d ", duty, RESOLUTION - 1);
        return 180;
    }

    double pulse_width = (double)duty / (double)RESOLUTION * 20.0;
    uint8_t angle = (pulse_width - 0.5) * 90.0; // * 180.0 / 2.0;

    if (angle > 180) {
        ESP_LOGE(TAG, "duty2angle \t --- made angle %d duty %lu, now 180° ", angle, duty);
        angle = 180;
    }

    return angle;
}


static esp_err_t configure_timers(
    void
) {
    esp_err_t err = ESP_OK;
    
    ledc_timer_config_t ledc_timer_pan = {
        .speed_mode = SERVOS_SPEED_MODE,
        .timer_num = SERVO_PAN_TIMER,
        .duty_resolution = DUTY_RESOLUTION,
        .freq_hz = SERVO_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    err |= ledc_timer_config(&ledc_timer_pan);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "configure_timers \t\t --- failed to configure LEDC pan timer: %s ", esp_err_to_name(err));
        return err;
    }

    ledc_timer_config_t ledc_timer_tilt = {
        .speed_mode = SERVOS_SPEED_MODE,
        .timer_num = SERVO_TILT_TIMER,
        .duty_resolution = DUTY_RESOLUTION,
        .freq_hz = SERVO_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    err |= ledc_timer_config(&ledc_timer_tilt);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "configure_timers \t\t --- failed to configure LEDC tilt timer: %s ", esp_err_to_name(err));
        return err;
    }

    return err;
}


static esp_err_t configure_ledc_channels(
    void
) {
    esp_err_t err = ESP_OK;

    ledc_channel_config_t pan_channel = {
        .channel = SERVO_PAN_CHANNEL,
        .duty = angle2duty(START_PAN_ANGLE),
        .gpio_num = SERVO_PAN_GPIO,
        .speed_mode = SERVOS_SPEED_MODE,
        .hpoint = MY_HPOINT,
        .timer_sel = SERVO_PAN_TIMER
    };
    err = ledc_channel_config(&pan_channel);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "configure_ledc_channels \t\t --- failed to configure pan channel, err: %s ", esp_err_to_name(err));
        return err;
    }

    ledc_channel_config_t tilt_channel = {
        .channel = SERVO_TILT_CHANNEL,
        .duty = angle2duty(START_TILT_ANGLE),
        .gpio_num = SERVO_TILT_GPIO,
        .speed_mode = SERVOS_SPEED_MODE,
        .hpoint = MY_HPOINT,
        .timer_sel = SERVO_TILT_TIMER
    };
    err |= ledc_channel_config(&tilt_channel);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "configure_ledc_channels \t\t --- failed to configure tilt channel, err: %s ", esp_err_to_name(err));
        return err;
    }

    return err;
}


esp_err_t init_my_servos(
    void
) {
    esp_err_t err = ESP_OK;

    if (are_servos_inited) {
        ESP_LOGW(TAG, "Servos already initialized ");
        return ESP_OK;
    }

    servo_queue = xQueueCreate(SERVO_QUEUE_LEN, sizeof(angles_diff_t));
    if (servo_queue == NULL) {
        ESP_LOGE(TAG, "init_my_servos \t\t --- Failed to create servo queue ");
        abort();
    }

    ESP_ERROR_CHECK(configure_timers());
    ESP_ERROR_CHECK(configure_ledc_channels());

    ESP_ERROR_CHECK(ledc_fade_func_install(0));

    are_servos_inited = 1;
    if (!err)
        #if LOCAL_LOG_LEVEL
            ESP_LOGI(TAG, "\n\t\t -- servos initialized successfully \n ");
        #endif
    return err;
}


static void check_normalize_angle_boundries(
    int16_t *target_pan_angle,
    int16_t *target_tilt_angle
) {
    #if LOCAL_LOG_LEVEL
        ESP_LOGI(TAG, "check_normalize_angle_boundries \t start ");
    #endif

    if (*target_pan_angle < SERVO_MIN_PAN) {
        #if LOCAL_LOG_LEVEL
            ESP_LOGI(TAG, "my_servos_change_angle target pan angle too left ");
        #endif
        *target_pan_angle = SERVO_MIN_PAN;
    }
    if (*target_pan_angle > SERVO_MAX_PAN) {
        #if LOCAL_LOG_LEVEL
            ESP_LOGI(TAG, "my_servos_change_angle target pan angle too right ");
        #endif
        *target_pan_angle = SERVO_MAX_PAN;
    }

    if (*target_tilt_angle < SERVO_MIN_TILT) {
        #if LOCAL_LOG_LEVEL
            ESP_LOGI(TAG, "my_servos_change_angle target tilt angle too high ");
        #endif
        *target_tilt_angle = SERVO_MIN_TILT;
    }
    if (*target_tilt_angle > SERVO_MAX_TILT) {
        #if LOCAL_LOG_LEVEL
            ESP_LOGI(TAG, "my_servos_change_angle target tilt angle too low ");
        #endif
        *target_tilt_angle = SERVO_MAX_TILT;
    }
    #if LOCAL_LOG_LEVEL
        ESP_LOGI(TAG, "check_normalize_angle_boundries \t end ");
    #endif
}


static esp_err_t set_duties(
    const angles_diff_t *angles_diff,
    const int16_t target_pan_angle,
    const int16_t target_tilt_angle
) {
    esp_err_t pwm_err = 0;
    if (angles_diff->pan != 0)
        pwm_err |= ledc_set_duty_and_update(
            SERVOS_SPEED_MODE,
            SERVO_PAN_CHANNEL,
            angle2duty((double)target_pan_angle / ANGLE_DIVIDER),
            MY_HPOINT
        );
    vTaskDelay(pdMS_TO_TICKS(25));
    if (angles_diff->tilt != 0)
        pwm_err |= ledc_set_duty_and_update(
            SERVOS_SPEED_MODE,
            SERVO_TILT_CHANNEL,
            angle2duty((double)target_tilt_angle / ANGLE_DIVIDER),
            MY_HPOINT
        );
    vTaskDelay(pdMS_TO_TICKS(25));

    return pwm_err;
}


static esp_err_t my_servos_change_angles(
    const angles_diff_t *angles_diff
) {
    if (!are_servos_inited) {
        ESP_LOGE(TAG, "my_servos_change_angles \t\t --- servos not initialized ");
        return ESP_ERR_INVALID_STATE;
    }

    if (abs(angles_diff->pan) > 90 || abs(angles_diff->tilt) > 90) {
        ESP_LOGE(TAG, "my_servos_change_angles \t\t --- angle diff too large: pan %d, tilt %d ", 
                angles_diff->pan, angles_diff->tilt);
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t measured_pan_angle = duty2angle(ledc_get_duty(SERVOS_SPEED_MODE, SERVO_PAN_CHANNEL));
    vTaskDelay(pdMS_TO_TICKS(25));
    uint8_t measured_tilt_angle = duty2angle(ledc_get_duty(SERVOS_SPEED_MODE, SERVO_TILT_CHANNEL));
    vTaskDelay(pdMS_TO_TICKS(25));

    int16_t target_pan_angle = measured_pan_angle + angles_diff->pan;
    int16_t target_tilt_angle = measured_tilt_angle + angles_diff->tilt;

    check_normalize_angle_boundries(&target_pan_angle, &target_tilt_angle);

    if (abs(target_pan_angle - (int16_t)measured_pan_angle) < ANGLE_THRESHOLD &&
        abs(target_tilt_angle - (int16_t)measured_tilt_angle) < ANGLE_THRESHOLD
    )
        return ESP_OK;

    esp_err_t pwm_err = set_duties(angles_diff, target_pan_angle, target_tilt_angle);

    #if LOCAL_LOG_LEVEL
        ESP_LOGI(TAG, "my_servos_change_angles \t  -- pan tilt: target %d° %d°  measured %d° %d° ",
                target_pan_angle, target_tilt_angle, measured_pan_angle, measured_tilt_angle);
    #endif

    return pwm_err;
}


static void servo_manager_task(
    void *pvParameters
) {
    angles_diff_t angles_diff = (angles_diff_t){0,0};

    while (1) {
        if (xQueueReceive(servo_queue, (void *)&angles_diff, 10) == pdTRUE) {
            #if LOCAL_LOG_LEVEL
                ESP_LOGI(TAG, "servo_manager_task \t -- got angles_diff %d° %d° ", angles_diff.pan, angles_diff.tilt);
            #endif
            esp_err_t err = my_servos_change_angles(&angles_diff);
            if (err)
                ESP_LOGE(TAG, "servo_manager_task \t\t\t --- failed to change servo angles: %s ", esp_err_to_name(err));
        }
        vTaskDelay(pdMS_TO_TICKS(pdMS_TO_TICKS(5)));
    }
}


esp_err_t run_servo_manager(
    void
) {
    esp_err_t servos_err = init_my_servos();
    ESP_ERROR_CHECK(servos_err);

    BaseType_t task_created = xTaskCreatePinnedToCore(
                    &servo_manager_task,
                    "servo_manager_task",
                    8192, // 6138,
                    NULL,
                    8,
                    NULL,
                    1);
    if (task_created != pdPASS) {
        ESP_LOGE(TAG, "run_servo_manager \t\t --- failed to create servo_manager_task ");
        return ESP_FAIL;
    }

    vTaskDelay(pdMS_TO_TICKS(1000));

    return servos_err;
}