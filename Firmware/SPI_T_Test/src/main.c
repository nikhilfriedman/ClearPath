#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "esp_log.h"

#define VEHICLE_ID "123456"  // Define the vehicle ID as a constant
#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_SCLK 18
#define SPI_CS   5

static const char *TAG = "SPI_TEST";
spi_device_handle_t spi;

// Function to send data over SPI (simulating LoRa transmission)
void send_spi_message(const char *message) {
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 64;  // Length in bits
    t.tx_buffer = message;           // Data to send
    t.rx_buffer = NULL;              // No data received

    ret = spi_device_transmit(spi, &t);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "SPI TX: %s with length %d", (const char*) t.tx_buffer, (const int) t.length);
    } else {
        ESP_LOGE(TAG, "SPI transmission failed");
    }
}

// Task to send vehicle ID every 100ms
void spi_task(void *pvParameter) {
    while (1) {
        send_spi_message(VEHICLE_ID);
        vTaskDelay(pdMS_TO_TICKS(5000));  // Wait 100ms
    }
}

// SPI Initialization
void init_spi() {
    esp_err_t ret;

    spi_bus_config_t buscfg = {
        .mosi_io_num = SPI_MOSI,
        .miso_io_num = SPI_MISO,
        .sclk_io_num = SPI_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 100
    };

    spi_device_interface_config_t devcfg = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .clock_speed_hz = 1000000,  // 1MHz
        .mode = 0,
        .spics_io_num = SPI_CS,
        .queue_size = 3
    };

    ret = spi_bus_initialize(HSPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus");
        return;
    }

    // Add SPI device
    ret = spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add SPI device");
        return;
    }

    ESP_LOGI(TAG, "SPI initialized successfully");
}

void app_main() {
    init_spi();
    xTaskCreate(&spi_task, "spi_task", 2048, NULL, 5, NULL);
}