#include "ecnu_log.h"
#include "ecnu_wifi.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_sntp.h"
#include "freertos/queue.h"

#define ECNU_WIFI_EVENT_QUEUE_SIZE (32)

typedef enum {
    ECNU_WIFI_CMD_CONNECT,
    ECNU_WIFI_CMD_STNP,
};

typedef struct {
    int cmd;
} ecnu_wifi_event_t;

typedef struct {
    char ssid[32];
    char password[64];
    QueueHandle_t event_queue;
    esp_netif_t *sta_netif;
    int status;
} ecnu_wifi_obj_t;

static ecnu_wifi_obj_t g_wifi_obj;

int ecnu_wifi_wait_connected()
{

    while (g_wifi_obj.status != 2) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    return 0;
}

int ecnu_wifi_connect()
{
    ecnu_wifi_event_t event = {
        .cmd = ECNU_WIFI_CMD_CONNECT,
    };

    if (xQueueSend(g_wifi_obj.event_queue, &event, portMAX_DELAY) != pdPASS) {
        LOGE("Failed to send connect event\n");
        return -1;
    }

    return 0;
}


static int ecnu_wifi_task_connect()
{
    LOGE("Connect to WiFi %s %s \n", g_wifi_obj.ssid, g_wifi_obj.password);
    wifi_config_t wifi_config = {0};
    memcpy(wifi_config.sta.ssid, g_wifi_obj.ssid, strlen(g_wifi_obj.ssid));
    memcpy(wifi_config.sta.password, g_wifi_obj.password, strlen(g_wifi_obj.password));
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    return 0;
}

static int ecnu_wifi_task_stnp()
{
    LOGI("Initializing SNTP\n");
    if (esp_sntp_enabled()){
        esp_sntp_stop();
    }
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "ntp.aliyun.com");
    esp_sntp_init();

    // 等待时间同步完成
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 100;
    LOGI("sync time\n");
    while (timeinfo.tm_year < 80 && ++retry < retry_count) {

        vTaskDelay(1000 / portTICK_PERIOD_MS);

        time(&now);
        localtime_r(&now, &timeinfo);
        LOGI("waiting time sntp...\n");
    }
    LOGI("time sync done\n");
    struct timeval tv = { .tv_sec = now };
    settimeofday(&tv, NULL);

    LOGI("Current time: %s %d\n", asctime(&timeinfo), timeinfo.tm_year );
    g_wifi_obj.status = 2;
    return 0;
}


static void ecnu_wifi_task(void *pvParameters)
{
    ecnu_wifi_event_t event = {0};
    while (1) {
        if (xQueueReceive(g_wifi_obj.event_queue, &event, portMAX_DELAY) != pdPASS) {
            LOGE("Failed to receive event\n");
            continue;
        }

        switch (event.cmd) {
            case ECNU_WIFI_CMD_CONNECT:
                ecnu_wifi_task_connect();
                break;
            case ECNU_WIFI_CMD_STNP:
                ecnu_wifi_task_stnp();
                break;
            default:
                LOGE("Unknown event %d\n", event.cmd);
                break;
        }

    }
    vTaskDelete(NULL);
}


int ecnu_wifi_set_config(const char *ssid, const char *password)
{
    if (strlen(ssid) > 32 || strlen(password) > 64) {
        LOGE("SSID or password too long\n");
        return -1;
    }

    memcpy(g_wifi_obj.ssid, ssid, strlen(ssid));
    memcpy(g_wifi_obj.password, password, strlen(password));

    return 0;
}

static void ecnu_wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    LOGI("event_base: %s, event_id: %ld", event_base, event_id);
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        LOGI("Start WiFi connection\n");
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        LOGI("Disconnected from %s\n", g_wifi_obj.ssid);
        esp_wifi_connect();
        g_wifi_obj.status = 0;
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
        LOGI("Connected to %s\n", g_wifi_obj.ssid);
        g_wifi_obj.status = 1;
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        LOGI("Got IP: " IPSTR "\n", IP2STR(&event->ip_info.ip));
        ecnu_wifi_event_t wifi_event = {
            .cmd = ECNU_WIFI_CMD_STNP,
        };
        if (xQueueSend(g_wifi_obj.event_queue, &wifi_event, portMAX_DELAY) != pdPASS) {
            LOGE("Failed to send stnp event\n");
        }
    } 
}

int ecnu_wifi_init()
{
    LOGI("Initializing WiFi...\n");
    memset(&g_wifi_obj, 0, sizeof(ecnu_wifi_obj_t));

    // queue init
    g_wifi_obj.event_queue = xQueueCreate(ECNU_WIFI_EVENT_QUEUE_SIZE, sizeof(ecnu_wifi_event_t));

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    g_wifi_obj.sta_netif = esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );

    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &ecnu_wifi_event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ecnu_wifi_event_handler, NULL) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );

    xTaskCreate(ecnu_wifi_task, "ecnu_wifi", 4096, NULL, 5, NULL);

    return 0;
}