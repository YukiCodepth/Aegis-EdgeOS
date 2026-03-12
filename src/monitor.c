#include "../include/core.h"

void start_memory_monitor() {
    FILE *sys_mem;
    FILE *log_file;
    char buffer[256];

    // Open our background log file in "append" mode
    log_file = fopen("logs/memory.log", "a");
    if (!log_file) {
        perror("[AEGIS ERROR] Could not open logs/memory.log. Does the logs folder exist?");
        exit(EXIT_FAILURE);
    }

    fprintf(log_file, "--- AEGIS MEMORY DAEMON STARTED ---\n");
    fflush(log_file);

    // The infinite background loop
    while(1) {
        sys_mem = fopen("/proc/meminfo", "r");
        if (sys_mem != NULL) {
            
            while (fgets(buffer, sizeof(buffer), sys_mem)) {
                if (strncmp(buffer, "MemAvailable:", 13) == 0) {
                    fprintf(log_file, "[AEGIS HEALTH] %s", buffer);
                    fflush(log_file); // Force write to disk
                    break;
                }
            }
            fclose(sys_mem);
        }
        
        // Sleep for 5 seconds so we don't fry the CPU
        sleep(5); 
    }
}
