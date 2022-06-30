from shared import SET_commands
import json
import socket
import time


def comm(sock, manager, conn_page, core, address, port):
    global SET_commands
    i = 0
    key = -1
    sock.settimeout(5)
    while True:
        if not manager.template:
            cmd = {'CMD': 'TEMP', 'log': {'id': i}}
            if key != -1:
                cmd['log']['key'] = key
            message = json.dumps(cmd).encode()
            i += 1
        elif SET_commands:
            cmd = SET_commands.pop(0)
            cmd['log'] = {'id': i}
            i += 1
            message = json.dumps(cmd).encode()
            if cmd['CMD'] == 'QUIT':
                core.destroy()
                print("Sending: ", message)
                sock.send(message)
                if cmd['CMD'] == 'QUIT':
                    sock.shutdown(socket.SHUT_RDWR)
                    sock.close()
                    return
        else:
            message = json.dumps({'CMD': 'UPDT', 'log': {'id': i}}).encode()
            i += 1
            
        print("Sending: ", message)
        sock.send(message)
        try:
            length = ''
            received = ''
            while received != ':':
                if received.isdigit():
                    length += received
                received = sock.recv(1).decode('utf-8')
                print(received, end='')
            head_len = len(length) + 1
            length = int(length)
            receive_size = min(length, 2048 - head_len)
            buffer = sock.recv(receive_size).decode("utf-8")
            print(buffer, end='')
            length -= receive_size
            while length:
                receive_size = min(2048, length)
                buffer += sock.recv(receive_size).decode('utf-8')
                print(buffer[-receive_size:], end='')
                length -= receive_size
            print()
            
        except (socket.timeout, ConnectionResetError) as err:
            print(repr(err))
            print("arduino hasn't responded for long time")
            manager.template = False
            sock.shutdown(socket.SHUT_RDWR)
            sock.close()
            sock = reconnect(address, port)
            continue

        try:
            reply = json.loads(buffer)
        except json.decoder.JSONDecodeError:
            print("Wrong buffer: ", buffer)
            sock.shutdown(socket.SHUT_RDWR)
            sock.close()
            manager.template = False
            print("trying to reconnect")
            sock = reconnect(address, port)
            continue

        if reply['STATUS'] == 'ERROR':
            print('ERROR:', reply['ERROR'])
        elif reply['STATUS'] == 'TEMPLATE':
            if 'log' in reply:
                if 'key' in reply['log']:
                    key = reply['log']['key']
            if conn_page.display:
                conn_page.destroy()
                manager.grid()
            manager.new(reply)
            
        elif reply['STATUS'] == 'UPDATE':
            manager.update(reply)
        time.sleep(0.5)


def reconnect(address, port):
    attempts = 0
    while True:
        time.sleep(1)
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.connect((address, port))
            sock.settimeout(5)
            print("Connected")
            return sock
        except OSError as e:
            attempts += 1
            print("No host")
            if attempts < 10:
                continue
            raise e
