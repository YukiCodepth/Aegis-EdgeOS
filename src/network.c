#include "../include/core.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define PORT 8080
#define PUBLIC_DIR "./public"

void *network_daemon(void *arg) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("[NETWORK ERROR] Socket failed");
        return NULL;
    }

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

    printf("[NETWORK] HTTP Web Server running on port %d...\n", PORT);

    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            continue;
        }

        char buffer[2048] = {0};
        read(new_socket, buffer, 2048);

        if (strncmp(buffer, "GET ", 4) == 0) {
            char filepath[256];
            char *file_start = buffer + 4;
            char *file_end = strchr(file_start, ' ');
            
            if (file_end != NULL) {
                *file_end = '\0';
                
                // --- UPGRADE: Default to the Dashboard HTML ---
                if (strcmp(file_start, "/") == 0) {
                    strcpy(file_start, "/index.html"); 
                }
                snprintf(filepath, sizeof(filepath), "%s%s", PUBLIC_DIR, file_start);

                FILE *file = fopen(filepath, "r");
                if (file) {
                    // --- UPGRADE: Dynamic MIME Types ---
                    const char *content_type = "text/plain";
                    if (strstr(filepath, ".html")) content_type = "text/html";
                    else if (strstr(filepath, ".json")) content_type = "application/json";
                    else if (strstr(filepath, ".css")) content_type = "text/css";

                    char http_header[512];
                    snprintf(http_header, sizeof(http_header), 
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: %s\r\n"
                        "Access-Control-Allow-Origin: *\r\n\r\n", content_type);
                    
                    write(new_socket, http_header, strlen(http_header));

                    char file_buf[1024];
                    int bytes_read;
                    while ((bytes_read = fread(file_buf, 1, sizeof(file_buf), file)) > 0) {
                        write(new_socket, file_buf, bytes_read);
                    }
                    fclose(file);
                    
                    // Only print to terminal if it's the JSON payload (to avoid log spam from the HTML UI)
                    if (strstr(filepath, ".json")) {
                       // printf("\n[NETWORK] Served payload to edge node.\nAegis> ", filepath);
                       // fflush(stdout);
                    }
                } else {
                    char not_found[] = "HTTP/1.1 404 Not Found\r\n\r\n404 - File Not Found";
                    write(new_socket, not_found, strlen(not_found));
                }
            }
        }
        close(new_socket);
    }
    return NULL;
}

void start_network_server() {
    pthread_t thread_id;
    if(pthread_create(&thread_id, NULL, network_daemon, NULL) != 0) {
        printf("[NETWORK ERROR] Failed to start HTTP daemon thread.\n");
    }
}
