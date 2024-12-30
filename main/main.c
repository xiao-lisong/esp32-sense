#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"


int app_main()
{
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    printf("Hello World\n");

    esp_restart();
    while (1) {
        printf("app is alive\n");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    
    return 0;
}