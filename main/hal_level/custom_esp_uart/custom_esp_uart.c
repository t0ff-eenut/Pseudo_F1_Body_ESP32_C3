/*
******************************************************************************
* File Name          : custom_esp_uart.c
* Description        : UART 통신 모듈
******************************************************************************
*/

#include "custom_esp_uart.h"
#include "driver/uart.h"

#define UART_DEBUG DEBUG
static const char *TAG = "[@]custom_esp_uart.c";

#define UART_BUF_SIZE 256

static QueueHandle_t uart_queue;

bool custom_uart_init(void) {
    #if UART_DEBUG
    printf("[%s] [시작] custom_uart_init()\n", custom_getRuntimeString());
    #endif

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    
    // UART 드라이버 설치 및 큐 핸들 획득
    esp_err_t err = uart_driver_install(OUTPUT_UART_PORT, UART_BUF_SIZE * 2, UART_BUF_SIZE * 2, 20, &uart_queue, 0);
    if (err != ESP_OK) return false;

    err = uart_param_config(OUTPUT_UART_PORT, &uart_config);
    if (err != ESP_OK) return false;

    err = uart_set_pin(OUTPUT_UART_PORT, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (err != ESP_OK) return false;

    #if UART_DEBUG
    printf("[%s] [완료] custom_uart_init()\n", custom_getRuntimeString());
    #endif
    return true;
}

bool custom_uart_deinit(void) {
    uart_driver_delete(OUTPUT_UART_PORT);
    return true;
}

bool custom_uart_read_byte(uint8_t *out_byte) {
    // 1바이트 비동기 읽기
    int len = uart_read_bytes(OUTPUT_UART_PORT, out_byte, 1, 0);
    return (len > 0);
}

void custom_uart_send_data(const uint8_t *data, size_t len) {
    uart_write_bytes(OUTPUT_UART_PORT, (const char *)data, len);
}
