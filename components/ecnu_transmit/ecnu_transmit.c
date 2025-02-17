#include "ecnu_log.h"
#include "ecnu_transmit.h"
#include "ecnu_mqtt.h"
#include "ecnu_tcp.h"

typedef struct {
    int (*send) (char *msg, int len);
    int (*recv) (char *msg, int len);
} ecnu_transmit_wrapper_t;

typedef enum {
    ECNU_TRANSMIT_MQTT,
    ECNU_TRANSMIT_TCP,
    ECNU_TRANSMIT_BLE
};

typedef struct {
    int type;
    ecnu_transmit_wrapper_t transmit;
} ecnu_transmit_obj_t;

static ecnu_transmit_obj_t g_ecnu_transmit;

int ecnu_transmit_send(char *msg, int len)
{
    return g_ecnu_transmit.transmit.send(msg, len);
}

int ecnu_transmit_init()
{
    LOGI("ecnu_transmit init\n");
    memset(&g_ecnu_transmit, 0x00, sizeof(g_ecnu_transmit));
    g_ecnu_transmit.type = ECNU_TRANSMIT_TCP;


    switch (g_ecnu_transmit.type) {
    case ECNU_TRANSMIT_MQTT:
        ecnu_mqtt_init();
        g_ecnu_transmit.transmit.send = ecnu_mqtt_send;
        break;
    case ECNU_TRANSMIT_TCP:
        ecnu_tcp_init();
        g_ecnu_transmit.transmit.send = ecnu_tcp_send;
        break;
    default:
        LOGE("ecnu_transmit init error\n");
        return -1;
    }
    return 0;
}
