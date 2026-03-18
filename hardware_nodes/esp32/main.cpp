#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// --- NETWORK CONFIGURATION ---
const char* ssid = "Yukkinofi";
const char* password = "komachan";

// Replace with your laptop's IP address (keep the :8080/node_config.json)
const char* aegis_server = "http://192.168.64.17:8080/node_config.json"; 
// -----------------------------

const int LED_PIN = 2; // The standard onboard blue LED for most ESP32s

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.println("\n=================================");
    Serial.println("[NODE] Booting Aegis Edge Node...");
    Serial.println("=================================");

    // Connect to Wi-Fi
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

            // Parse the JSON payload
            JsonDocument doc; 
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                const char* target = doc["target_node"];
                
                // If Aegis Core sent a command meant for us
                if (strcmp(target, "ESP32") == 0) {
                    Serial.println("[SYSTEM] Command matched! Firing hardware relay (LED)...");
                    digitalWrite(LED_PIN, HIGH);
                    delay(1000); // Keep LED on for 1 second
                    digitalWrite(LED_PIN, LOW);
                }
            } else {
                Serial.println("[ERROR] JSON Parsing failed.");
            }
        } else {
            Serial.printf("[ERROR] HTTP GET failed. Is Aegis Core running? Error code: %d\n", httpResponseCode);
        }
        http.end();
    }
    
    // Wait 5 seconds before asking Aegis for updates again
    delay(5000); 
}
