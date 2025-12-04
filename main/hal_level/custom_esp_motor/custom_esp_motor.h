#ifndef CUSTOM_ESP_MOTOR_H
#define CUSTOM_ESP_MOTOR_H

#include "hw_level_handle.h"

/*
******************************************************************************
* Motor Control Module - L298N Driver Interface
******************************************************************************
* DC Motor A (Left)  : ENA(PWM), IN1, IN2
* DC Motor B (Right) : ENB(PWM), IN3, IN4
******************************************************************************
*/

/**
 * @enum        motor_direction_enum(mde)
 * @brief       Motor Direction Enum
 * @param MOTOR_STOP        0 - Coast (free spin)
 * @param MOTOR_FORWARD     1 - Forward
 * @param MOTOR_BACKWARD    2 - Backward
 * @param MOTOR_BRAKE       3 - Brake (active stop)
 */
typedef enum motor_direction_enum {
    MOTOR_STOP = 0,
    MOTOR_FORWARD,
    MOTOR_BACKWARD,
    MOTOR_BRAKE,
} mde;

/**
 * @struct      motor_state_struct(mss)
 * @brief       Motor State Structure
 * @param int8_t    i8_speed_left   : Left motor speed (-100 ~ +100)
 * @param int8_t    i8_speed_right  : Right motor speed (-100 ~ +100)
 * @param mde       mde_dir_left    : Left motor direction
 * @param mde       mde_dir_right   : Right motor direction
 */
typedef struct motor_state_struct {
    int8_t i8_speed_left;
    int8_t i8_speed_right;
    mde mde_dir_left;
    mde mde_dir_right;
} mss;

/**
 * @brief       Motor Module Initialize
 * @return      bool    true: success, false: failed
 */
bool custom_motor_init(void);

/**
 * @brief       Set left motor speed
 * @param[in]   i8_speed : -100(backward) ~ 0(stop) ~ +100(forward)
 */
void custom_motor_set_left(int8_t i8_speed);

/**
 * @brief       Set right motor speed
 * @param[in]   i8_speed : -100(backward) ~ 0(stop) ~ +100(forward)
 */
void custom_motor_set_right(int8_t i8_speed);

/**
 * @brief       Set both motors speed
 * @param[in]   i8_left  : Left motor speed (-100 ~ +100)
 * @param[in]   i8_right : Right motor speed (-100 ~ +100)
 */
void custom_motor_set_both(int8_t i8_left, int8_t i8_right);

/**
 * @brief       Brake both motors (active stop)
 */
void custom_motor_brake(void);

/**
 * @brief       Coast both motors (free spin)
 */
void custom_motor_coast(void);

/**
 * @brief       Get current motor state
 * @return      mss     Motor state structure
 */
mss custom_motor_get_state(void);

/**
 * @brief       Motor Module Deinitialize
 * @return      bool    true: success, false: failed
 */
bool custom_motor_deinit(void);

#endif // CUSTOM_ESP_MOTOR_H
