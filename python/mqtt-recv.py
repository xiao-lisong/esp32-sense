import paho.mqtt.client as mqtt

# 连接到 EMQX 服务器
broker = "www.xiaolisong.asia"  # EMQX 服务器的 IP 地址
port = 1883  # MQTT 默认端口
topic = "test/topic"  # 订阅的主题

# MQTT 客户端回调函数
def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    # 连接成功后订阅主题
    client.subscribe(topic)

def on_message(client, userdata, msg):
    print(f"Message received on topic {msg.topic}: {msg.payload.decode()}")

# 创建 MQTT 客户端实例
client = mqtt.Client()

# 设置回调函数
client.on_connect = on_connect
client.on_message = on_message

# 连接到 EMQX Broker
client.connect(broker, port, 60)

# 循环等待消息
client.loop_forever()
