#include "defs.h"
#include "server/webserver.h"
#include "img_processing/follow_obj_in_img.h"

#ifndef TAG
#define TAG "main"
#endif



void app_main(void)
{
    init_esp_things();

    init_camera();
    run_photographer();
    // compare_frames();
    server_up();
}