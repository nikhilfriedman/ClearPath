#include <stdio.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define SPI_MOSI 23
#define SPI_MISO 19  // Not used in one-way communication
#define SPI_SCK 18
#define SPI_CS 5

#define TAG "SPI_MASTER"

void app_main() {
    esp_err_t ret;
    spi_device_handle_t spi;

    spi_bus_config_t buscfg = {
        .mosi_io_num = SPI_MOSI,
        .miso_io_num = SPI_MISO, // Not needed for one-way transmission
        .sclk_io_num = SPI_SCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32
    };

    spi_device_interface_config_t devcfg = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 0,
        .duty_cycle_pos = 128,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = 1 * 1000 * 1000, // 1 MHz
        .spics_io_num = SPI_CS,
        .flags = 0,
        .queue_size = 1
    };

    ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);
    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);

    char send_data[] = "Hello Slave!";
    spi_transaction_t trans = {
        .length = 8 * sizeof(send_data), 
        .tx_buffer = send_data,
        .rx_buffer = NULL
    };

    while (1) {
        ret = spi_device_transmit(spi, &trans);
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "Sent: %s", send_data);
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Send every second
    }
}
