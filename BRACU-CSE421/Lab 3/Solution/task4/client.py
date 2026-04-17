import socket

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host = socket.gethostbyname(socket.gethostname())
port = 6666
client.connect((host, port))

while True:
    msg = input("Enter hours worked (type 'end' to quit): ")
    client.send(msg.encode('utf-8'))
    
    if msg.lower() == 'end':
        break
        
    reply = client.recv(2048).decode('utf-8')
    print(f"Reply from Server: {reply}")

client.close()