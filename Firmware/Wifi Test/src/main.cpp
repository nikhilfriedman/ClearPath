#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "nikhil";
const char* password = "secretpassword";

// Create an AsyncWebServer on port 80
AsyncWebServer server(80);
// Create a WebSocket endpoint at "/" (adjust if needed)
AsyncWebSocket ws("/");

// This function handles WebSocket events
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client connected: #%u\n", client->id());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client disconnected: #%u\n", client->id());
      break;
    case WS_EVT_DATA: {
      AwsFrameInfo * info = (AwsFrameInfo*)arg;
      if (info->opcode == WS_TEXT) {
        // Print the received text message
        Serial.printf("Received text: %.*s\n", len, data);
      }
      break;
    }
    default:
      break;
  }
}

void setup() {
  Serial.begin(115200);
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Attach our event handler to the WebSocket
  ws.onEvent(onWsEvent);
  // Add the WebSocket to the server
  server.addHandler(&ws);

  // Start the server
  server.begin();
  Serial.println("WebSocket server started.");
}

void loop() {
  // Clean up disconnected clients
  ws.cleanupClients();
  // Other periodic tasks can be added here if needed.
}
