#include "ecnu_log.h"
#include "ecnu_config.h"
#include "esp_partition.h"
#include "cJSON.h"


typedef struct {
    char * config_str;

    cJSON *config_root;

} ecmu_config_t;

static ecmu_config_t g_ecnu_config_obj;

int ecnu_config_get_int(const char *type, const char *key, void *value)
{
    if (g_ecnu_config_obj.config_root == NULL) {
        LOGE("Config is not initialized\n");
        return -1;
    }

    cJSON *type_json = cJSON_GetObjectItem(g_ecnu_config_obj.config_root, type);
    if (type_json == NULL) {
        LOGE("Failed to find type %s\n", type);
        return -1;
    }
    cJSON *key_json = cJSON_GetObjectItem(type_json, key);
    if (key_json == NULL) {
        LOGE("Failed to find key %s\n", key);
        return -1;
    }
    if (cJSON_IsNumber(key_json)) {
        *((int *)value) = key_json->valueint;
        return 0;
    }
    LOGE("Failed to find int value %s %s\n", type, key);
    return -1;
}

int ecnu_config_get_str(const char *type, const char *key, void **value)
{
    if (g_ecnu_config_obj.config_root == NULL) {
        LOGE("Config is not initialized\n");
        return -1;
    }

    cJSON *type_json = cJSON_GetObjectItem(g_ecnu_config_obj.config_root, type);
    if (type_json == NULL) {
        LOGE("Failed to find type %s\n", type);
        return -1;
    }
    cJSON *key_json = cJSON_GetObjectItem(type_json, key);
    if (key_json == NULL) {
        LOGE("Failed to find key %s\n", key);
        return -1;
    }
    if (cJSON_IsString(key_json)) {
        *((char **)value) = key_json->valuestring;
        return 0;
    }
    LOGE("Failed to find string value %s %s\n", type, key);
    return -1;
}


int ecnu_config_init(void)
{
    memset(&g_ecnu_config_obj, 0, sizeof(ecmu_config_t));

    esp_partition_t * part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "profile");
    if (part == NULL) {
        LOGE("Failed to find profile partition\n");
        return -1;
    }

    g_ecnu_config_obj.config_str = (char *)malloc(part->size);
    if (g_ecnu_config_obj.config_str == NULL) {
        LOGE("Failed to malloc config_str\n");
        return -1;
    }
    memset(g_ecnu_config_obj.config_str, 0, part->size);
    esp_partition_read(part, 0, (void *)g_ecnu_config_obj.config_str, part->size);
    g_ecnu_config_obj.config_root = cJSON_Parse(g_ecnu_config_obj.config_str);
    if (g_ecnu_config_obj.config_root == NULL) {
        LOGE("Failed to parse config\n");
        return -1;
    }
    return 0;
}