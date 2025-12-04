#ifndef CUSTOM_ESP_GPIO_H
#define CUSTOM_ESP_GPIO_H

#include "hw_level_top.h"

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "led_strip.h"

/*
******************************************************************************
* L298N Motor Driver Pin Configuration
******************************************************************************
* Motor A (Left)  : ENA(PWM), IN1, IN2
* Motor B (Right) : ENB(PWM), IN3, IN4
******************************************************************************
*/

#if CONFIG_IDF_TARGET_ESP32C3
    #if ESP32C3 == ESP32C3_MINI
        // NVS Reset Button
        #define NVS_RESET_BUTTON_GPIO_NUM       GPIO_NUM_9      // BOOT 버튼

        // UART (Raspberry Pi Communication)
        #define OUTPUT_UART_PORT                UART_NUM_1
        #define TXD_PIN                         GPIO_NUM_20
        #define RXD_PIN                         GPIO_NUM_21

        // Motor A (Left Motor)
        #define MOTOR_A_ENA_GPIO_NUM            GPIO_NUM_2      // PWM 속도 제어
        #define MOTOR_A_IN1_GPIO_NUM            GPIO_NUM_3      // 방향 제어
        #define MOTOR_A_IN2_GPIO_NUM            GPIO_NUM_4      // 방향 제어

        // Motor B (Right Motor)
        #define MOTOR_B_ENB_GPIO_NUM            GPIO_NUM_10     // PWM 속도 제어
        #define MOTOR_B_IN3_GPIO_NUM            GPIO_NUM_5      // 방향 제어
        #define MOTOR_B_IN4_GPIO_NUM            GPIO_NUM_6      // 방향 제어

        // Servo Motor (Steering)
        #define SERVO_PWM_GPIO_NUM              GPIO_NUM_7      // 서보 PWM

        // LED Strip (Status Indicator)
        #define LED_STRIP_GPIO_NUM              GPIO_NUM_8
        #define LED_STRIP_LEN                   1
        #define LED_STRIP_RESOLUTION_HZ         10 * 1000 * 1000
        #define LED_STRIP_WITH_DMA              false

    #elif ESP32C3 == ESP32C3_SUPER_MINI
        // NVS Reset Button
        #define NVS_RESET_BUTTON_GPIO_NUM       GPIO_NUM_9      // BOOT 버튼

        // UART (Raspberry Pi Communication)
        #define OUTPUT_UART_PORT                UART_NUM_1
        #define TXD_PIN                         GPIO_NUM_20
        #define RXD_PIN                         GPIO_NUM_21

        // Motor A (Left Motor)
        #define MOTOR_A_ENA_GPIO_NUM            GPIO_NUM_2      // PWM 속도 제어
        #define MOTOR_A_IN1_GPIO_NUM            GPIO_NUM_3      // 방향 제어
        #define MOTOR_A_IN2_GPIO_NUM            GPIO_NUM_4      // 방향 제어

        // Motor B (Right Motor)
        #define MOTOR_B_ENB_GPIO_NUM            GPIO_NUM_10     // PWM 속도 제어
        #define MOTOR_B_IN3_GPIO_NUM            GPIO_NUM_5      // 방향 제어
        #define MOTOR_B_IN4_GPIO_NUM            GPIO_NUM_6      // 방향 제어

        // Servo Motor (Steering)
        #define SERVO_PWM_GPIO_NUM              GPIO_NUM_7      // 서보 PWM

        // LED Strip (Status Indicator)
        #define LED_STRIP_GPIO_NUM              GPIO_NUM_8
        #define LED_STRIP_LEN                   1
        #define LED_STRIP_RESOLUTION_HZ         10 * 1000 * 1000
        #define LED_STRIP_WITH_DMA              false
    #endif

#else
    // Default configuration for other ESP32 variants
    #define NVS_RESET_BUTTON_GPIO_NUM       GPIO_NUM_0

    #define OUTPUT_UART_PORT                UART_NUM_1
    #define TXD_PIN                         GPIO_NUM_17
    #define RXD_PIN                         GPIO_NUM_18

    #define MOTOR_A_ENA_GPIO_NUM            GPIO_NUM_2
    #define MOTOR_A_IN1_GPIO_NUM            GPIO_NUM_3
    #define MOTOR_A_IN2_GPIO_NUM            GPIO_NUM_4

    #define MOTOR_B_ENB_GPIO_NUM            GPIO_NUM_10
    #define MOTOR_B_IN3_GPIO_NUM            GPIO_NUM_5
    #define MOTOR_B_IN4_GPIO_NUM            GPIO_NUM_6

    #define SERVO_PWM_GPIO_NUM              GPIO_NUM_7

    #define LED_STRIP_GPIO_NUM              GPIO_NUM_38
    #define LED_STRIP_LEN                   1
    #define LED_STRIP_RESOLUTION_HZ         10 * 1000 * 1000
    #define LED_STRIP_WITH_DMA              false

#endif

// PWM Configuration for Motors
#define MOTOR_PWM_TIMER                     LEDC_TIMER_0
#define MOTOR_PWM_MODE                      LEDC_LOW_SPEED_MODE
#define MOTOR_PWM_DUTY_RES                  LEDC_TIMER_10_BIT   // 0 ~ 1023
#define MOTOR_PWM_FREQUENCY                 1000                // 1 kHz

#define MOTOR_A_PWM_CHANNEL                 LEDC_CHANNEL_0
#define MOTOR_B_PWM_CHANNEL                 LEDC_CHANNEL_1

// PWM Configuration for Servo
#define SERVO_PWM_TIMER                     LEDC_TIMER_1
#define SERVO_PWM_MODE                      LEDC_LOW_SPEED_MODE
#define SERVO_PWM_DUTY_RES                  LEDC_TIMER_13_BIT   // 0 ~ 8191
#define SERVO_PWM_FREQUENCY                 50                  // 50 Hz (20ms period)
#define SERVO_PWM_CHANNEL                   LEDC_CHANNEL_2

// Servo pulse width (in microseconds)
#define SERVO_MIN_PULSE_US                  500     // -90도
#define SERVO_MAX_PULSE_US                  2500    // +90도
#define SERVO_CENTER_PULSE_US               1500    // 0도

/**
 * @brief       Custom GPIO Initial Function
 * @attention   RC카용 GPIO 초기화 (모터, 서보, LED 등)
 * @param[in]   void
 * @return      bool    true : 초기화 성공, false : 초기화 실패
 */
bool custom_gpio_init(void);

/**
 * @brief       Custom GPIO Set LED Strip Color Function
 * @param[in]   input_ui32_red_value : Red 색상 (0 ~ 255)
 * @param[in]   input_ui32_green_value : Green 색상 (0 ~ 255)
 * @param[in]   input_ui32_blue_value : Blue 색상 (0 ~ 255)
 * @return      bool    true : 성공, false : 실패
 */
bool custom_gpio_set_led_strip_color(const uint32_t input_ui32_red_value, const uint32_t input_ui32_green_value, const uint32_t input_ui32_blue_value);

/**
 * @brief       Custom GPIO Deinitialize Function
 * @param[in]   void
 * @return      bool    true : 해제 성공, false : 해제 실패
 */
bool custom_gpio_deinit(void);

#endif
