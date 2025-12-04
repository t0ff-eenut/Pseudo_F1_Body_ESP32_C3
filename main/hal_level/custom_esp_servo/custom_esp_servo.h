#ifndef CUSTOM_ESP_SERVO_H
#define CUSTOM_ESP_SERVO_H

#include "hw_level_handle.h"

/*
******************************************************************************
* Servo Motor Control Module
******************************************************************************
* Servo PWM: GPIO7, 50Hz, 500-2500us pulse width
* Angle range: -45 ~ +45 degrees (for steering)
******************************************************************************
*/

/**
 * @brief       Servo Module Initialize
 * @return      bool    true: success, false: failed
 */
bool custom_servo_init(void);

/**
 * @brief       Set servo angle
 * @param[in]   i16_angle : -45 ~ +45 degrees
 */
void custom_servo_set_angle(int16_t i16_angle);

/**
 * @brief       Get current servo angle
 * @return      int16_t : Current angle (-45 ~ +45)
 */
int16_t custom_servo_get_angle(void);

/**
 * @brief       Center the servo (0 degrees)
 */
void custom_servo_center(void);

/**
 * @brief       Servo Module Deinitialize
 * @return      bool    true: success, false: failed
 */
bool custom_servo_deinit(void);

#endif // CUSTOM_ESP_SERVO_H
