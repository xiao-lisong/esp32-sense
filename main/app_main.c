#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "ecnu_log.h"
#include "ecnu_wifi.h"
#include "nvs_flash.h"
#include "ecnu_transmit.h"

#define WIFI_SSID "xiaolisong_F3A057_DH"
#define WIFI_PASSWORD "12345678"


static void system_info_dump()
{
    static char InfoBuffer[1024] = {0}; 

    memset(InfoBuffer, 0, sizeof(InfoBuffer));
    LOGI("free_heap_size = %ld\n", esp_get_free_heap_size());
    vTaskList((char *) &InfoBuffer);
    printf("任务名      任务状态 优先级   剩余栈 任务序号  运行核\n");
    printf("%s\n", InfoBuffer);

    return;
}

int app_main()
{
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    LOGI("Hello World\n");

    nvs_flash_init();

    ecnu_wifi_init();
    ecnu_wifi_set_config(WIFI_SSID, WIFI_PASSWORD);
    ecnu_wifi_connect();

    ecnu_wifi_wait_connected();

    ecnu_transmit_init();

    int num = 0;
    char send_data[16] = {0};
    while (1) {
        memset(send_data, 0, sizeof(send_data));
        sprintf(send_data, "test %d", num++);
        ecnu_transmit_send(send_data, strlen(send_data) + 1);
        system_info_dump();
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
    
    return 0;
}