import socket
import threading

def handle_client(conn, addr):
    print(f"A new client has connected: {addr}")
    while True:
        message = conn.recv(2048).decode('utf-8')
        if not message or message.lower() == 'end':
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
    
    print(f"Disconnecting {addr}")
    conn.close()

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host = socket.gethostbyname(socket.gethostname())
port = 6666
server.bind((host, port))
server.listen()
print("Server is listening for multiple connections...")

while True:
    conn, addr = server.accept()
    thread = threading.Thread(target=handle_client, args=(conn, addr))
    thread.start()