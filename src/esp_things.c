#include "defs.h"

void led_blink(float periods, uint8_t count)
{
    for (uint8_t def_i = 0; def_i < count; ++def_i) {
        LED_ON();
        vTaskDelay(BLINK_PERIOD * periods);
        LED_OFF();
        vTaskDelay(BLINK_PERIOD * periods);
    }
}