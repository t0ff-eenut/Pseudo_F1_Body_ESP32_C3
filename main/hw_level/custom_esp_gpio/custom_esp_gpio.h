#ifndef CUSTOM_ESP_GPIO_H
#define CUSTOM_ESP_GPIO_H

#include "hw_level_top.h"

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "led_strip.h"

/*
******************************************************************************
* Motor Driver Pin Configuration (2-pin Control)
******************************************************************************
* Motor Front (A) : IN1, IN2
* Motor Rear  (B) : IN3, IN4
* Steering        : Servo PWM
******************************************************************************
*/

#if CONFIG_IDF_TARGET_ESP32C3
    // Common settings for C3
    #define NVS_RESET_BUTTON_GPIO_NUM       GPIO_NUM_9      // BOOT Button

    // UART (Control Signal)
    #define OUTPUT_UART_PORT                UART_NUM_1
    #define TXD_PIN                         GPIO_NUM_20
    #define RXD_PIN                         GPIO_NUM_21

    // Motor Front (Drive A)
    // Moved from GPIO 2/3 to GPIO 6/10 to avoid strapping pin issues (GPIO 2)
    #define MOTOR_FRONT_IN1_GPIO_NUM        GPIO_NUM_6
    #define MOTOR_FRONT_IN2_GPIO_NUM        GPIO_NUM_10

    // Motor Rear (Drive B)
    #define MOTOR_REAR_IN1_GPIO_NUM         GPIO_NUM_4
    #define MOTOR_REAR_IN2_GPIO_NUM         GPIO_NUM_5

    // Steering Servo
    #define SERVO_PWM_GPIO_NUM              GPIO_NUM_7

    // LED Strip
    #define LED_STRIP_GPIO_NUM              GPIO_NUM_8
    #define LED_STRIP_LEN                   1
    #define LED_STRIP_RESOLUTION_HZ         10 * 1000 * 1000
    #define LED_STRIP_WITH_DMA              false

#else
    // Fallback for non-C3
    #define NVS_RESET_BUTTON_GPIO_NUM       GPIO_NUM_0
    #define OUTPUT_UART_PORT                UART_NUM_1
    #define TXD_PIN                         GPIO_NUM_17
    #define RXD_PIN                         GPIO_NUM_18
    #define MOTOR_FRONT_IN1_GPIO_NUM        GPIO_NUM_12
    #define MOTOR_FRONT_IN2_GPIO_NUM        GPIO_NUM_13
    #define MOTOR_REAR_IN1_GPIO_NUM         GPIO_NUM_14
    #define MOTOR_REAR_IN2_GPIO_NUM         GPIO_NUM_15
    #define SERVO_PWM_GPIO_NUM              GPIO_NUM_16
    #define LED_STRIP_GPIO_NUM              GPIO_NUM_38
    #define LED_STRIP_LEN                   1
    #define LED_STRIP_RESOLUTION_HZ         10 * 1000 * 1000
    #define LED_STRIP_WITH_DMA              false
#endif

// PWM Configuration for Motors
#define MOTOR_PWM_TIMER                     LEDC_TIMER_0
#define MOTOR_PWM_MODE                      LEDC_LOW_SPEED_MODE
#define MOTOR_PWM_DUTY_RES                  LEDC_TIMER_10_BIT   // 0 ~ 1023
#define MOTOR_PWM_FREQUENCY                 20000               // 20 kHz (Quieter)

// PWM Channels
#define MOTOR_FRONT_IN1_CHANNEL             LEDC_CHANNEL_0
#define MOTOR_FRONT_IN2_CHANNEL             LEDC_CHANNEL_1
#define MOTOR_REAR_IN1_CHANNEL              LEDC_CHANNEL_2
#define MOTOR_REAR_IN2_CHANNEL              LEDC_CHANNEL_3

// PWM Configuration for Servo
#define SERVO_PWM_TIMER                     LEDC_TIMER_1
#define SERVO_PWM_MODE                      LEDC_LOW_SPEED_MODE
#define SERVO_PWM_DUTY_RES                  LEDC_TIMER_13_BIT   // 0 ~ 8191
#define SERVO_PWM_FREQUENCY                 50                  // 50 Hz
#define SERVO_PWM_CHANNEL                   LEDC_CHANNEL_4      // Use Channel 4

// Servo pulse width (us)
#define SERVO_MIN_PULSE_US                  500
#define SERVO_MAX_PULSE_US                  2500
#define SERVO_CENTER_PULSE_US               1500

/**
 * @brief       Custom GPIO Initial Function
 */
bool custom_gpio_init(void);

/**
 * @brief       Custom GPIO Set LED Strip Color Function
 */
bool custom_gpio_set_led_strip_color(const uint32_t input_ui32_red_value, const uint32_t input_ui32_green_value, const uint32_t input_ui32_blue_value);

/**
 * @brief       Custom GPIO Deinitialize Function
 */
bool custom_gpio_deinit(void);

#endif
