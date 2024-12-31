#include "ecnu_log.h"
#include "ecnu_packege.h"
#include "esp_system.h"
#include <stdio.h>
#include <string.h>
#include "cJSON.h"
static const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static void base64_encode(const unsigned char *src, size_t len, char *out) {
    int i, j;
    for (i = 0, j = 0; i < len; i += 3) {
        unsigned int octet_a = i < len ? src[i] : 0;
        unsigned int octet_b = i + 1 < len ? src[i + 1] : 0;
        unsigned int octet_c = i + 2 < len ? src[i + 2] : 0;

        unsigned int triple = (octet_a << 16) | (octet_b << 8) | octet_c;

        out[j++] = base64_table[(triple >> 18) & 0x3F];
        out[j++] = base64_table[(triple >> 12) & 0x3F];
        out[j++] = (i + 1 < len) ? base64_table[(triple >> 6) & 0x3F] : '=';
        out[j++] = (i + 2 < len) ? base64_table[triple & 0x3F] : '=';
    }
    out[j] = '\0';
}



static int ecnu_packege_encode_image(const char * input, int input_len, char * output, int output_len, void * extra_data)
{
    LOGI("ecnu_packege_encode_image\n");

    if (input == NULL || output == NULL || extra_data == NULL) {
        LOGE("input or output is NULL\n");
        return -1;
    }

    if (input_len <= 0 || output_len <= 0 || input_len * 4 / 3 > output_len) {
        LOGE("invalid input %d or output %d length\n", input_len, output_len);
        return -1;
    }

    ecnu_packege_extra_image * extra = (ecnu_packege_extra_image *)extra_data;
    memset(output, 0, output_len);
    // 对图片进行base64编码
    base64_encode((const unsigned char *)input, input_len, output);
    // LOGI("base64_encode output %s\n", output);
    // LOGI("base64_encode output len %d\n", strlen(output));


    // 组装json协议
    cJSON* root = cJSON_CreateObject();
    if (root == NULL) {
        LOGE("cJSON_CreateObject failed\n");
        return -1;
    }

    cJSON_AddStringToObject(root, "device_id", "test1");
    cJSON_AddNumberToObject(root, "timestamp", extra->timestamp);
    cJSON_AddStringToObject(root, "data_type", "image");
    cJSON* payload = cJSON_CreateObject();
    if (payload == NULL) {
        cJSON_Delete(root);
        LOGE("cJSON_CreateObject failed\n");
        return -1;
    }

    cJSON_AddItemToObject(root, "payload", payload);
    cJSON* image = cJSON_CreateObject();
    if (image == NULL) {
        cJSON_Delete(root);
        LOGE("cJSON_CreateObject failed\n");
        return -1;
    }

    cJSON_AddItemToObject(payload, "image", image);
    cJSON_AddStringToObject(image, "format", extra->format);
    cJSON_AddNumberToObject(image, "width", extra->width);
    cJSON_AddNumberToObject(image, "height", extra->height);
    cJSON_AddStringToObject(image, "data", output);
    if (!cJSON_PrintPreallocated(root, output, output_len, 0)) {
        LOGE("cJSON_PrintPreallocated failed\n");
        return -1;
    }
    cJSON_Delete(root);

    // LOGI("ecnu_packege_encode output %s\n", output);

    return strlen(output);
}


int ecnu_packege_encode(int type, const char * input, int input_len, char * output, int output_len, void * extra_data)
{
    if (input == NULL || output == NULL) {
        LOGE("input or output is NULL\n");
        return -1;
    }


    switch (type) {
        case ECNU_PACKEGE_ENCODE_IMAGE:
            return ecnu_packege_encode_image(input, input_len, output, output_len, extra_data);
            break;
        default:
            LOGE("unknown type %d\n", type);
            return -1;
    }

    return 0;
}
