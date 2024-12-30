#ifndef ECNU_LOG_H
#define ECNU_LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C" {
#endif
#define ONLY_FILENAME(x) (strrchr(x, '/') ? strrchr(x, '/')+1 : x)

#ifndef LOGI
#define LOGI(fmt, ...)  printf("[I][%s:%d] " fmt "", ONLY_FILENAME(__FILE__), __LINE__, ##__VA_ARGS__)
#endif

#ifndef LOGE
#define LOGE(fmt, ...)  printf("[E][%s:%d] " fmt "", ONLY_FILENAME(__FILE__), __LINE__, ##__VA_ARGS__)
#endif

#ifndef LOGW
#define LOGW(fmt, ...)  printf("[W][%s:%d] " fmt "", ONLY_FILENAME(__FILE__), __LINE__, ##__VA_ARGS__)
#endif

#ifndef LOGD
#define LOGD(fmt, ...)
#endif

#ifdef __cplusplus
}
#endif

#endif