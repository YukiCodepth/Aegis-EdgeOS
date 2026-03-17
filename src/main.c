#include "../include/core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> // Required for opening the serial port

// --- HARDWARE BRIDGE ---
int hardware_fd = -1; // Global connection to our virtual node

void send_serial(const char* command) {
    if (hardware_fd != -1) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s\n", command);
        write(hardware_fd, buffer, strlen(buffer));
        printf("[NODE TX] Transmitted: %s\n", command);
    } else {
        printf("[NODE ERROR] No hardware node connected! Use 'node connect <port> <baud>'\n");
    }
}
// -----------------------

int main() {
    printf("========================================\n");
    printf("[SYSTEM] Booting Aegis Core Controller...\n");
    
    // Simulate the background memory daemon from Phase 1
    printf("[SYSTEM] Memory Daemon started in background (PID: %d)\n", getpid() + 125);
    
    printf("[SYSTEM] Ready for telemetry and commands.\n");
    printf("========================================\n");

    // --- STAGE 6: BOOT THE HTTP NETWORK DAEMON ---
    start_network_server();
    // ---------------------------------------------

    char input[1024];

    // Main OS Command Loop
    while (1) {
        printf("Aegis> ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break; // Exit if user presses Ctrl+D
        }

        // Strip the trailing newline character
        input[strcspn(input, "\n")] = 0;

        // Ignore empty inputs
        if (strlen(input) == 0) {
            continue;
        }

        // --- COMMAND ROUTER ---
        if (strcmp(input, "exit") == 0 || strcmp(input, "shutdown") == 0) {
            printf("[SYSTEM] Powering down Aegis Core...\n");
            if (hardware_fd != -1) close(hardware_fd);
            break;
        } 
        else if (strncmp(input, "node connect ", 13) == 0) {
            char port[256];
            int baud;
            if (sscanf(input + 13, "%s %d", port, &baud) == 2) {
                // Close existing connection if one is already open
                if (hardware_fd != -1) close(hardware_fd);
                
                // Open the virtual/physical serial port
                hardware_fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);
                if (hardware_fd < 0) {
                    printf("[NODE ERROR] Failed to open port: %s\n", port);
                } else {
                    printf("[NODE SYSTEM] Linked to hardware node at %s (Baud: %d)\n", port, baud);
                }
            } else {
                printf("[ERROR] Usage: node connect <port> <baud>\n");
            }
        }
        else if (strncmp(input, "ai mode ", 8) == 0) {
            if (strstr(input, "local")) {
                set_ai_mode(1);
            } else if (strstr(input, "cloud")) {
                set_ai_mode(0);
            } else {
                printf("[ERROR] Unknown AI mode. Use 'cloud' or 'local'.\n");
            }
        }
        else if (strncmp(input, "ai intent ", 10) == 0) {
            process_intent(input + 10);
        }
        else {
            printf("[ERROR] Unknown command: %s\n", input);
        }
    }

    return 0;
}
