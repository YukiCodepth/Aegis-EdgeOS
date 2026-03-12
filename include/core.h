#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define AEGIS_TOK_BUFSIZE 64
#define AEGIS_TOK_DELIM " \t\r\n\a"

// Function prototypes
void boot_sequence();
void core_loop();
char **split_line(char *line);
int execute_command(char **args);
void start_memory_monitor(); // Our new daemon prototype

#endif
