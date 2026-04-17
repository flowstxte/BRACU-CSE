import socket

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host = socket.gethostbyname(socket.gethostname())
port = 6666
server.bind((host, port))
server.listen()
print("Server is listening...")

conn, addr = server.accept()
print(f"Connected to {addr}")

while True:
    message = conn.recv(2048).decode('utf-8')
    if not message or message.lower() == 'end':
        print("Disconnecting client.")
        break
    
    vowels = "aeiouAEIOU"
    count = sum(1 for char in message if char in vowels)
    
    if count == 0:
        response = "Not enough vowels"
    elif count <= 2:
        response = "Enough vowels I guess"
    else:
        response = "Too many vowels"
        
    conn.send(response.encode('utf-8'))

conn.close()
