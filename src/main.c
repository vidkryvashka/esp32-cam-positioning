#include "defs.h"
// #include "camera.h"
#include "webserver.h"


void app_main(void)
{
    // esp configuring
    GPIO_INIT();

    init_camera();
    server_up();
}