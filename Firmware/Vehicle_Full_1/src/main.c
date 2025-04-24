// #include <stdio.h>
// #include <string.h>
// #include "driver/uart.h"
// #include "driver/spi_slave.h"
// #include "driver/spi_master.h"
// #include "esp_log.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"

// #define UART1_TX_PIN 22
// #define UART1_RX_PIN 21

// #define UART2_TX_PIN 17
// #define UART2_RX_PIN 16
// #define UART_BAUD_RATE 115200
// #define BUFFER_SIZE 256

// #define HSPI_MOSI 13
// #define HSPI_MISO 12
// #define HSPI_SCK 14
// #define HSPI_CS 15

// #define VSPI_MOSI 23
// #define VSPI_MISO 19  //not used for transmission
// #define VSPI_SCK 18
// #define VSPI_CS 5

// #define VEHICLE_ID "1234"

// static const char *TAG = "SYSTEM";
// spi_device_handle_t vspi;

// // Initialize UART2 (for GPS & data transmission)
// void init_uart() {
//     const uart_config_t uart_config = {
//         .baud_rate = UART_BAUD_RATE,
//         .data_bits = UART_DATA_8_BITS,
//         .parity = UART_PARITY_DISABLE,
//         .stop_bits = UART_STOP_BITS_1,
//         .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
//     };
//     uart_driver_install(UART_NUM_1, BUFFER_SIZE, 0, 0, NULL, 0);
//     uart_param_config(UART_NUM_1, &uart_config);
//     uart_set_pin(UART_NUM_1, UART1_TX_PIN, UART1_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
// }

// // Initialize UART2 (for transmitting data)
// void init_uart2() {
//     const uart_config_t uart2_config = {
//         .baud_rate = UART_BAUD_RATE,
//         .data_bits = UART_DATA_8_BITS,
//         .parity = UART_PARITY_DISABLE,
//         .stop_bits = UART_STOP_BITS_1,
//         .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
//     };
//     uart_driver_install(UART_NUM_2, BUFFER_SIZE, 0, 0, NULL, 0);
//     uart_param_config(UART_NUM_2, &uart2_config);
//     uart_set_pin(UART_NUM_2, UART2_TX_PIN, UART2_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
// }

// // Initialize HSPI as SLAVE
// void init_hspi_slave() {
//     spi_bus_config_t buscfg = {
//         .mosi_io_num = HSPI_MOSI,
//         .miso_io_num = HSPI_MISO,
//         .sclk_io_num = HSPI_SCK,
//         .quadwp_io_num = -1,
//         .quadhd_io_num = -1,
//         .max_transfer_sz = 32
//     };

//     spi_slave_interface_config_t slvcfg = {
//         .mode = 0,
//         .spics_io_num = HSPI_CS,
//         .queue_size = 1,
//         .flags = 0,
//     };

//     ESP_ERROR_CHECK(spi_slave_initialize(HSPI_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO));
// }

// // Receive acceleration data from HSPI Master
// void receive_hspi_data(char *accel_data, size_t len) {
//     uint8_t rx_buf[3] = {0};  // Buffer for received data
//     spi_slave_transaction_t trans;
//     memset(&trans, 0, sizeof(trans));

//     trans.length = sizeof(rx_buf) * 8;  // 3 bytes (AX, AY, AZ)
//     trans.rx_buffer = rx_buf;

//     esp_err_t ret = spi_slave_transmit(HSPI_HOST, &trans, portMAX_DELAY);
//     if (ret == ESP_OK) {
//         snprintf(accel_data, len, "AX:%d AY:%d AZ:%d", rx_buf[0], rx_buf[1], rx_buf[2]);
//     } else {
//         snprintf(accel_data, len, "AX:0 AY:0 AZ:0");
//     }
// }

// void parse_gps_data(const char *gps_data, float *latitude, float *longitude) {
//     char *token;
//     char buffer[256];  // Copy to avoid modifying original data
//     strcpy(buffer, gps_data);

//     token = strtok(buffer, "\n"); // Get first NMEA sentence
//     while (token != NULL) {
//         if (strncmp(token, "$GPRMC", 6) == 0 || strncmp(token, "$GPGGA", 6) == 0) {
//             char *ptr = token;
//             int field = 0;
//             char lat[10] = {0}, lon[11] = {0}, lat_dir = 0, lon_dir = 0;

//             while ((ptr = strchr(ptr, ',')) != NULL) {
//                 ptr++; // Move past the comma
//                 field++;

//                 // Extract latitude (field 3 in $GPRMC, field 2 in $GPGGA)
//                 if (field == 3 || field == 2) {
//                     strncpy(lat, ptr, 9); // Copy DDMM.MMMM format
//                 }
//                 // Latitude direction (field 4 in $GPRMC, field 3 in $GPGGA)
//                 else if (field == 4 || field == 3) {
//                     lat_dir = ptr[0];
//                 }
//                 // Extract longitude (field 5 in $GPRMC, field 4 in $GPGGA)
//                 else if (field == 5 || field == 4) {
//                     strncpy(lon, ptr, 10); // Copy DDDMM.MMMM format
//                 }
//                 // Longitude direction (field 6 in $GPRMC, field 5 in $GPGGA)
//                 else if (field == 6 || field == 5) {
//                     lon_dir = ptr[0];
//                     break; // No need to parse further
//                 }
//             }

//             // Convert to decimal degrees
//             if (strlen(lat) > 0 && strlen(lon) > 0) {
//                 float lat_dd = atof(lat) / 100;
//                 float lon_dd = atof(lon) / 100;

//                 int lat_deg = (int)lat_dd;
//                 int lon_deg = (int)lon_dd;

//                 float lat_min = (lat_dd - lat_deg) * 100;
//                 float lon_min = (lon_dd - lon_deg) * 100;

//                 *latitude = lat_deg + (lat_min / 60.0);
//                 *longitude = lon_deg + (lon_min / 60.0);

//                 if (lat_dir == 'S') *latitude *= -1;
//                 if (lon_dir == 'W') *longitude *= -1;

//                 return; // Stop after first valid GPS sentence
//             }
//         }
//         token = strtok(NULL, "\n"); // Next sentence
//     }

//     // If no valid data found
//     *latitude = 0.0;
//     *longitude = 0.0;
// }

// // Read GPS data from UART2
// void read_gps_data(char *gps_data, size_t len, float* latitude, float* longitude) {
//     int rx_bytes = uart_read_bytes(UART_NUM_1, gps_data, len - 1, pdMS_TO_TICKS(10));
//     if (rx_bytes > 0) {
//         gps_data[rx_bytes] = '\0';
//         parse_gps_data(gps_data, latitude, longitude);
//     } else {
//         strcpy(gps_data, "No GPS Data");
//     }
// }

// // Initialize VSPI as Master
// void init_vspi_master() {
//     spi_bus_config_t buscfg = {
//         .mosi_io_num = VSPI_MOSI,
//         .miso_io_num = -1,  // Not used
//         .sclk_io_num = VSPI_SCK,
//         .quadwp_io_num = -1,
//         .quadhd_io_num = -1,
//         .max_transfer_sz = 32
//     };

//     spi_device_interface_config_t devcfg = {
//         .clock_speed_hz = 1000000,  // 1 MHz
//         .mode = 0,
//         .spics_io_num = VSPI_CS,
//         .queue_size = 1
//     };

//     ESP_ERROR_CHECK(spi_bus_initialize(VSPI_HOST, &buscfg, SPI_DMA_CH_AUTO));
//     ESP_ERROR_CHECK(spi_bus_add_device(VSPI_HOST, &devcfg, &vspi));
// }

// // Send vehicle ID via VSPI
// void send_vehicle_id_vspi() {
//     spi_transaction_t trans;
//     memset(&trans, 0, sizeof(trans));

//     trans.length = (strlen(VEHICLE_ID) * 8) + 32;  // Length in bits
//     trans.tx_buffer = VEHICLE_ID;

//     esp_err_t ret = spi_device_transmit(vspi, &trans);
//     if (ret == ESP_OK) {
//         ESP_LOGI(TAG, "Sent Vehicle ID over VSPI: %s", VEHICLE_ID);
//     } else {
//         ESP_LOGE(TAG, "VSPI Transmission Failed");
//     }
//     vTaskDelay(pdMS_TO_TICKS(1000));
// }

// // Task to handle GPS, Acceleration, and VSPI Transmission
// void task_main(void *arg) {
//     init_uart();
//     init_uart2();
//     init_hspi_slave();

//     uint8_t gps_data[BUFFER_SIZE / 2];
//     char accel_data[32];
//     float latitude, longitude;

//     while (1) {
//         // Receive acceleration from HSPI Master
//         receive_hspi_data(accel_data, sizeof(accel_data));

//         // Read GPS data
//         read_gps_data((char *)gps_data, sizeof(gps_data), &latitude, &longitude);

//         // Format output message
//         char output[BUFFER_SIZE];
//         snprintf(output, sizeof(output), "ID:%s, %s, GPS Latitude: %.6f, Longitude: %.6f\n", VEHICLE_ID, accel_data, latitude, longitude);

//         // Send data over UART2 TX
//         uart_write_bytes(UART_NUM_2, output, strlen(output));
//         ESP_LOGI(TAG, "Sent: %s", output);

//         vTaskDelay(pdMS_TO_TICKS(1000));  // Delay 100ms
//     }
// }

// // Task 2: Handle VSPI Transmission
// void task_vspi(void *arg) {
//     init_vspi_master();

//     while (1) {
//         send_vehicle_id_vspi();
//         vTaskDelay(pdMS_TO_TICKS(1000));  // Delay 100ms
//     }
// }

// void app_main() {
//     xTaskCreate(task_main, "task_main", 4096, NULL, 5, NULL);
//     xTaskCreate(task_vspi, "task_vspi", 4096, NULL, 5, NULL);
// }



// #include <stdio.h>
// #include "driver/uart.h"
// #include "driver/gpio.h"
// #include "freertos/task.h"
// #include <string.h>
// #include <stdbool.h>

// #define TX_PIN 17
// #define RX_PIN 16
// #define PWRKEY_PIN 4
// #define UART_PORT UART_NUM_1

// void init_uart() {
//     uart_config_t uart_config = {
//         .baud_rate = 9600,
//         .data_bits = UART_DATA_8_BITS,
//         .parity = UART_PARITY_DISABLE,
//         .stop_bits = UART_STOP_BITS_1,
//         .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
//     };

//     uart_param_config(UART_PORT, &uart_config);
//     uart_set_pin(UART_PORT, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
//     uart_driver_install(UART_PORT, 1024, 0, 0, NULL, 0);
// }

// void power_on_sim7080g() {
//     gpio_set_direction(PWRKEY_PIN, GPIO_MODE_OUTPUT);
//     gpio_set_level(PWRKEY_PIN, 0);
//     vTaskDelay(1000 / portTICK_PERIOD_MS); // Hold PWRKEY low for 1 second
//     gpio_set_level(PWRKEY_PIN, 1);
//     vTaskDelay(5000 / portTICK_PERIOD_MS); // Wait for the module to initialize
// }

// void send_and_wait_for_response(char * cmd) {
//     bool got_response = false;


//     while (!got_response) {
//         uart_write_bytes(UART_PORT, cmd, strlen(cmd));

//         char data[128];
//         int len = uart_read_bytes(UART_PORT, data, sizeof(data), 100 / portTICK_PERIOD_MS);
//         if (len > 0) {
//             got_response = true;
//             data[len] = '\0';
//             printf("%s\n", data);
//         }
//         vTaskDelay(2000 / portTICK_PERIOD_MS);
//     }
// }

// void app_main(void) {
//     printf("I am app main\n");

//     init_uart();

//     send_and_wait_for_response("AT\r");
//     send_and_wait_for_response("AT+CNMP=38\r");
//     send_and_wait_for_response("AT+CMNB=3\r");
//     send_and_wait_for_response("AT+CSDP=2\r");
//     send_and_wait_for_response("AT+SHCONF=\"URL\",\"clearpath-http-relay.onrender.com\"\r");
//     send_and_wait_for_response("AT+SHCONF=\"BODYLEN\",1024\r");
//     send_and_wait_for_response("AT+SHCONF=\"HEADERLEN\",350\r");
//     send_and_wait_for_response("AT+SHCONF?\r");
//     send_and_wait_for_response("AT+CNACT=0,1\r");
//     send_and_wait_for_response("AT+SHCONN\r");
//     send_and_wait_for_response("AT+SHREQ=\"clearpath-http-relay.onrender.com\",1\r");
//     send_and_wait_for_response("AT+SHREAD=0,110\r");
//     send_and_wait_for_response("AT+SHDISC\r");
//     send_and_wait_for_response("AT+CNACT=0,0\r");

//     // // After uart/init and PWRKEY power-on...
//     // send_and_wait_for_response("AT+SHCONF=\"URL\",\"https://f249-2600-1015-b204-f8a3-d8a8-b436-db79-3e5.ngrok-free.app\"\r");
//     // send_and_wait_for_response("AT+SHCONF=\"BODYLEN\",512\r");
//     // send_and_wait_for_response("AT+SHCONF=\"HEADERLEN\",256\r");

//     // // Activate PDP context, open connection
//     // send_and_wait_for_response("AT+CNACT=0,1\r");
//     // send_and_wait_for_response("AT+SHCONN\r");

//     // // ----  GET /test  ----
//     // send_and_wait_for_response("AT+SHREQ=\"/test\",0\r");  // 0 = GET
//     // send_and_wait_for_response("AT+SHREAD=0,512\r");      // read up to 512 bytes

//     // // ----  POST /test  ----
//     // // build a JSON payload
//     // char *json = "{\"foo\":\"bar\"}";
//     // char cmd[64];
//     // sprintf(cmd, "AT+SHREQ=\"/test\",1,%d\r", strlen(json));  // 1 = POST
//     // send_and_wait_for_response(cmd);
//     // send_and_wait_for_response(json);      // send the actual JSON
//     // send_and_wait_for_response("AT+SHREAD=0,512\r"); 

//     // // Cleanup
//     // send_and_wait_for_response("AT+SHDISC\r");
//     // send_and_wait_for_response("AT+CNACT=0,0\r");


//     printf("i finished\n");

// }




#include <stdio.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/task.h"
#include <string.h>
#include <stdbool.h>

#define TX_PIN         17
#define RX_PIN         16
#define PWRKEY_PIN     4
#define UART_PORT      UART_NUM_1

// simple AT→UART→read routine
void send_and_wait_for_response(const char *cmd, int timeout_s) {
    printf("CMD → %s", cmd);
    uart_write_bytes(UART_PORT, cmd, strlen(cmd));
    // wait up to timeout_s seconds for any response
    int waited = 0;
    while (waited < timeout_s) {
        char buf[256];
        int len = uart_read_bytes(UART_PORT, buf, sizeof(buf)-1, 1000 / portTICK_PERIOD_MS);
        if (len > 0) {
            buf[len] = '\0';
            printf("RESP ← %s\n", buf);
            return;
        }
        waited++;
    }
    printf("⚠️ No response after %d s\n", timeout_s);
}

void init_uart() {
    uart_config_t uc = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_PORT, &uc);
    uart_set_pin(UART_PORT, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_PORT, 1024, 0, 0, NULL, 0);
}

void power_on_sim7080g() {
    gpio_set_direction(PWRKEY_PIN, GPIO_MODE_OUTPUT);
    // PWRKEY low then high to latch power
    gpio_set_level(PWRKEY_PIN, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    gpio_set_level(PWRKEY_PIN, 1);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
}

void send_json_to_relay(const char *path, const char *json) {
    char cmd[128];
    int bodylen = strlen(json);

    // tell module what path to hit, that it's a POST and how many bytes we'll send
    snprintf(cmd, sizeof(cmd),
             "AT+SHREQ=\"%s\",1,%d\r", path, bodylen);
    send_and_wait_for_response(cmd, 5);

    // now actually stream the JSON payload
    send_and_wait_for_response(json, 5);

    // read up to 512 bytes of reply
    send_and_wait_for_response("AT+SHREAD=0,512\r", 5);
}

void poll_relay(const char *path) {
    char cmd[64];

    // GET is mode 0, no body length
    snprintf(cmd, sizeof(cmd),
             "AT+SHREQ=\"%s\",0\r", path);
    send_and_wait_for_response(cmd, 5);

    // read whatever the server sent back
    send_and_wait_for_response("AT+SHREAD=0,512\r", 5);
}

void app_main(void) {
    init_uart();
    power_on_sim7080g();

    // Basic NB‐IoT and SH setup
    send_and_wait_for_response("AT\r",         5);
    send_and_wait_for_response("AT+CNMP=38\r", 5);
    send_and_wait_for_response("AT+CMNB=3\r",  5);
    send_and_wait_for_response("AT+CSDP=2\r",  5);

    // point at your Render relay (must include http://)
    send_and_wait_for_response(
      "AT+SHCONF=\"URL\",\"http://clearpath-http-relay.onrender.com/api\"\r", 5);
    // tell it how big headers/body can be
    send_and_wait_for_response("AT+SHCONF=\"HEADERLEN\",350\r", 5);
    send_and_wait_for_response("AT+SHCONF=\"BODYLEN\",1024\r",  5);

    // bring up the PDP context once
    send_and_wait_for_response("AT+CNACT=0,1\r", 10);
    // open the socket
    send_and_wait_for_response("AT+SHCONN\r",     5);

    // ----- Send one JSON message to the relay -----
    const char *json = "{\"payload\":\"hello from ESP32\"}";
    send_json_to_relay("/api/device/send", json);

    // ----- Now enter a loop to poll for messages every 5s -----
    while (true) {
        poll_relay("/api/device/receive");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }

    // (if you ever exit loop, cleanup:)
    // send_and_wait_for_response("AT+SHDISC\r", 2);
    // send_and_wait_for_response("AT+CNACT=0,0\r", 2);
}
