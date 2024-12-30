#ifndef ECNU_WIFI_H
#define ECNU_WIFI_H

int ecnu_wifi_wait_connected();

int ecnu_wifi_connect();

int ecnu_wifi_set_config(const char *ssid, const char *password);

int ecnu_wifi_init();

#endif //ECNU_LOG_H