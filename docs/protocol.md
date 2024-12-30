
数据传输协议
```
{
  "device_id": "device123",           // 设备唯一标识符
  "timestamp": 1672444800000,         // 采集时间戳 (毫秒)
  "data_type": "image",               // 数据类型: "image", "audio" ...
  "payload": {
    "image": {                         // 图像数据结构
        ...
    }
  }
}

```
图像协议填充
```
    "image": {                         // 图像数据结构
      "format": "jpeg",                // 图像格式 (如 png, jpeg)
      "width": 1920,                   // 图像宽度
      "height": 1080,                  // 图像高度
      "data": "base64_encoded_string"  // 图像数据的 Base64 编码字符串
    }
```

音频协议填充
```
,
    "audio": {                         // 音频数据结构
      "format": "wav",                 // 音频格式 (如 mp3, wav)
      "duration": 120,                 // 音频时长 (秒)
      "data": "base64_encoded_string"  // 音频数据的 Base64 编码字符串
    }
```