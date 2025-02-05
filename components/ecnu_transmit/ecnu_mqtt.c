#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ecnu_log.h"
#include "mqtt_client.h"
#include "ecnu_config.h"
#define MQTT_BROKER    "mqtt://www.xiaolisong.asia:1883"
#define MQTT_TOPIC     "test/topic"

typedef struct {
    esp_mqtt_client_handle_t client;
    char * broker;
    char * topic;
} ecnu_mqtt_obj_t;

static ecnu_mqtt_obj_t g_ecnu_mqtt;
int ecnu_mqtt_send(char * msg, int len)
{
    // LOGI("MQTT send msg: %s\n", msg);
    esp_mqtt_client_publish(g_ecnu_mqtt.client, g_ecnu_mqtt.topic, msg, len, 0, 0);
    return 0;
}

// MQTT 事件处理程序
static int ecnu_mqtt_event_handler_cb(esp_mqtt_event_handle_t event) {

    if (event == NULL) {
        //LOGE("MQTT event handler callback error\n");
        return -1;
    }

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            LOGI("MQTT connected\n");
            break;

        case MQTT_EVENT_DISCONNECTED:
            LOGI("MQTT disconnected\n");
            break;

        case MQTT_EVENT_PUBLISHED:
            LOGI("Message published (msg_id=%d)\n", event->msg_id);
            break;

        case MQTT_EVENT_ERROR:
            LOGE("MQTT error\n");
            break;

        default:
            break;
    }
    return 0;
}

int ecnu_mqtt_init()
{
    LOGI("MQTT init\n");
    memset(&g_ecnu_mqtt, 0, sizeof(g_ecnu_mqtt));
    ecnu_config_get_str("mqtt", "broker", &g_ecnu_mqtt.broker);
    ecnu_config_get_str("mqtt", "topic", &g_ecnu_mqtt.topic);
    if (g_ecnu_mqtt.broker == NULL || g_ecnu_mqtt.topic == NULL) {
        LOGE("MQTT broker not found\n");
        g_ecnu_mqtt.broker = MQTT_BROKER;
        g_ecnu_mqtt.topic = MQTT_TOPIC;
    }
    // MQTT 客户端配置
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = g_ecnu_mqtt.broker,
        .buffer.out_size = 512*1024,
        .buffer.size = 512*1024,
    };
    // 创建并启动 MQTT 客户端
    g_ecnu_mqtt.client = esp_mqtt_client_init(&mqtt_cfg);
    if (g_ecnu_mqtt.client == NULL) {
        LOGE("MQTT client init failed\n");
        return -1;
    }
    esp_mqtt_client_register_event(g_ecnu_mqtt.client, ESP_EVENT_ANY_ID, ecnu_mqtt_event_handler_cb, NULL);
    esp_mqtt_client_start(g_ecnu_mqtt.client);

    return 0;
}