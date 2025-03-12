#include "defs.h"
#include "camera.h"
#include "server/webserver.h"
#include "find_sun.h"

#ifndef TAG
#define TAG "esp_main"
#endif


// works bad
void photographer() {
    int8_t FOVs[2] = {0, 0};
    while(1) {
        camera_fb_t *frame = esp_camera_fb_get();
        max_brightness_pixels_t *mbp = mark_sun(frame);
        get_FOVs(&mbp->center_coord, FOVs);

        esp_camera_fb_return(frame);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    init_esp_things();

    init_camera();
    photographer();
    // server_up();
}