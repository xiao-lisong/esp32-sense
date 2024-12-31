#include "ecnu_log.h"
#include "ecnu_camera.h"
#include "esp_camera.h"
#include "ecnu_packege.h"
#include "ecnu_utils.h"
#include "ecnu_transmit.h"

#ifndef CAMERA_CONFIG_H
#define CAMERA_CONFIG_H

#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     10
#define SIOD_GPIO_NUM     40
#define SIOC_GPIO_NUM     39

#define Y9_GPIO_NUM       48
#define Y8_GPIO_NUM       11
#define Y7_GPIO_NUM       12
#define Y6_GPIO_NUM       14
#define Y5_GPIO_NUM       16
#define Y4_GPIO_NUM       18
#define Y3_GPIO_NUM       17
#define Y2_GPIO_NUM       15
#define VSYNC_GPIO_NUM    38
#define HREF_GPIO_NUM     47
#define PCLK_GPIO_NUM     13

#define LED_GPIO_NUM      21

#endif //CAMERA_CONFIG_H

#define ECNU_CAMERA_BUFFER_SIZE (256 * 1024)


typedef struct {



} ecnu_camera_obj_t;

static ecnu_camera_obj_t g_ecnu_camera_obj;

static void ecnu_camera_task(void *arg)
{
    int ret = 0;
    camera_fb_t *fb = NULL;
    float fps = 0;
    int64_t start_time = esp_timer_get_time();
    int64_t end_time = 0;
    int frame_count = 0;
    ecnu_packege_extra_image extra_image = {0};
    char * buffer = NULL;
    buffer = malloc(ECNU_CAMERA_BUFFER_SIZE);
    if (buffer == NULL) {
        LOGE("malloc failed");
        vTaskDelete(NULL);
        return;
    }


    while (1) {
        fb = esp_camera_fb_get();
        if (!fb) {
            LOGE("Camera capture failed");
            continue;
        }
        frame_count++;
        // 计算帧率
        if (frame_count == 20) {
            int64_t end_time = esp_timer_get_time();
            fps = ((20 * 1000000.0) / (float)(end_time - start_time));
            start_time = end_time;
            frame_count = 0;
        }

        LOGI("Camera frame fps %f, size: %d x %d, format %d, time_stamp %lld %ld, data_size %d\n",
            fps, fb->width, fb->height, fb->format, fb->timestamp.tv_sec, fb->timestamp.tv_usec, fb->len);

        // 编码json
        extra_image.width = fb->width;
        extra_image.height = fb->height;
        strcpy(extra_image.format, "jpeg");
        extra_image.timestamp = ecnu_get_current_timestamp_ms();
        long long start_tm = ecnu_get_current_timestamp_ms();
        ret = ecnu_packege_encode(ECNU_PACKEGE_ENCODE_IMAGE, (const char *)fb->buf, fb->len, buffer, ECNU_CAMERA_BUFFER_SIZE, &extra_image);
        if (ret < 0) {
            LOGE("ecnu_packege_encode failed");
        } else {
            LOGI("encode cost time %lld ms\n", ecnu_get_current_timestamp_ms() - start_tm);
            start_tm = ecnu_get_current_timestamp_ms();
            LOGI("ecnu_packege_encode success, size %d\n", ret);
            ecnu_transmit_send(buffer, ret);
            LOGI("send cost time %lld ms\n", ecnu_get_current_timestamp_ms() - start_tm);
        }

        esp_camera_fb_return(fb);
        fb = NULL;
    }

    free(buffer);
    vTaskDelete(NULL);
    return;
}


int ecnu_camera_init()
{
    LOGI("ecnu_camera_init\n");

    camera_config_t camera_config = {
        .pin_pwdn = PWDN_GPIO_NUM,
        .pin_reset = RESET_GPIO_NUM,
        .pin_xclk = XCLK_GPIO_NUM,
        .pin_sccb_sda = SIOD_GPIO_NUM,
        .pin_sccb_scl = SIOC_GPIO_NUM,
        .pin_d7 = Y9_GPIO_NUM,
        .pin_d6 = Y8_GPIO_NUM,
        .pin_d5 = Y7_GPIO_NUM,
        .pin_d4 = Y6_GPIO_NUM,
        .pin_d3 = Y5_GPIO_NUM,
        .pin_d2 = Y4_GPIO_NUM,
        .pin_d1 = Y3_GPIO_NUM,
        .pin_d0 = Y2_GPIO_NUM,
        .pin_vsync = VSYNC_GPIO_NUM,
        .pin_href = HREF_GPIO_NUM,
        .pin_pclk = PCLK_GPIO_NUM,

        .xclk_freq_hz = 20000000,          // 图像传感器的时钟频率
        .fb_location = CAMERA_FB_IN_PSRAM, // 设置帧缓冲区存储位置
        .pixel_format = PIXFORMAT_JPEG,    // 图像的像素格式：PIXFORMAT_ + YUV422|GRAYSCALE|RGB565|JPEG
        .frame_size = FRAMESIZE_HD,
        //.frame_size = FRAMESIZE_UXGA,      // 图像的分辨率大小：FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
        .jpeg_quality = 15,                // JPEG图像的质量，范围从0到63。
        .fb_count = 2,                     // 使用的帧缓冲区数量。
        .grab_mode = CAMERA_GRAB_LATEST    // 图像捕获模式。
    };

    int ret = esp_camera_init(&camera_config);
    if (ret != ESP_OK) {
        LOGE("Camera init failed with error 0x%x", ret);
        return -1;
    }

    sensor_t *s = esp_camera_sensor_get();

    s->set_brightness(s, 0);                 // -2 到 2
    s->set_contrast(s, 0);                   // -2 到 2
    s->set_saturation(s, 0);                 // -2 到 2
    s->set_special_effect(s, 0);             // 0 到 6 (0 - 无效果, 1 - 负片, 2 - 灰度, 3 - 红色调, 4 - 绿色调, 5 - 蓝色调, 6 - 怀旧)
    s->set_whitebal(s, 1);                   // 0 = 禁用 , 1 = 启用
    s->set_awb_gain(s, 1);                   // 0 = 禁用 , 1 = 启用
    s->set_wb_mode(s, 0);                    // 0 到 4 - 如果启用了 awb_gain (0 - 自动, 1 - 阳光, 2 - 阴天, 3 - 办公室, 4 - 家庭)
    s->set_exposure_ctrl(s, 1);              // 0 = 禁用 , 1 = 启用
    s->set_aec2(s, 0);                       // 0 = 禁用 , 1 = 启用
    s->set_ae_level(s, 0);                   // -2 到 2
    s->set_aec_value(s, 300);                // 0 到 1200
    s->set_gain_ctrl(s, 1);                  // 0 = 禁用 , 1 = 启用
    s->set_agc_gain(s, 0);                   // 0 到 30
    s->set_gainceiling(s, (gainceiling_t)0); // 0 到 6
    s->set_bpc(s, 0);                        // 0 = 禁用 , 1 = 启用
    s->set_wpc(s, 1);                        // 0 = 禁用 , 1 = 启用
    s->set_raw_gma(s, 1);                    // 0 = 禁用 , 1 = 启用
    s->set_lenc(s, 1);                       // 0 = 禁用 , 1 = 启用
    s->set_hmirror(s, 0);                    // 0 = 禁用 , 1 = 启用
    s->set_vflip(s, 0);                      // 0 = 禁用 , 1 = 启用
    s->set_dcw(s, 1);                        // 0 = 禁用 , 1 = 启用
    s->set_colorbar(s, 0);                   // 0 = 禁用 , 1 = 启用


    xTaskCreate(ecnu_camera_task, "ecnu_camera_task", 4096, NULL, 5, NULL);

    return 0;
}