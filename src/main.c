#include "defs.h"
#include "camera.h"
#include "webserver.h"


void app_main(void)
{
    init_esp_things();

    init_camera();
    server_up();
}