import paho.mqtt.client as mqtt

broker = "www.xiaolisong.asia"
port = 1883
topic = "test/topic"
message = "Hello, MQTT!"

# 创建客户端实例
client = mqtt.Client()

# 连接到 broker
client.connect(broker, port, 60)

# 发布消息
client.publish(topic, message)

# 断开连接
client.disconnect()
