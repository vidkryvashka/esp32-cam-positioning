#include "defs.h"
#include "my_servos.h"
#include "server/webserver.h"
#include "img_processing/photographer.h"

#define TAG "my_main"



void app_main(void)
{
    init_esp_things();

    init_camera();
    run_photographer();
    server_up();
    // run_servo_manager();
}