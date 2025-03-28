#include <SPI.h>
#include <LoRa.h>

// LoRa module pins
#define LORA_MISO 19
#define LORA_MOSI 23
#define LORA_SCK  18
#define LORA_CS   5
#define LORA_RST  14

void setup() {
    Serial.begin(115200);
    while (!Serial);

    // SPI and LoRa Initialization
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
    LoRa.setPins(LORA_CS, LORA_RST, -1);  // Disable IRQ pin (set to -1)

    if (!LoRa.begin(915E6)) {  // Set frequency (adjust for your region)
        Serial.println("LoRa initialization failed!");
        while (1);
    }
    LoRa.setSpreadingFactor(7);
    LoRa.setSignalBandwidth(125E3);
    LoRa.setCodingRate4(5);
    LoRa.disableCrc();

    Serial.println("LoRa Receiver Initialized Successfully!");
}

void loop() {
    int packetSize = LoRa.parsePacket(); // Check if a packet was received
    if (packetSize) { // If there's a packet
        String received = "";
        while (LoRa.available()) {
            received += (char)LoRa.read();
        }
        Serial.println("Received: " + received);
    }
}
