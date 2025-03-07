#include <stdio.h>
#include <string.h>
#include "driver/uart.h"
#include "driver/spi_slave.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define UART1_TX_PIN 22
#define UART1_RX_PIN 21

#define UART2_TX_PIN 17
#define UART2_RX_PIN 16
#define UART_BAUD_RATE 115200
#define BUFFER_SIZE 256

#define HSPI_MOSI 13
#define HSPI_MISO 12
#define HSPI_SCK 14
#define HSPI_CS 15

#define VSPI_MOSI 23
#define VSPI_MISO 19  //not used for transmission
#define VSPI_SCK 18
#define VSPI_CS 5

#define VEHICLE_ID "1234"

static const char *TAG = "SYSTEM";
spi_device_handle_t vspi;

// Initialize UART2 (for GPS & data transmission)
void init_uart() {
    const uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_driver_install(UART_NUM_1, BUFFER_SIZE, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, UART1_TX_PIN, UART1_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

// Initialize UART2 (for transmitting data)
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

// Initialize HSPI as SLAVE
void init_hspi_slave() {
    spi_bus_config_t buscfg = {
        .mosi_io_num = HSPI_MOSI,
        .miso_io_num = HSPI_MISO,
        .sclk_io_num = HSPI_SCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32
    };

    spi_slave_interface_config_t slvcfg = {
        .mode = 0,
        .spics_io_num = HSPI_CS,
        .queue_size = 1,
        .flags = 0,
    };

    ESP_ERROR_CHECK(spi_slave_initialize(HSPI_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO));
}

// Receive acceleration data from HSPI Master
void receive_hspi_data(char *accel_data, size_t len) {
    uint8_t rx_buf[3] = {0};  // Buffer for received data
    spi_slave_transaction_t trans;
    memset(&trans, 0, sizeof(trans));

    trans.length = sizeof(rx_buf) * 8;  // 3 bytes (AX, AY, AZ)
    trans.rx_buffer = rx_buf;

    esp_err_t ret = spi_slave_transmit(HSPI_HOST, &trans, portMAX_DELAY);
    if (ret == ESP_OK) {
        snprintf(accel_data, len, "AX:%d AY:%d AZ:%d", rx_buf[0], rx_buf[1], rx_buf[2]);
    } else {
        snprintf(accel_data, len, "AX:0 AY:0 AZ:0");
    }
}

void parse_gps_data(const char *gps_data, float *latitude, float *longitude) {
    char *token;
    char buffer[256];  // Copy to avoid modifying original data
    strcpy(buffer, gps_data);

    token = strtok(buffer, "\n"); // Get first NMEA sentence
    while (token != NULL) {
        if (strncmp(token, "$GPRMC", 6) == 0 || strncmp(token, "$GPGGA", 6) == 0) {
            char *ptr = token;
            int field = 0;
            char lat[10] = {0}, lon[11] = {0}, lat_dir = 0, lon_dir = 0;

            while ((ptr = strchr(ptr, ',')) != NULL) {
                ptr++; // Move past the comma
                field++;

                // Extract latitude (field 3 in $GPRMC, field 2 in $GPGGA)
                if (field == 3 || field == 2) {
                    strncpy(lat, ptr, 9); // Copy DDMM.MMMM format
                }
                // Latitude direction (field 4 in $GPRMC, field 3 in $GPGGA)
                else if (field == 4 || field == 3) {
                    lat_dir = ptr[0];
                }
                // Extract longitude (field 5 in $GPRMC, field 4 in $GPGGA)
                else if (field == 5 || field == 4) {
                    strncpy(lon, ptr, 10); // Copy DDDMM.MMMM format
                }
                // Longitude direction (field 6 in $GPRMC, field 5 in $GPGGA)
                else if (field == 6 || field == 5) {
                    lon_dir = ptr[0];
                    break; // No need to parse further
                }
            }

            // Convert to decimal degrees
            if (strlen(lat) > 0 && strlen(lon) > 0) {
                float lat_dd = atof(lat) / 100;
                float lon_dd = atof(lon) / 100;

                int lat_deg = (int)lat_dd;
                int lon_deg = (int)lon_dd;

                float lat_min = (lat_dd - lat_deg) * 100;
                float lon_min = (lon_dd - lon_deg) * 100;

                *latitude = lat_deg + (lat_min / 60.0);
                *longitude = lon_deg + (lon_min / 60.0);

                if (lat_dir == 'S') *latitude *= -1;
                if (lon_dir == 'W') *longitude *= -1;

                return; // Stop after first valid GPS sentence
            }
        }
        token = strtok(NULL, "\n"); // Next sentence
    }

    // If no valid data found
    *latitude = 0.0;
    *longitude = 0.0;
}

// Read GPS data from UART2
void read_gps_data(char *gps_data, size_t len, float* latitude, float* longitude) {
    int rx_bytes = uart_read_bytes(UART_NUM_1, gps_data, len - 1, pdMS_TO_TICKS(10));
    if (rx_bytes > 0) {
        gps_data[rx_bytes] = '\0';
        parse_gps_data(gps_data, latitude, longitude);
    } else {
        strcpy(gps_data, "No GPS Data");
    }
}

// Initialize VSPI as Master
void init_vspi_master() {
    spi_bus_config_t buscfg = {
        .mosi_io_num = VSPI_MOSI,
        .miso_io_num = -1,  // Not used
        .sclk_io_num = VSPI_SCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1000000,  // 1 MHz
        .mode = 0,
        .spics_io_num = VSPI_CS,
        .queue_size = 1
    };

    ESP_ERROR_CHECK(spi_bus_initialize(VSPI_HOST, &buscfg, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(spi_bus_add_device(VSPI_HOST, &devcfg, &vspi));
}

// Send vehicle ID via VSPI
void send_vehicle_id_vspi() {
    spi_transaction_t trans;
    memset(&trans, 0, sizeof(trans));

    trans.length = (strlen(VEHICLE_ID) * 8) + 32;  // Length in bits
    trans.tx_buffer = VEHICLE_ID;

    esp_err_t ret = spi_device_transmit(vspi, &trans);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Sent Vehicle ID over VSPI: %s", VEHICLE_ID);
    } else {
        ESP_LOGE(TAG, "VSPI Transmission Failed");
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
}

// Task to handle GPS, Acceleration, and VSPI Transmission
void task_main(void *arg) {
    init_uart();
    init_uart2();
    init_hspi_slave();

    uint8_t gps_data[BUFFER_SIZE / 2];
    char accel_data[32];
    float latitude, longitude;

    while (1) {
        // Receive acceleration from HSPI Master
        receive_hspi_data(accel_data, sizeof(accel_data));

        // Read GPS data
        read_gps_data((char *)gps_data, sizeof(gps_data), &latitude, &longitude);

        // Format output message
        char output[BUFFER_SIZE];
        snprintf(output, sizeof(output), "ID:%s, %s, GPS Latitude: %.6f, Longitude: %.6f\n", VEHICLE_ID, accel_data, latitude, longitude);

        // Send data over UART2 TX
        uart_write_bytes(UART_NUM_2, output, strlen(output));
        ESP_LOGI(TAG, "Sent: %s", output);

        vTaskDelay(pdMS_TO_TICKS(1000));  // Delay 100ms
    }
}

// Task 2: Handle VSPI Transmission
void task_vspi(void *arg) {
    init_vspi_master();

    while (1) {
        send_vehicle_id_vspi();
        vTaskDelay(pdMS_TO_TICKS(1000));  // Delay 100ms
    }
}

void app_main() {
    xTaskCreate(task_main, "task_main", 4096, NULL, 5, NULL);
    xTaskCreate(task_vspi, "task_vspi", 4096, NULL, 5, NULL);
}
