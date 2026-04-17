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
        break
    
    try:
        hours = float(message)
        if hours <= 40:
            salary = hours * 200
        else:
            salary = 8000 + ((hours - 40) * 300)
        
        response = f"Calculated Salary: Tk {salary}"
    except ValueError:
        response = "Invalid input. Please enter a number."

    conn.send(response.encode('utf-8'))
conn.close()