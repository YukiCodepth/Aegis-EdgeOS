#include "../include/core.h"

void boot_sequence() {
    printf("========================================\n");
    printf("[SYSTEM] Booting Aegis Core Controller...\n");
    printf("[SYSTEM] Memory allocated.\n");
    printf("[SYSTEM] Ready for telemetry and commands.\n");
    printf("========================================\n");
}

void core_loop() {
    char input_buffer[256]; // Allocate 256 bytes for user input
    int running = 1;

    while (running) {
        printf("Aegis> ");
        
        // Read input securely
        if (fgets(input_buffer, sizeof(input_buffer), stdin) != NULL) {
            
            // Strip the "Enter" (newline) key from the end of the string
            input_buffer[strcspn(input_buffer, "\n")] = 0;

            // Check for exit command
            if (strcmp(input_buffer, "shutdown") == 0) {
                printf("[SYSTEM] Powering down Aegis Core...\n");
                running = 0;
            } else if (strlen(input_buffer) > 0) {
                printf("[ECHO] Unrecognized command: %s\n", input_buffer);
            }
        }
    }
}

int main() {
    boot_sequence();
    core_loop();
    return EXIT_SUCCESS;
}
