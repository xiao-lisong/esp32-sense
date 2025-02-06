import paho.mqtt.client as mqtt
import base64
import json
from datetime import datetime
import os
# EMQX Broker 配置
broker = "www.xiaolisong.asia"  # EMQX 服务器的 IP 地址
port = 1883  # MQTT 默认端口
topic = "test/topic"  # 订阅的主题


# MQTT 客户端回调函数
def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    # 连接成功后订阅主题
    client.subscribe(topic)

def on_message(client, userdata, msg):
    try:
        # 解析消息
        payload = msg.payload.decode()
        # print(f"Message received on topic {msg.topic}: {payload}")

        # 解析 JSON 数据
        data = json.loads(payload)

        # 提取协议字段
        device_id = data.get("device_id", "unknown_device")
        timestamp = data.get("timestamp", 0)
        data_type = data.get("data_type", "")
        image_info = data.get("payload", {}).get("image", {})
        timestamp_s = timestamp / 1000
        print(f"Device ID: {device_id}, Timestamp: {timestamp}, Data Type: {data_type}")
        if data_type == "image" and "data" in image_info:
            # 解码 Base64 图像数据
            image_data = base64.b64decode(image_info["data"])

            # 保存图像到文件
            timestamp_str = datetime.fromtimestamp(timestamp_s).strftime("%Y%m%d_%H%M%S") + f".{timestamp % 1000:03d}"
            filename = f"images/{device_id}_{timestamp_str}.jpeg"
            with open(filename, "wb") as image_file:
                image_file.write(image_data)

            print(f"Image saved as {filename}")
        else:
            print("No valid image data in message.")
    except Exception as e:
        print(f"Error processing message: {e}")

if __name__ == '__main__':
    os.makedirs("images", exist_ok=True)

    # 创建 MQTT 客户端实例
    client = mqtt.Client()

    # 设置回调函数
    client.on_connect = on_connect
    client.on_message = on_message

    # 连接到 EMQX Broker
    client.connect(broker, port, 60)

    # 循环等待消息
    client.loop_forever()
