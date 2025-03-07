#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "sx127x.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "LoRa_TX"

// Define SPI and GPIO pins
#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  18
#define PIN_NUM_CS   5
#define PIN_NUM_RST  14
#define PIN_NUM_DIO0 26

// Vehicle ID message
#define VEHICLE_ID "EV1234"

// LoRa device instance
sx127x lora_dev;
bool lora_initialized = false;  // Flag to check if LoRa is available

// LoRa initialization function
void init_lora()
{
    esp_err_t err;
    ESP_LOGI(TAG, "Initializing SPI bus...");
    
    spi_bus_config_t bus_config = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096
    };

    err = spi_bus_initialize(SPI2_HOST, &bus_config, SPI_DMA_CH_AUTO);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus: %s", esp_err_to_name(err));
        return;
    }

    ESP_LOGI(TAG, "Configuring SPI device...");
    spi_device_interface_config_t dev_cfg = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 0, // SPI Mode 0
        .clock_speed_hz = 1000000, // 1 MHz
        .spics_io_num = PIN_NUM_CS,
        .queue_size = 1
    };

    spi_device_handle_t spi_handle;
    err = spi_bus_add_device(SPI2_HOST, &dev_cfg, &spi_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add SPI device: %s", esp_err_to_name(err));
        return;
    }

    ESP_LOGI(TAG, "Creating LoRa device with SPI handle: %p", (void *)spi_handle);
    
    err = sx127x_create((void *)spi_handle, &lora_dev);
    if (err != SX127X_OK) {
        ESP_LOGE(TAG, "LoRa device creation failed! Error: %d", err);
        ESP_LOGE(TAG, "No LoRa module detected. Skipping further initialization.");
        return;
    }

    ESP_LOGI(TAG, "Configuring LoRa parameters...");
    sx127x_set_frequency(915E6, &lora_dev);
    sx127x_lora_set_bandwidth(SX127x_BW_125000, &lora_dev);
    sx127x_lora_set_modem_config_2(SX127x_SF_7, &lora_dev);
    sx127x_lora_set_implicit_header(NULL, &lora_dev);
    sx127x_tx_set_pa_config(SX127x_PA_PIN_BOOST, 17, &lora_dev);

    ESP_LOGI(TAG, "LoRa initialized successfully.");
    lora_initialized = true;
}

// Function to transmit a message
void send_message()
{
    char message[32];
    snprintf(message, sizeof(message), "Vehicle ID: %s", VEHICLE_ID);

    while (1)
    {
        if (!lora_initialized) {
            ESP_LOGW(TAG, "LoRa module not initialized! Skipping transmission.");
            vTaskDelay(pdMS_TO_TICKS(5000));
            continue;
        }

        ESP_LOGI(TAG, "Transmitting: %s", message);
        int err = sx127x_lora_tx_set_for_transmission((uint8_t *)message, strlen(message), &lora_dev);
        
        if (err == SX127X_OK) {
            ESP_LOGI(TAG, "Message sent successfully.");
        } else {
            ESP_LOGE(TAG, "Failed to send message: %d", err);
        }

        vTaskDelay(pdMS_TO_TICKS(5000)); // Transmit every 5 seconds
    }
}

// Main function
void app_main()
{
    vTaskDelay(pdMS_TO_TICKS(5000)); // 5000 ms = 5 seconds

    ESP_LOGI(TAG, "Starting ESP32 LoRa transmitter...");
    
    ESP_LOGI(TAG, "Calling init_lora()...");
    init_lora();

    ESP_LOGI(TAG, "Calling send_message()...");
    send_message();
}
