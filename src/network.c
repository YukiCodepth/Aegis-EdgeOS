#include "../include/core.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>

#define PORT 8080
#define PUBLIC_DIR "./public"

// The background daemon that listens for Wi-Fi requests
void *network_daemon(void *arg) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // 1. Open the Socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("[NETWORK ERROR] Socket failed");
        return NULL;
    }

    // 2. Bind it to Port 8080
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("[NETWORK ERROR] Bind failed");
        return NULL;
    }

    if (listen(server_fd, 3) < 0) {
        perror("[NETWORK ERROR] Listen failed");
        return NULL;
    }

    printf("[NETWORK] HTTP File Server running on port %d...\n", PORT);

    // 3. The Infinite Listening Loop
    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            continue;
        }

        char buffer[2048] = {0};
        read(new_socket, buffer, 2048);

        // 4. Very simple HTTP GET Parser
        if (strncmp(buffer, "GET ", 4) == 0) {
            char filepath[256];
            char *file_start = buffer + 4;
            char *file_end = strchr(file_start, ' ');
            
            if (file_end != NULL) {
                *file_end = '\0';
                
                // If they just ask for "/", give them the config
                if (strcmp(file_start, "/") == 0) {
                    strcpy(file_start, "/node_config.json"); 
                }
                snprintf(filepath, sizeof(filepath), "%s%s", PUBLIC_DIR, file_start);

                FILE *file = fopen(filepath, "r");
                if (file) {
                    // Send HTTP Success Header
                    char http_header[] = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
                    write(new_socket, http_header, strlen(http_header));

                    // Stream the file bytes over the network
                    char file_buf[1024];
                    int bytes_read;
                    while ((bytes_read = fread(file_buf, 1, sizeof(file_buf), file)) > 0) {
                        write(new_socket, file_buf, bytes_read);
                    }
                    fclose(file);
                    printf("\n[NETWORK] Served %s to remote node.\nAegis> ", filepath);
                    fflush(stdout); // Force prompt to reprint
                } else {
                    // File doesn't exist
                    char not_found[] = "HTTP/1.1 404 Not Found\r\n\r\n404 - File Not Found";
                    write(new_socket, not_found, strlen(not_found));
                    printf("\n[NETWORK] 404 Error - Node requested missing file: %s\nAegis> ", filepath);
                    fflush(stdout);
                }
            }
        }
        close(new_socket); // Hang up the "phone"
    }
    return NULL;
}

// Function to spin up the daemon in a background thread
void start_network_server() {
    pthread_t thread_id;
    if(pthread_create(&thread_id, NULL, network_daemon, NULL) != 0) {
        printf("[NETWORK ERROR] Failed to start HTTP daemon thread.\n");
    }
}
