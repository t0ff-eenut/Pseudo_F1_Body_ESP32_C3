#ifndef CUSTOM_ESP_UART_H
#define CUSTOM_ESP_UART_H

#include "hw_level_handle.h"
#include "driver/uart.h"

/*
******************************************************************************
* UART Communication Module - Raspberry Pi Interface
******************************************************************************
* UART1: TX(GPIO20), RX(GPIO21), 115200 baud
* Ring buffer based async receive
******************************************************************************
*/

#define UART_BUF_SIZE           256
#define UART_BAUD_RATE          115200

/**
 * @brief       UART Module Initialize
 * @return      bool    true: success, false: failed
 */
bool custom_uart_init(void);

/**
 * @brief       Send data via UART
 * @param[in]   p_data : Pointer to data buffer
 * @param[in]   ui16_len : Data length
 * @return      int     Number of bytes sent, -1 on error
 */
int custom_uart_send(const uint8_t* p_data, uint16_t ui16_len);

/**
 * @brief       Send string via UART
 * @param[in]   p_str : Null-terminated string
 * @return      int     Number of bytes sent, -1 on error
 */
int custom_uart_send_str(const char* p_str);

/**
 * @brief       Read data from UART (blocking with timeout)
 * @param[in]   p_buf : Buffer to store received data
 * @param[in]   ui16_max_len : Maximum buffer size
 * @param[in]   ui32_timeout_ms : Timeout in milliseconds
 * @return      int     Number of bytes received, -1 on timeout/error
 */
int custom_uart_read(uint8_t* p_buf, uint16_t ui16_max_len, uint32_t ui32_timeout_ms);

/**
 * @brief       Check if data is available in UART buffer
 * @return      int     Number of bytes available
 */
int custom_uart_available(void);

/**
 * @brief       Flush UART RX buffer
 */
void custom_uart_flush_rx(void);

/**
 * @brief       UART Module Deinitialize
 * @return      bool    true: success, false: failed
 */
bool custom_uart_deinit(void);

#endif // CUSTOM_ESP_UART_H
