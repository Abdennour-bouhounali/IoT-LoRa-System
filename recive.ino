#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

const char *ssid = "TP-LINK_B29EDB";
const char *password = "Dz47+@+@+@47";
const char *apiEndpoint = "http://agritech-dz.com/api/send";

void setup() {
    Serial.begin(115200);

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
}

void loop() {
    // Create JSON payload
    String payload = "";

    // Make HTTP POST request to the Laravel API
    HTTPClient http;
    http.begin(apiEndpoint);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-API-SECRET", "sOkd5mW8cs5Xw7eRbyrUcG0yi6k2nzMghdNbfCHpiXALeApU6LDAx4kn9iR9Lid6");
    http.addHeader("X-API-KEY", "8Ft7AVb17uKFcv252jHpQYwtBOZsnS9a");

    int httpCode = http.POST(payload);

    if (httpCode > 0) {
        Serial.println(httpCode);
        String response = http.getString();
        JSONVar parsedData = JSON.parse(response);
        JSONVar toggleButtons = JSON.parse(parsedData["data"]);
        
        // Loop through each ToggleButton and print values
        for (int i = 0; i < toggleButtons.length(); i++) {
          JSONVar toggleButton = toggleButtons[i];
          
          Serial.print("Greenhouse ID: ");
          Serial.println(toggleButton["greenhouse_id"]);
          Serial.print("Actuator: ");
          Serial.println(toggleButton["actuator"]);
          Serial.print("Is Active: ");
          Serial.println(toggleButton["is_active"]);
          Serial.println("-------------------------");
        }
    } else {
        Serial.println("HTTP request failed");
    }

    http.end();

    // Delay for a certain period before sending the next request
    delay(60000); // 60 seconds
}
