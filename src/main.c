#include "../include/core.h"

void boot_sequence() {
    pid_t daemon_pid;

    printf("========================================\n");
    printf("[SYSTEM] Booting Aegis Core Controller...\n");
    
    // Spawn the background daemon
    daemon_pid = fork();
    
    if (daemon_pid == 0) {
        // This is the cloned child process. It becomes the ghost.
        start_memory_monitor();
        exit(EXIT_SUCCESS); 
    } else if (daemon_pid > 0) {
        // Parent acknowledges the ghost
        printf("[SYSTEM] Memory Daemon started in background (PID: %d)\n", daemon_pid);
    } else {
        printf("[ERROR] Failed to boot Memory Daemon.\n");
    }

    printf("[SYSTEM] Ready for telemetry and commands.\n");
    printf("========================================\n");
}

// 1. The Parser
char **split_line(char *line) {
    int bufsize = AEGIS_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "aegis: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, AEGIS_TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;
        token = strtok(NULL, AEGIS_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

// 2. The Execution Engine
int execute_command(char **args) {
    pid_t pid;
    int status;

    if (args[0] == NULL) {
        return 1;
    }

    pid = fork();
    if (pid == 0) {
        // Child process: replace itself with the Linux command
        if (execvp(args[0], args) == -1) {
            perror("aegis"); 
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("aegis");
    } else {
        // Parent process (Aegis Core) waits for the command to finish
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

// 3. The Core Loop
void core_loop() {
    char input_buffer[256]; 
    char **args;
    int running = 1;

    while (running) {
        printf("Aegis> ");
        
        if (fgets(input_buffer, sizeof(input_buffer), stdin) != NULL) {
            
            // Remove the newline character
            input_buffer[strcspn(input_buffer, "\n")] = 0;

            if (strcmp(input_buffer, "shutdown") == 0) {
                printf("[SYSTEM] Powering down Aegis Core...\n");
                running = 0;
                continue;
            }

            args = split_line(input_buffer);
            execute_command(args);
            free(args);
        }
    }
}

int main() {
    boot_sequence();
    core_loop();
    return EXIT_SUCCESS;
}
