#include <WiFi.h>
#include <ArduinoWebsockets.h>

const char* ssid = "Galaxy A713B09"; 
const char* password = "ttkf0302";
const char* websocket_server = "ws://192.168.61.152:8765";  // Update with your Mac's IP

using namespace websockets;
WebsocketsClient client;

void onMessageCallback(WebsocketsMessage message) {
    Serial.println("Received message: " + message.data());
}

void setup() {
    Serial.begin(115200);
    
    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to Wi-Fi...");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("\nConnected to Wi-Fi!");

    // Connect to WebSocket server
    if (client.connect(websocket_server)) {
        Serial.println("Connected to WebSocket server!");
        client.onMessage(onMessageCallback);
    } else {
        Serial.println("WebSocket connection failed.");
    }
}

void loop() {
    if (client.available()) {
        client.poll();  // Check for new messages
    }

    // Send a message every 5 seconds
    static unsigned long lastSendTime = 0;
    if (millis() - lastSendTime > 5000) {
        lastSendTime = millis();
        client.send("Hello from ESP32!");
        Serial.println("Message sent to WebSocket server.");
    }

    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(1000);
    }
    // Serial.println("\nConnected to Wi-Fi!");
}
