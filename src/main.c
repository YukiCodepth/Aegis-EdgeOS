#include "../include/core.h"

// 1. The Boot Sequence (Phase 1)
void boot_sequence() {
    pid_t daemon_pid;

    printf("========================================\n");
    printf("[SYSTEM] Booting Aegis Core Controller...\n");
    
    // Spawn the background memory daemon
    daemon_pid = fork();
    
    if (daemon_pid == 0) {
        start_memory_monitor();
        exit(EXIT_SUCCESS); 
    } else if (daemon_pid > 0) {
        printf("[SYSTEM] Memory Daemon started in background (PID: %d)\n", daemon_pid);
    } else {
        printf("[ERROR] Failed to boot Memory Daemon.\n");
    }

    printf("[SYSTEM] Ready for telemetry and commands.\n");
    printf("========================================\n");
}

// 2. The Command Parser (Phase 1)
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

// 3. The Execution Engine (Phase 1)
int execute_command(char **args) {
    pid_t pid;
    int status;

    if (args[0] == NULL) {
        return 1;
    }

    pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("aegis"); 
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("aegis");
    } else {
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

// 4. The Core Loop (Phase 1 + Phase 2 Hardware & AI Bridge)
void core_loop() {
    char input_buffer[256]; 
    char **args;
    int running = 1;

    while (running) {
        printf("Aegis> ");
        
        if (fgets(input_buffer, sizeof(input_buffer), stdin) != NULL) {
            
            input_buffer[strcspn(input_buffer, "\n")] = 0;

            if (strcmp(input_buffer, "shutdown") == 0) {
                printf("[SYSTEM] Powering down Aegis Core...\n");
                running = 0;
                continue;
            }

            args = split_line(input_buffer);
            
            // Prevent crashes if the user just presses Enter
            if (args[0] == NULL) {
                free(args);
                continue;
            }

            // --- PHASE 2: HARDWARE INTERCEPTOR ---
            if (strcmp(args[0], "node") == 0) {
                if (args[1] != NULL && strcmp(args[1], "connect") == 0) {
                    if (args[2] != NULL && args[3] != NULL) {
                        int baud = atoi(args[3]);
                        init_serial(args[2], baud);
                    } else {
                        printf("Usage: node connect <port> <baudrate>\n");
                    }
                } 
                else if (args[1] != NULL && strcmp(args[1], "send") == 0) {
                    if (args[2] != NULL) {
                        send_serial(args[2]);
                    } else {
                        printf("Usage: node send <message>\n");
                    }
                } else {
                    printf("[NODE ERROR] Unknown node command.\n");
                }
                free(args);
                continue; 
            }

            // --- PHASE 2: AI INTERCEPTOR ---
            else if (strcmp(args[0], "ai") == 0) {
                if (args[1] != NULL && strcmp(args[1], "mode") == 0 && args[2] != NULL) {
                    if (strcmp(args[2], "local") == 0) set_ai_mode(1);
                    else if (strcmp(args[2], "cloud") == 0) set_ai_mode(0);
                    else printf("Usage: ai mode <local|cloud>\n");
                }
                else if (args[1] != NULL && strcmp(args[1], "intent") == 0) {
                    // Reconstruct the user's sentence from the array
                    char intent[256] = "";
                    for (int i = 2; args[i] != NULL; i++) {
                        strcat(intent, args[i]);
                        strcat(intent, " ");
                    }
                    process_intent(intent);
                } else {
                    printf("Commands:\n  ai mode <local|cloud>\n  ai intent <your sentence here>\n");
                }
                free(args);
                continue;
            }
            // ----------------------------------------

            execute_command(args);
            free(args);
        }
    }
}

// 5. System Initialization
int main() {
    boot_sequence();
    core_loop();
    return EXIT_SUCCESS;
}
