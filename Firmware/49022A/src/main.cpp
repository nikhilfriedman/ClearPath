#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Aashi's iPhone (2)";
const char* password = "a1b2c3d4";
const char* test_url = "http://example.com";

void setup() {
    Serial.begin(115200);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("\nWiFi Connected!");

    Serial.print("Local IP Address: ");
    Serial.println(WiFi.localIP());

    HTTPClient http;
    http.begin(test_url);
    int httpCode = http.GET();
    if (httpCode > 0) {
        Serial.printf("HTTP Response Code: %d\n", httpCode);
        String payload = http.getString();
        Serial.println(payload);
    } else {
        Serial.println("HTTP Request failed!");
        Serial.printf("WiFi Status: %d\n", WiFi.status());
        Serial.printf("HTTP Error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}

void loop() {}
