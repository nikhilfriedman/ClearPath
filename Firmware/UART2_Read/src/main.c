#include <stdio.h>
#include <string.h>
#include "driver/spi_slave.h"
#include "driver/spi_master.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define HSPI_MOSI 13
#define HSPI_MISO 12
#define HSPI_SCK 14
#define HSPI_CS 15

#define VSPI_MOSI 23
#define VSPI_MISO 19
#define VSPI_SCK  18
#define VSPI_CS   5

#define UART1_TX_PIN 22
#define UART1_RX_PIN 21

#define UART2_TX_PIN 17
#define UART2_RX_PIN 16
#define UART_BAUD_RATE 115200
#define BUFFER_SIZE 256

static const char *TAG_HSPI = "HSPI_MASTER";
static const char *TAG_UART = "GPS_TX";
static const char *TAG_VSPI = "VSPI_SLAVE";
static const char *TAG_SYS = "SYS_UART";

spi_device_handle_t spi;

// Initialize HSPI as master
void init_hspi() {
    spi_bus_config_t buscfg = {
        .mosi_io_num = HSPI_MOSI,
        .miso_io_num = HSPI_MISO,
        .sclk_io_num = HSPI_SCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32
    };
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1000000,  // 1 MHz
        .mode = 0,
        .spics_io_num = HSPI_CS,
        .queue_size = 1
    };
    spi_bus_initialize(HSPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
}

// Initialize VSPI as slave
void init_vspi_slave() {
    spi_bus_config_t buscfg = {
        .mosi_io_num = VSPI_MOSI,
        .miso_io_num = VSPI_MISO,
        .sclk_io_num = VSPI_SCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32
    };
    
    spi_slave_interface_config_t slvcfg = {
        .spics_io_num = VSPI_CS,
        .flags = 0,
        .queue_size = 1,
        .mode = 0
    };

    ESP_ERROR_CHECK(spi_slave_initialize(VSPI_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO));
}

// Initialize UART for GPS transmission
void init_uart() {
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_driver_install(UART_NUM_1, BUFFER_SIZE, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, UART1_TX_PIN, UART1_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

void init_uart2() {
    const uart_config_t uart2_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_driver_install(UART_NUM_2, BUFFER_SIZE, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_2, &uart2_config);
    uart_set_pin(UART_NUM_2, UART2_TX_PIN, UART2_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

// Read GPS data from UART2
void read_uart_data() {
    uint8_t data[BUFFER_SIZE];
    int length = uart_read_bytes(UART_NUM_2, data, BUFFER_SIZE - 1, pdMS_TO_TICKS(100));
    if (length > 0) {
        data[length] = '\0';  // Null-terminate the string
        ESP_LOGI(TAG_SYS, "Received All Data: %s", data);
    }
}

// Send Dummy Accelerometer Data
void send_accel_data() {
    uint8_t accel_data[6] = {12, 34, 56, 0, 0, 0};  // Example: AX=12, AY=34, AZ=56
    spi_transaction_t trans = {
        .length = 6 * 8,  // 6 bytes
        .tx_buffer = accel_data,
        .rx_buffer = NULL
    };

    esp_err_t ret = spi_device_transmit(spi, &trans);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG_HSPI, "Sent Dummy Accel Data: AX:%d AY:%d AZ:%d", accel_data[0], accel_data[1], accel_data[2]);
    } else {
        ESP_LOGE(TAG_HSPI, "HSPI Transmission Failed");
    }
}

// Send Dummy GPS Data
void send_gps_data() {
    char gps_data[256];
    strcpy(gps_data, "$GPRMC,201602.00,A,4025.72034,N,08654.68250,W,0.041,,030325,,,A*60\n");  // Example: Latitude,Longitude
    uart_write_bytes(UART_NUM_1, gps_data, strlen(gps_data));
    ESP_LOGI(TAG_UART, "Sent GPS Data: %s", gps_data);
}

// Read data from VSPI (Slave)
void read_vspi_data() {
    uint8_t recv_buf[32] = {0};  // Buffer for received data
    spi_slave_transaction_t trans;
    memset(&trans, 0, sizeof(trans));
    trans.length = sizeof(recv_buf) * 8;  // 3 bytes (AX, AY, AZ)
    trans.rx_buffer = recv_buf;

    esp_err_t ret = spi_slave_transmit(VSPI_HOST, &trans, portMAX_DELAY);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG_VSPI, "Received VSPI Data: %s", recv_buf);
    } else {
        ESP_LOGE(TAG_VSPI, "VSPI Reception Failed");
    }
}

// Task for sending and receiving data
void task_send_receive_data(void *arg) {
    init_hspi();
    init_uart();
    init_uart2();
    init_vspi_slave();

    while (1) {
        send_accel_data();
        send_gps_data();  
        read_uart_data(); 
        read_vspi_data();  // Read VSPI data after sending other data
        vTaskDelay(pdMS_TO_TICKS(1000));  // Keep everything in sync
    }
}

void app_main() {
    xTaskCreate(task_send_receive_data, "task_send_receive_data", 4096, NULL, 5, NULL);
}
