/*
******************************************************************************
* File Name          : custom_esp_uart.c
* Description        : UART Communication Module for Raspberry Pi
******************************************************************************
* UART1 configuration: TX(GPIO20), RX(GPIO21), 115200 baud, 8N1
* Uses ESP-IDF UART driver with ring buffer
******************************************************************************
* first update : 2025/12/04
******************************************************************************
*/

#include "custom_esp_uart.h"

#define UART_DEBUG  DEBUG
static const char *custom_esp_uart_TAG = "[@]custom_esp_uart.c";

static bool b_uart_initialized = false;

bool custom_uart_init(void) {
    #if UART_DEBUG
    printf("[%s] "COLOR_WHITE"[Start]\t %s custom_uart_init()\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_uart_TAG);
    #endif

    if (b_uart_initialized) {
        #if UART_DEBUG
        printf("[%s] "COLOR_YELLOW"[Warn]\t %s UART already initialized\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_uart_TAG);
        #endif
        return true;
    }

    const uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    esp_err_t err;
    
    // Configure UART parameters
    err = uart_param_config(OUTPUT_UART_PORT, &uart_config);
    if (err != ESP_OK) {
        #if UART_DEBUG
        printf("[%s] "COLOR_RED"[Error]\t %s uart_param_config failed: %d\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_uart_TAG, err);
        #endif
        return false;
    }

    // Set UART pins
    err = uart_set_pin(OUTPUT_UART_PORT, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (err != ESP_OK) {
        #if UART_DEBUG
        printf("[%s] "COLOR_RED"[Error]\t %s uart_set_pin failed: %d\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_uart_TAG, err);
        #endif
        return false;
    }

    // Install UART driver with RX ring buffer
    err = uart_driver_install(OUTPUT_UART_PORT, UART_BUF_SIZE * 2, UART_BUF_SIZE * 2, 0, NULL, 0);
    if (err != ESP_OK) {
        #if UART_DEBUG
        printf("[%s] "COLOR_RED"[Error]\t %s uart_driver_install failed: %d\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_uart_TAG, err);
        #endif
        return false;
    }

    b_uart_initialized = true;

    #if UART_DEBUG
    printf("[%s] "COLOR_GREEN"[Done]\t %s UART initialized (TX:%d, RX:%d, Baud:%d)\n" COLOR_RESET, 
           custom_getRuntimeString(), custom_esp_uart_TAG, TXD_PIN, RXD_PIN, UART_BAUD_RATE);
    #endif

    return true;
}

int custom_uart_send(const uint8_t* p_data, uint16_t ui16_len) {
    if (!b_uart_initialized || p_data == NULL || ui16_len == 0) {
        return -1;
    }

    int bytes_sent = uart_write_bytes(OUTPUT_UART_PORT, p_data, ui16_len);
    
    #if UART_DEBUG
    printf("[%s] "COLOR_WHITE"[TX]\t %s Sent %d bytes\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_uart_TAG, bytes_sent);
    #endif

    return bytes_sent;
}

int custom_uart_send_str(const char* p_str) {
    if (!b_uart_initialized || p_str == NULL) {
        return -1;
    }

    int len = strlen(p_str);
    return custom_uart_send((const uint8_t*)p_str, len);
}

int custom_uart_read(uint8_t* p_buf, uint16_t ui16_max_len, uint32_t ui32_timeout_ms) {
    if (!b_uart_initialized || p_buf == NULL || ui16_max_len == 0) {
        return -1;
    }

    int bytes_read = uart_read_bytes(OUTPUT_UART_PORT, p_buf, ui16_max_len, pdMS_TO_TICKS(ui32_timeout_ms));
    
    #if UART_DEBUG
    if (bytes_read > 0) {
        printf("[%s] "COLOR_WHITE"[RX]\t %s Received %d bytes\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_uart_TAG, bytes_read);
    }
    #endif

    return bytes_read;
}

int custom_uart_available(void) {
    if (!b_uart_initialized) {
        return 0;
    }

    size_t buffered_size = 0;
    uart_get_buffered_data_len(OUTPUT_UART_PORT, &buffered_size);
    return (int)buffered_size;
}

void custom_uart_flush_rx(void) {
    if (b_uart_initialized) {
        uart_flush_input(OUTPUT_UART_PORT);
    }
}

bool custom_uart_deinit(void) {
    #if UART_DEBUG
    printf("[%s] "COLOR_WHITE"[Info]\t %s custom_uart_deinit()\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_uart_TAG);
    #endif

    if (!b_uart_initialized) {
        return true;
    }

    esp_err_t err = uart_driver_delete(OUTPUT_UART_PORT);
    if (err != ESP_OK) {
        #if UART_DEBUG
        printf("[%s] "COLOR_RED"[Error]\t %s uart_driver_delete failed: %d\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_uart_TAG, err);
        #endif
        return false;
    }

    b_uart_initialized = false;
    return true;
}
