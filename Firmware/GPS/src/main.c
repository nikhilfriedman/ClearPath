#include <stdio.h>
#include "driver/uart.h"
#include "esp_log.h"

#define GPS_UART UART_NUM_2
#define TXD_PIN 17  // Not required for basic reading
#define RXD_PIN 16  // GPS TX -> ESP32 RX2

#define BUF_SIZE (1024)

static const char *TAG = "GPS";

void init_uart() {
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(GPS_UART, &uart_config);
    uart_set_pin(GPS_UART, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(GPS_UART, BUF_SIZE * 2, 0, 0, NULL, 0);
}

void gps_task(void *arg) {
    uint8_t data[BUF_SIZE];
    while (1) {
        int len = uart_read_bytes(GPS_UART, data, BUF_SIZE, pdMS_TO_TICKS(500));
        if (len > 0) {
            data[len] = '\0';  // Null-terminate the string
            ESP_LOGI(TAG, "GPS Data: %s", data);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main() {
    ESP_LOGI(TAG, "Initializing GPS UART...");
    init_uart();
    xTaskCreate(gps_task, "gps_task", 4096, NULL, 5, NULL);
}
