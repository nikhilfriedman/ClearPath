#include <SPI.h>
#include <LoRa.h>

// LoRa module pins
#define LORA_MISO 19
#define LORA_MOSI 23
#define LORA_SCK  18
#define LORA_CS   5
#define LORA_RST  14
#define LORA_IRQ  2

void setup() {
    Serial.begin(115200);
    while (!Serial);

    // SPI and LoRa Initialization
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
    LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);

    if (!LoRa.begin(915E6)) {  // Set frequency (adjust for your region)
        Serial.println("LoRa initialization failed!");
        while (1);
    }
  
    // Configure LoRa parameters
    LoRa.setSpreadingFactor(7);
    LoRa.setSignalBandwidth(125E3);
    LoRa.setCodingRate4(5);
    LoRa.setTxPower(14);
    LoRa.disableCrc();
  
    Serial.println("LoRa Initialized Successfully!");
}

void loop() {
    // Send a message
    LoRa.beginPacket();
    LoRa.print("Hello LoRa");
    Serial.print("Sending packet: Hello LoRa\n");
    LoRa.endPacket();
  
    delay(1000);
}
