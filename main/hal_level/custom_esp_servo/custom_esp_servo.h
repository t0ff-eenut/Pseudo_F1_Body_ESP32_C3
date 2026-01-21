#ifndef CUSTOM_ESP_SERVO_H
#define CUSTOM_ESP_SERVO_H

#include "hal_level_handle.h"

/**
 * @brief Initialize Servo Module
 * @return true if success
 */
bool custom_servo_init(void);

/**
 * @brief Set Servo Angle
 * @param angle Angle in degrees (-45 to 45 or -90 to 90 depending on mechanism)
 *              Assuming -90 (Left) to +90 (Right), 0 (Center)
 */
void custom_servo_set_angle(int16_t angle);

/**
 * @brief Get Current Servo Angle
 * @return Current angle
 */
int16_t custom_servo_get_angle(void);

/**
 * @brief Center the servo
 */
void custom_servo_center(void);

/**
 * @brief Deinitialize Servo Module
 */
bool custom_servo_deinit(void);

#endif // CUSTOM_ESP_SERVO_H
