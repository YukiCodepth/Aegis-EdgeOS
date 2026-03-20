#include "../include/core.h"
#include <stdlib.h> // Required for reading environment variables
#include <string.h>
#include <stdio.h>

void update_hardware_payload(const char* command) {
    // Open the JSON file in "w" (write) mode, which completely overwrites the old data
    FILE *file = fopen("./public/node_config.json", "w");
    if (file) {
        fprintf(file, "{\n    \"system\": \"Aegis Core\",\n    \"status\": \"Online\",\n    \"target_node\": \"ESP32\",\n    \"command\": \"%s\"\n}\n", command);
        fclose(file);
        printf("[SYSTEM] Web Payload Updated -> Broadcast Command: %s\n", command);
    } else {
        printf("[ERROR] File I/O Failed. Cannot update public/node_config.json\n");
    }
}

int ai_mode = 0;

void set_ai_mode(int mode) {
    ai_mode = mode;
    if (ai_mode == 1) {
        printf("[AI ENGINE] Mode switched to LOCAL (Ollama on port 11434).\n");
    } else {
        printf("[AI ENGINE] Mode switched to CLOUD API.\n");
    }
}

void process_intent(const char* user_text) {
    char command[2048];
    char response[8192]; 
    FILE *fp;

    // --- CYBERSECURITY PATCH V2: ULTIMATE SANITIZATION ---
    char safe_text[256];
    strncpy(safe_text, user_text, sizeof(safe_text) - 1);
    safe_text[sizeof(safe_text) - 1] = '\0';

    char *p = safe_text;
    while (*p) {
        if (*p == '\'' || *p == '"' || *p == '\\') {
            *p = ' '; 
        }
        p++;
    }
    // -----------------------------------------------------

    printf("[AI ENGINE] Analyzing intent: '%s'\n", safe_text);

    if (ai_mode == 1) {
        snprintf(command, sizeof(command),
            "curl -s -X POST http://localhost:11434/api/generate -d '{\"model\": \"llama3\", \"stream\": false, \"prompt\": \"You are a hardware OS. The user says: %s. Reply with EXACTLY one of these words and nothing else: LED_ON, LED_OFF, RELAY_ON.\"}'", safe_text);
    } else {
        
        // --- SECURITY PATCH V3: SECURE API KEY FETCHING ---
        const char* CLOUD_API_KEY = getenv("GROQ_API_KEY");
        if (CLOUD_API_KEY == NULL) {
            printf("[AI ERROR] GROQ_API_KEY environment variable is missing!\n");
            printf("Please load your .env file before running Aegis.\n");
            return;
        }
        // --------------------------------------------------

        snprintf(command, sizeof(command),
            "curl -s -X POST https://api.groq.com/openai/v1/chat/completions "
            "-H \"Authorization: Bearer %s\" "
            "-H \"Content-Type: application/json\" "
            "-d '{\"model\": \"llama-3.3-70b-versatile\", \"messages\": [{\"role\": \"system\", \"content\": \"You are a hardware controller. Reply ONLY with the command: LED_ON, LED_OFF, or UNKNOWN.\"}, {\"role\": \"user\", \"content\": \"%s\"}]}'", CLOUD_API_KEY, safe_text);
    }

    fp = popen(command, "r");
    if (fp == NULL) {
        printf("[AI ERROR] Failed to open network pipe.\n");
        return;
    }

    response[0] = '\0';
    while (fgets(response + strlen(response), sizeof(response) - strlen(response), fp) != NULL) {}
    pclose(fp);

    char* content_start = NULL;
    if (ai_mode == 1) {
        content_start = strstr(response, "\"response\":\"");
        if (content_start) content_start += 12; 
    } else {
        content_start = strstr(response, "\"content\":\"");
        if (content_start) content_start += 11; 
    }

    if (content_start) {
        char* content_end = strchr(content_start, '"');
        if (content_end) {
            *content_end = '\0'; 
            printf("[AI DECISION] Hardware Command Extracted: %s\n", content_start);
            printf("[SYSTEM] Forwarding AI decision to Wireless Telemetry...\n");
            
            // --- THE AUTONOMOUS BRIDGE FIX ---
            update_hardware_payload(content_start); 
            // ---------------------------------
            
            return;
        }
    }

    printf("[AI ERROR] Could not parse the API response.\n");
}
