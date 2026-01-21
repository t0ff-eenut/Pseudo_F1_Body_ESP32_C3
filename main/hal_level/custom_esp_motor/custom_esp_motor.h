#ifndef CUSTOM_ESP_MOTOR_H
#define CUSTOM_ESP_MOTOR_H

#include "hal_level_handle.h"

/*
******************************************************************************
* DC Motor Control Module Header
******************************************************************************
* Supports 2 DC Motors (Front and Rear)
* Each motor controlled by 2 pins (IN1, IN2)
******************************************************************************
*/

typedef enum {
    MOTOR_STOP = 0,
    MOTOR_FORWARD,
    MOTOR_BACKWARD,
    MOTOR_BRAKE
} mde; // Motor Direction Enum

typedef struct {
    int8_t speed_front; // -100 to 100
    int8_t speed_rear;  // -100 to 100
    mde dir_front;
    mde dir_rear;
} mss; // Motor State Struct

/**
 * @brief Initialize Motor Module
 * @return true if success
 */
bool custom_motor_init(void);

/**
 * @brief Set Front Motor Speed
 * @param speed -100 (Full Reverse) to 100 (Full Forward)
 */
void custom_motor_set_front(int8_t speed);

/**
 * @brief Set Rear Motor Speed
 * @param speed -100 (Full Reverse) to 100 (Full Forward)
 */
void custom_motor_set_rear(int8_t speed);

/**
 * @brief Set Both Motors Speed
 */
void custom_motor_set_both(int8_t front_speed, int8_t rear_speed);

/**
 * @brief Stop All Motors (Coast)
 */
void custom_motor_stop_all(void);

/**
 * @brief Get Current Motor State
 */
mss custom_motor_get_state(void);

/**
 * @brief Deinitialize Motor Module
 */
bool custom_motor_deinit(void);

#endif // CUSTOM_ESP_MOTOR_H
