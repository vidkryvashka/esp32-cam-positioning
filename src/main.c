#include "defs.h"
#include "camera.h"
#include "server/webserver.h"
#include "find_sun.h"

#ifndef TAG
#define TAG "esp_main"
#endif

// // to replace server for cli only testing
// void photographer() {
//     while(1) {
//         camera_fb_t *frame = esp_camera_fb_get();
//         mark_sun(frame);

//         esp_camera_fb_return(frame);
//         vTaskDelay(pdMS_TO_TICKS(1000));
//     }
// }

void app_main(void)
{
    init_esp_things();

    init_camera();
    // photographer();
    server_up();
}