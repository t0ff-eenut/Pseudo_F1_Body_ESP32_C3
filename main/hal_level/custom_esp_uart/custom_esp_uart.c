/*
******************************************************************************
* File Name          : custom_esp_uart.c
* Description        : UART 통신 모듈
******************************************************************************
*/

#include "custom_esp_uart.h"
#include "driver/usb_serial_jtag.h"
#include "driver/usb_serial_jtag_vfs.h"

#define UART_DEBUG DEBUG
static const char *TAG = "[@]custom_esp_uart.c";

#define UART_BUF_SIZE 256

bool custom_uart_init(void) {
    #if UART_DEBUG
    printf("[%s] [시작] custom_uart_init()\n", custom_getRuntimeString());
    #endif

    usb_serial_jtag_driver_config_t cfg = {
        .rx_buffer_size = UART_BUF_SIZE * 2,
        .tx_buffer_size = UART_BUF_SIZE * 2,
    };

    esp_err_t err = usb_serial_jtag_driver_install(&cfg);
    if (err != ESP_OK) return false;

    // printf(stdout)도 동일 드라이버를 통하도록 VFS 경로 전환.
    // 이렇게 해야 기본 콘솔 VFS와 드라이버가 동일 하드웨어를 두고
    // 경쟁하지 않아 RX 데이터가 정상 수신된다.
    usb_serial_jtag_vfs_use_driver();

    #if UART_DEBUG
    printf("[%s] [완료] custom_uart_init() - USB Serial/JTAG\n", custom_getRuntimeString());
    #endif
    return true;
}

bool custom_uart_deinit(void) {
    usb_serial_jtag_driver_uninstall();
    return true;
}

bool custom_uart_read_byte(uint8_t *out_byte) {
    int len = usb_serial_jtag_read_bytes(out_byte, 1, 0);
    return (len > 0);
}

void custom_uart_send_data(const uint8_t *data, size_t len) {
    usb_serial_jtag_write_bytes(data, len, pdMS_TO_TICKS(10));
}
