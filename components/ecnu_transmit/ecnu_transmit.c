#include "ecnu_log.h"
#include "ecnu_transmit.h"
#include "ecnu_mqtt.h"
typedef struct {
    int (*send) (char *msg, int len);
    int (*recv) (char *msg, int len);
} ecnu_transmit_wrapper_t;


typedef struct {
    ecnu_transmit_wrapper_t mqtt;
    ecnu_transmit_wrapper_t ble;
} ecnu_transmit_obj_t;

static ecnu_transmit_obj_t g_ecnu_transmit;

int ecnu_transmit_send(char *msg, int len)
{

    return g_ecnu_transmit.mqtt.send(msg, len);
}

int ecnu_transmit_init()
{
    LOGI("ecnu_transmit init\n");
    memset(&g_ecnu_transmit, 0x00, sizeof(g_ecnu_transmit));

    ecnu_mqtt_init();

    g_ecnu_transmit.mqtt.send = ecnu_mqtt_send;

    return 0;
}
