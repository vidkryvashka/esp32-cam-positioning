#include "defs.h"
#include "server/webserver.h"
#include "img_processing/photographer.h"

#ifndef TAG
    #define TAG "my_main"
#endif



void app_main(void)
{
    init_esp_things();

    init_camera();
    run_photographer();
    server_up();
}