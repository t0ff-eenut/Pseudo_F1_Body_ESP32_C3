#ifndef CUSTOM_ESP_UART_H
#define CUSTOM_ESP_UART_H

#include "hal_level_handle.h"

/**
 * @brief Initialize UART Module
 * @return true if success
 */
bool custom_uart_init(void);

/**
 * @brief Deinitialize UART Module
 */
bool custom_uart_deinit(void);

/**
 * @brief Read a byte from the RX buffer
 * @param out_byte Pointer to store the byte
 * @return true if data was available
 */
bool custom_uart_read_byte(uint8_t *out_byte);

/**
 * @brief Send data over UART
 */
void custom_uart_send_data(const uint8_t *data, size_t len);

#endif // CUSTOM_ESP_UART_H
