#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// --- UPDATE THESE TWO LINES WITH YOUR HOTSPOT INFO ---
const char* ssid = "Yukkifi";
const char* password = "komachan";

// Your Asahi Mac IP Address
const char* aegis_server = "http://10.211.164.197:8080/node_config.json"; 

const int LED_PIN = 4; // Standard onboard blue LED

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.println("\n=================================");
    Serial.println("[NODE] Booting Aegis Edge Node...");
    Serial.println("=================================");

    WiFi.begin(ssid, password);
    Serial.print("[NODE] Connecting to Wi-Fi");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\n[NODE] Connected successfully!");
    Serial.print("[NODE] Local IP: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        Serial.printf("\n[NETWORK] Pinging Aegis Core at %s...\n", aegis_server);
        
        http.begin(aegis_server);
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
            String payload = http.getString();
            Serial.println("[NETWORK] Payload Received:");
            Serial.println(payload);

            JsonDocument doc; 
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                const char* target = doc["target_node"];
                const char* command = doc["command"]; // Pull the new AI command field

                if (target && strcmp(target, "ESP32") == 0) {
                    
                    // --- THE AUTONOMOUS HARDWARE LOGIC ---
                    if (command && strcmp(command, "LED_ON") == 0) {
                        Serial.println("[SYSTEM] AI Command Executed: LIGHTS ON");
                        digitalWrite(LED_PIN, HIGH); 
                    } 
                    else if (command && strcmp(command, "LED_OFF") == 0) {
                        Serial.println("[SYSTEM] AI Command Executed: LIGHTS OFF");
                        digitalWrite(LED_PIN, LOW); 
                    }
                    else {
                        Serial.println("[SYSTEM] Awaiting valid AI command...");
                    }
                }
            }
        }
        http.end();
    }
    delay(5000); 
}
