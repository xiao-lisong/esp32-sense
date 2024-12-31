#ifndef ECNU_PACKEGET_H
#define ECNU_PACKEGET_H

typedef enum {
    ECNU_PACKEGE_ENCODE_IMAGE,
    ECNU_PACKEGE_ENCODE_AUDIO,
    ECNU_PACKEGE_ENCODE_MAX
} ecnu_packege_type;

typedef struct {
    int width;
    int height;
    char format[8];
    long long timestamp;
} ecnu_packege_extra_image;


/* 
    type: 编码类型
    input: 输入数据
    input_len: 输入数据长度
    output: 接收buffer
    output_len: buffer内存长度
    extra_data: 额外数据
*/

int ecnu_packege_encode(int type, const char * input, int input_len, char * output, int output_len, void * extra_data);

#endif