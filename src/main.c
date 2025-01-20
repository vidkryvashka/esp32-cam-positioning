#include "defs.h"
#include "camera.h"
#include "server/webserver.h"

#ifndef TAG
#define TAG "esp_main"
#endif

void app_main(void)
{
    init_esp_things();

    init_camera();
    server_up();
}