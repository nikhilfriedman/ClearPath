#include <stdio.h>
#include <string.h>
#include "driver/spi_master.h"
#include "sx127x.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LORA_CS    GPIO_NUM_5   // Chip Select
#define LORA_MOSI  GPIO_NUM_23  // MOSI
#define LORA_MISO  GPIO_NUM_19  // MISO
#define LORA_SCK   GPIO_NUM_18  // SCK
#define LORA_RST   GPIO_NUM_14  // Reset (Optional)
#define LORA_DIO0  GPIO_NUM_26  // Interrupt Pin (Optional)

spi_device_handle_t spi_handle;
sx127x device;

// Function to initialize SX1278 LoRa module
void setup_lora() {
    spi_bus_config_t bus_config = {
        .mosi_io_num = LORA_MOSI,
        .miso_io_num = LORA_MISO,
        .sclk_io_num = LORA_SCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 0
    };

    spi_device_interface_config_t dev_config = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 0,  // SPI Mode 0 for SX1278
        .clock_speed_hz = 1 * 1000 * 1000, // 1 MHz
        .spics_io_num = LORA_CS,
        .queue_size = 1
    };

    // Initialize VSPI
    spi_bus_initialize(VSPI_HOST, &bus_config, SPI_DMA_CH_AUTO);
    spi_bus_add_device(VSPI_HOST, &dev_config, &spi_handle);

    // Create SX1278 device
    sx127x_create(&spi_handle, &device);

    // Configure LoRa settings
    sx127x_set_frequency(915E6, &device);
    sx127x_lora_set_spreading_factor(&device, 7);
    sx127x_lora_set_bandwidth(&device, 125E3);
    sx127x_lora_set_coding_rate(&device, 5);
    sx127x_set_tx_power(14, &device);
    sx127x_set_preamble_length(8, &device);
}

// Function to send LoRa message
void send_lora_message(const char *message) {
    printf("Sending LoRa Message: %s\n", message);
    sx127x_write(&device, (uint8_t *)message, strlen(message));
}

// FreeRTOS task for sending messages every 5 seconds
void lora_task(void *pvParameters) {
    while (1) {
        send_lora_message("Hello from ESP32 LoRa!");
        vTaskDelay(pdMS_TO_TICKS(5000));  // Delay for 5 seconds
    }
}

// Main application entry point
void app_main() {
    printf("Initializing LoRa Module...\n");
    setup_lora();

    // Create FreeRTOS task for LoRa transmission
    xTaskCreate(lora_task, "lora_task", 4096, NULL, 5, NULL);
}
