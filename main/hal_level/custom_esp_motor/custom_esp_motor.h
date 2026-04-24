#ifndef CUSTOM_ESP_MOTOR_H
#define CUSTOM_ESP_MOTOR_H

#include "hal_level_handle.h"

/*
******************************************************************************
* DC 모터 제어 모듈 헤더
******************************************************************************
* 2개의 DC 모터(전륜/후륜) 지원
* 각 모터는 2개의 핀(IN1, IN2)으로 제어됨
******************************************************************************
*/

typedef enum {
    MOTOR_STOP = 0,     // 정지
    MOTOR_FORWARD,      // 전진
    MOTOR_BACKWARD,     // 후진
    MOTOR_BRAKE         // 브레이크
} mde; // 모터 방향 열거형 (Motor Direction Enum)

typedef struct {
    int8_t speed_front; // -127 ~ +127
    int8_t speed_rear;  // -127 ~ +127
    mde dir_front;
    mde dir_rear;
} mss; // 모터 상태 구조체 (Motor State Struct)

/**
 * @brief 모터 모듈 초기화
 * @return 성공 시 true
 */
bool custom_motor_init(void);

/**
 * @brief 전륜 모터 속도 설정
 * @param speed -127 (최대 후진) ~ +127 (최대 전진)
 */
void custom_motor_set_front(int8_t speed);

/**
 * @brief 후륜 모터 속도 설정
 * @param speed -127 (최대 후진) ~ +127 (최대 전진)
 */
void custom_motor_set_rear(int8_t speed);

/**
 * @brief 양쪽 모터 속도 설정
 */
void custom_motor_set_both(int8_t front_speed, int8_t rear_speed);

/**
 * @brief 모든 모터 정지 (Coast)
 */
void custom_motor_stop_all(void);

/**
 * @brief 전륜 모터 브레이크
 * @param strength 0 (코스팅) ~ 127 (100% 쇼트 브레이크)
 */
void custom_motor_brake_front(int8_t strength);

/**
 * @brief 후륜 모터 브레이크
 * @param strength 0 (코스팅) ~ 127 (100% 쇼트 브레이크)
 */
void custom_motor_brake_rear(int8_t strength);

/**
 * @brief 양쪽 모터 브레이크
 * @param strength 0 (코스팅) ~ 127 (100% 쇼트 브레이크)
 */
void custom_motor_brake_all(int8_t strength);

/**
 * @brief 현재 모터 상태 가져오기
 */
mss custom_motor_get_state(void);

/**
 * @brief 모터 모듈 해제
 */
bool custom_motor_deinit(void);

#endif // CUSTOM_ESP_MOTOR_H
