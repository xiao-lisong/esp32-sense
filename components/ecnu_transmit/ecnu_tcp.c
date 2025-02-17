#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ecnu_log.h"
#include "ecnu_tcp.h"
#include "ecnu_config.h"
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct {
    int sock;
    char * server;
    int port;
} ecnu_tcp_obj_t;

static ecnu_tcp_obj_t g_ecnu_tcp;

static int create_tcp_client() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        LOGE("create socket failed\n");
        return -1;
    }

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(g_ecnu_tcp.port),
        .sin_addr.s_addr = inet_addr(g_ecnu_tcp.server),
    };

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        LOGE("connect server failed\n");
        close(sock);
        return -1;
    }

    LOGI("connect server %s:%d success\n", g_ecnu_tcp.server, g_ecnu_tcp.port);
    g_ecnu_tcp.sock = sock;
    return 0;
}


int ecnu_tcp_send(char * msg, int len)
{
    int ret = 0;
    if (g_ecnu_tcp.sock < 0) {
        LOGE("socket not init\n");
        ret = create_tcp_client();
        if (ret < 0 || g_ecnu_tcp.sock < 0) {
            LOGE("create tcp client failed\n");
            return -1;
        }
    }

    ret = send(g_ecnu_tcp.sock, msg, len, 0);
    if (ret < 0) {
        LOGE("send failed\n");
        close(g_ecnu_tcp.sock);
        g_ecnu_tcp.sock = -1;
        return -1;
    }

    return 0;
}

int ecnu_tcp_init()
{
    LOGI("TCP init\n");
    memset(&g_ecnu_tcp, 0, sizeof(g_ecnu_tcp));
    ecnu_config_get_str("tcp", "server", &g_ecnu_tcp.server);
    ecnu_config_get_int("tcp", "port", &g_ecnu_tcp.port);
    LOGI("TCP server:%s, port:%d\n", g_ecnu_tcp.server, g_ecnu_tcp.port);

    create_tcp_client();
    if (g_ecnu_tcp.sock < 0) {
        LOGE("create tcp client failed\n");
    }

    return 0;
}