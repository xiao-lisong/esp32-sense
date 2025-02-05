#ifndef ECNU_CONFIG_H
#define ECNU_CONFIG_H

int ecnu_config_get_int(const char *type, const char *key, void **value);

int ecnu_config_get_str(const char *type, const char *key, void **value);

int ecnu_config_init(void);

#endif