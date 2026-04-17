import socket

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

hostname = socket.gethostname()
ip = socket.gethostbyname(hostname)
port = 6666

client.connect((ip, port))

message = f"Client IP: {ip}, Device Name: {hostname}"
client.send(message.encode('utf-8'))

client.close()