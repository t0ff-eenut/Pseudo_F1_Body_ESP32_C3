#ifndef CUSTOM_ESP_UART_H
#define CUSTOM_ESP_UART_H

#include "hal_level_handle.h"

/**
 * @brief UART 모듈 초기화
 * @return 성공 시 true
 */
bool custom_uart_init(void);

/**
 * @brief UART 모듈 해제
 */
bool custom_uart_deinit(void);

/**
 * @brief RX 버퍼에서 1바이트 읽기
 * @param out_byte 읽은 바이트를 저장할 포인터
 * @return true 데이터 있음, false 데이터 없음
 */
bool custom_uart_read_byte(uint8_t *out_byte);

/**
 * @brief UART로 데이터 전송
 */
void custom_uart_send_data(const uint8_t *data, size_t len);

#endif // CUSTOM_ESP_UART_H
