#include <stdio.h>
#include "driver/spi_slave.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define SPI_MOSI 19
#define SPI_MISO 23  // Not used in one-way communication
#define SPI_SCK 18
#define SPI_CS 5

#define TAG "SPI_SLAVE"

void app_main() {
    esp_err_t ret;

    spi_bus_config_t buscfg = {
        .mosi_io_num = SPI_MOSI,
        .miso_io_num = SPI_MISO, // Not needed for one-way communication
        .sclk_io_num = SPI_SCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32
    };

    spi_slave_interface_config_t slvcfg = {
        .spics_io_num = SPI_CS,
        .flags = 0,
        .queue_size = 1,
        .mode = 0,
        .post_setup_cb = NULL,
        .post_trans_cb = NULL
    };

    ret = spi_slave_initialize(SPI2_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);

    char recv_data[32] = {0};
    spi_slave_transaction_t trans = {
        .length = 8 * sizeof(recv_data),
        .tx_buffer = NULL,
        .rx_buffer = recv_data
    };

    while (1) {
        ret = spi_slave_transmit(SPI2_HOST, &trans, portMAX_DELAY);
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "Received: %s", recv_data);
        }
    }
}