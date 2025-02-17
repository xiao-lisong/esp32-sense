import socket
import struct
import json
import base64
import threading
from datetime import datetime
import os
HOST = '0.0.0.0'  # 监听所有接口
PORT = 12345       # 端口号

def parser_json(json_obj):
    try:
        # 解析 JSON 数据
        data = json_obj

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

def handle_client(conn, addr):
    print(f"Connected by {addr}")
    buffer = b""
    
    while True:
        chunk = conn.recv(1024)  # 读取数据
        if not chunk:
            return None
        buffer += chunk
        
        try:
            # 在流式数据中查找 JSON 起点
            buffer_str = buffer.decode("utf-8", errors="ignore")
            decoder = json.JSONDecoder()

            while buffer_str:
                try:
                    obj, idx = decoder.raw_decode(buffer_str)  # 尝试解析 JSON
                    print("解析成功 JSON:", obj)
                    buffer_str = buffer_str[idx:]  # 移除已解析部分
                    buffer = buffer_str.encode("utf-8")  # 更新 buffer
                    #print("obj:", obj)
                    parser_json(obj)
                except json.JSONDecodeError:
                    # 解析失败，继续接收更多数据
                    break
        except Exception as e:
            print("错误:", e)

# 启动服务器
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind((HOST, PORT))
    s.listen()
    os.makedirs("images", exist_ok=True)
    print("Server started, waiting for connections...")
    while True:
        conn, addr = s.accept()
        client_thread = threading.Thread(target=handle_client, args=(conn, addr))
        client_thread.start()