#ifndef RCCAR_CONTROL_H
#define RCCAR_CONTROL_H

#include "app_level_top.h"
#include "RCcar_protocol.h"

/**
 * @brief RC카 제어 로직 초기화
 */
bool rccar_control_init(void);

/**
 * @brief 메인 제어 태스크 (루프에서 실행)
 */
void rccar_control_task(void);

/**
 * @brief 수신된 바이트 처리
 */
void rccar_control_process_byte(uint8_t byte);

#endif // RCCAR_CONTROL_H
