#include "ecnu_log.h"
#include "ecnu_utils.h"
#include <time.h>
#include <sys/time.h>
long long ecnu_get_current_timestamp_ms()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}