#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
//
//const char *ssid = "TP-LINK_B29EDB";
//const char *password = "Dz47+@+@+@47";
const char *apiEndpoint = "http://agritech-dz.com/api/store";


 const char *ssid = "abdou";
 const char *password = "0123456789";
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
    String payload = "{\"temperature\": 10, \"humidity\": 0.33, \"co2\": 350, \"soil_humidity\": 0.3, \"wind_velocity\": 11, \"irradiation\": 0, \"acquisition_time\": \"2022-02-18 09:34:56\", \"greenhouse_name\": \"Greenhouse1\"}";

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

          Serial.println(response);
          Serial.println("-------------------------");
        
    } else {
        Serial.println("HTTP request failed");
    }

    http.end();

    // Delay for a certain period before sending the next request
    delay(60000); // 60 seconds
}
