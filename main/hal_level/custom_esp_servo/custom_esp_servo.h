#ifndef CUSTOM_ESP_SERVO_H
#define CUSTOM_ESP_SERVO_H

#include "hal_level_handle.h"

/**
 * @brief 서보 모듈 초기화
 * @return 성공 시 true
 */
bool custom_servo_init(void);

/**
 * @brief 서보 각도 설정
 * @param angle 각도 (-90 ~ 90), 0은 중앙
 */
void custom_servo_set_angle(int16_t angle);

/**
 * @brief 현재 서보 각도 가져오기
 * @return 현재 각도
 */
int16_t custom_servo_get_angle(void);

/**
 * @brief 서보 중앙 정렬
 */
void custom_servo_center(void);

/**
 * @brief 서보 모듈 해제
 */
bool custom_servo_deinit(void);

#endif // CUSTOM_ESP_SERVO_H
