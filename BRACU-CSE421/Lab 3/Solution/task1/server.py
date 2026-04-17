import socket

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

host = socket.gethostbyname(socket.gethostname())
port = 6666
server.bind((host, port))

server.listen()
print(f"Server is listening on {host}:{port}")

conn, addr = server.accept()
print(f"A new client has connected from: {addr}")

message = conn.recv(2048).decode('utf-8')
print(f"Message from Client: {message}")

conn.close()