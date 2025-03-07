#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include "driver/spi_slave.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Wi-Fi Credentials
const char* ssid = "Galaxy A713B09";
const char* password = "ttkf0302";

// WebSocket Server URL
using namespace websockets;
WebsocketsClient webSocket;
const char* websocket_server = "ws://192.168.122.152:8765";  // WebSocket server address

SemaphoreHandle_t vehicleIDMutex;

// Traffic Light GPIO Pins
#define RED_N 33
#define YELLOW_N 25
#define GREEN_N 26
#define RED_S 27
#define YELLOW_S 14
#define GREEN_S 13
#define RED_W 32
#define YELLOW_W 22
#define GREEN_W 21
#define RED_E 17
#define YELLOW_E 16
#define GREEN_E 4

// Unique Traffic Light ID
const char* TRAFFIC_LIGHT_ID = "TL_001";

// VSPI (Vehicle ID Transmission)
#define VSPI_MISO 19
#define VSPI_MOSI 23
#define VSPI_SCK 18
#define VSPI_CS 5

// Allocate DMA-capable buffer for SPI reception
uint8_t* receivedVehicleID;

bool vehicleDetected = false;
unsigned long lastUpdateTime = 0;
// SemaphoreHandle_t vehicleDetectedSemaphore;

void controlTrafficLight(String data) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, data);
    if (error) {
        Serial.println("Failed to parse JSON");
        return;
    }

    digitalWrite(RED_N, strcmp(doc["north"], "RED") == 0);
    digitalWrite(YELLOW_N, strcmp(doc["north"], "YELLOW") == 0);
    digitalWrite(GREEN_N, strcmp(doc["north"], "GREEN") == 0);

    digitalWrite(RED_S, strcmp(doc["south"], "RED") == 0);
    digitalWrite(YELLOW_S, strcmp(doc["south"], "YELLOW") == 0);
    digitalWrite(GREEN_S, strcmp(doc["south"], "GREEN") == 0);

    digitalWrite(RED_W, strcmp(doc["west"], "RED") == 0);
    digitalWrite(YELLOW_W, strcmp(doc["west"], "YELLOW") == 0);
    digitalWrite(GREEN_W, strcmp(doc["west"], "GREEN") == 0);

    digitalWrite(RED_E, strcmp(doc["east"], "RED") == 0);
    digitalWrite(YELLOW_E, strcmp(doc["east"], "YELLOW") == 0);
    digitalWrite(GREEN_E, strcmp(doc["east"], "GREEN") == 0);
}

void onMessageCallback(WebsocketsMessage message) {
    Serial.println("Received command: " + message.data());
    controlTrafficLight(message.data());
}

void readVehicleID(void* parameter) {
    while (1) {
        uint8_t tempVehicleID[64] = {0};  // Temporary buffer to avoid race conditions

        spi_slave_transaction_t trans = {
            .length = 64 * 8,
            .tx_buffer = NULL,
            .rx_buffer = tempVehicleID
        };

        esp_err_t ret = spi_slave_transmit(VSPI_HOST, &trans, portMAX_DELAY);

        if (ret == ESP_OK && tempVehicleID[0] != '\0') {
            if (xSemaphoreTake(vehicleIDMutex, portMAX_DELAY) == pdTRUE) {
                memcpy(receivedVehicleID, tempVehicleID, 64); // Copy new data safely
                Serial.print("Received Vehicle ID: ");
                Serial.println((char*)receivedVehicleID);
                vehicleDetected = true;
                xSemaphoreGive(vehicleIDMutex); // Release mutex
            }
        } else {
            memset(receivedVehicleID, 0, 64);
            vehicleDetected = false;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void sendTrafficLightState(void* parameter) {
    while (1) {
        unsigned long currentTime = millis();
        if (currentTime - lastUpdateTime >= 1000) {
            String jsonPayload = "{";
            jsonPayload += "\"traffic_light_id\": \"" + String(TRAFFIC_LIGHT_ID) + "\",";
            jsonPayload += "\"north\": \"" + String(digitalRead(GREEN_N) ? "GREEN" : (digitalRead(YELLOW_N) ? "YELLOW" : "RED")) + "\",";
            jsonPayload += "\"south\": \"" + String(digitalRead(GREEN_S) ? "GREEN" : (digitalRead(YELLOW_S) ? "YELLOW" : "RED")) + "\",";
            jsonPayload += "\"west\": \"" + String(digitalRead(GREEN_W) ? "GREEN" : (digitalRead(YELLOW_W) ? "YELLOW" : "RED")) + "\",";
            jsonPayload += "\"east\": \"" + String(digitalRead(GREEN_E) ? "GREEN" : (digitalRead(YELLOW_E) ? "YELLOW" : "RED")) + "\"";

            char tempVehicleID[64] = "UNKNOWN";

            if (vehicleDetected) {
                if (xSemaphoreTake(vehicleIDMutex, portMAX_DELAY) == pdTRUE) {
                    strcpy(tempVehicleID, (char*)receivedVehicleID);
                    memset(receivedVehicleID, 0, 64);
                    xSemaphoreGive(vehicleIDMutex);
                }
            }

            jsonPayload += ", \"vehicle_id\": \"" + String(tempVehicleID) + "\"";
            jsonPayload += "}";

            Serial.println("Sending WebSocket message: " + jsonPayload);
            webSocket.send(jsonPayload);

            lastUpdateTime = currentTime;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(115200);

    vehicleIDMutex = xSemaphoreCreateMutex();
    if (vehicleIDMutex == NULL) {
        Serial.println("Failed to create mutex!");
        while (1);
    }

    receivedVehicleID = (uint8_t*) heap_caps_malloc(64, MALLOC_CAP_DMA);
    if (!receivedVehicleID) {
        Serial.println("Failed to allocate DMA memory for SPI RX buffer!");
        while (1);
    }
    memset(receivedVehicleID, '1', 64); // Fill the buffer with character '1'

    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");

    webSocket.onMessage(onMessageCallback);
    while (!webSocket.connect(websocket_server)) {
        delay(1000);
        Serial.println("connecting to websocket...");
    }
    Serial.println("\nconnected to websocket!");

    int pins[] = {RED_N, YELLOW_N, GREEN_N, RED_S, YELLOW_S, GREEN_S, RED_W, YELLOW_W, GREEN_W, RED_E, YELLOW_E, GREEN_E};
    for (int i = 0; i < 12; i++) {
        pinMode(pins[i], OUTPUT);
        digitalWrite(pins[i], LOW);
    }
    digitalWrite(YELLOW_N, HIGH);  
    digitalWrite(YELLOW_S, HIGH);
    digitalWrite(RED_W, HIGH);
    digitalWrite(RED_E, HIGH);

    spi_bus_config_t buscfg = {
        .mosi_io_num = VSPI_MOSI,
        .miso_io_num = VSPI_MISO,
        .sclk_io_num = VSPI_SCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 64
    };

    spi_slave_interface_config_t slvcfg = {
        .spics_io_num = VSPI_CS,
        .flags = 0,
        .queue_size = 1,
        .mode = 0
    };

    esp_err_t ret = spi_slave_initialize(VSPI_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        Serial.println("SPI Slave Init Failed!");
        while (1);
    }
    
    // Create tasks
    xTaskCreate(readVehicleID, "Read SPI", 2048, NULL, 1, NULL);
    xTaskCreate(sendTrafficLightState, "Send Traffic Light State", 2048, NULL, 1, NULL);
}

void loop() {
    if (webSocket.available()) {
        webSocket.poll();
    }
    else{
        while (!webSocket.connect(websocket_server)) {
            delay(1000);
            Serial.println("connecting to websocket...");
        }
        Serial.println("\nconnected to websocket!");
    }
}
