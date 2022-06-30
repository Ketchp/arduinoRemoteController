import socket
import time
import json

address = '192.168.50.199'

port = 9874

print(time.time())
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
print(time.time())
s.connect((address, port))
print(time.time())
x = json.dumps({"CMD": "TEMP", "AUTH": 6969})

while x:
    s.send(x.encode())
    received = s.recv(1).decode('utf-8')
    while received not in ('{', '['):
        received = s.recv(1).decode('ascii')
    print(received, end='')
    buffer = received
    stack = 1
    while stack:
        received = s.recv(1).decode('utf-8')
        print(received, end='')
        if received in ('{', '['):
            stack += 1
        if received in ('}', ']'):
            stack -= 1
        buffer += received
    x = input("\n>")
time.sleep(1)
s.close()
