#ifndef CUSTOM_ESP_GPIO_H
#define CUSTOM_ESP_GPIO_H

#include "hw_level_top.h"

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "led_strip.h"

/*
******************************************************************************
* 모터 드라이버 핀 설정 (2-pin Control)
******************************************************************************
* 전륜 모터 (Front) : IN1, IN2
* 후륜 모터 (Rear)  : IN3, IN4
* 조향 (Steering)   : Servo PWM
******************************************************************************
*/

#if CONFIG_IDF_TARGET_ESP32C3
    // C3 공통 설정
    #define NVS_RESET_BUTTON_GPIO_NUM       GPIO_NUM_9      // BOOT 버튼

    // UART (제어 신호)
    #define OUTPUT_UART_PORT                UART_NUM_1
    #define TXD_PIN                         GPIO_NUM_20
    #define RXD_PIN                         GPIO_NUM_21

    // 전륜 모터 (Drive A)
    // GPIO 2는 부트 스트래핑 핀이므로 GPIO 6/10으로 이동하여 부트 문제 방지
    #define MOTOR_FRONT_IN1_GPIO_NUM        GPIO_NUM_6
    #define MOTOR_FRONT_IN2_GPIO_NUM        GPIO_NUM_10

    // 후륜 모터 (Drive B)
    #define MOTOR_REAR_IN1_GPIO_NUM         GPIO_NUM_4
    #define MOTOR_REAR_IN2_GPIO_NUM         GPIO_NUM_5

    // 조향 서보 (Steering Servo)
    #define SERVO_PWM_GPIO_NUM              GPIO_NUM_7

    // LED 스트립
    #define LED_STRIP_GPIO_NUM              GPIO_NUM_8
    #define LED_STRIP_LEN                   1
    #define LED_STRIP_RESOLUTION_HZ         10 * 1000 * 1000
    #define LED_STRIP_WITH_DMA              false

#else
    // 기타 ESP32 모델을 위한 기본 설정
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

// 모터 PWM 설정
#define MOTOR_PWM_TIMER                     LEDC_TIMER_0
#define MOTOR_PWM_MODE                      LEDC_LOW_SPEED_MODE
#define MOTOR_PWM_DUTY_RES                  LEDC_TIMER_10_BIT   // 0 ~ 1023
#define MOTOR_PWM_FREQUENCY                 20000               // 20 kHz (저소음)

// PWM 채널
#define MOTOR_FRONT_IN1_CHANNEL             LEDC_CHANNEL_0
#define MOTOR_FRONT_IN2_CHANNEL             LEDC_CHANNEL_1
#define MOTOR_REAR_IN1_CHANNEL              LEDC_CHANNEL_2
#define MOTOR_REAR_IN2_CHANNEL              LEDC_CHANNEL_3

// 서보 PWM 설정
#define SERVO_PWM_TIMER                     LEDC_TIMER_1
#define SERVO_PWM_MODE                      LEDC_LOW_SPEED_MODE
#define SERVO_PWM_DUTY_RES                  LEDC_TIMER_13_BIT   // 0 ~ 8191
#define SERVO_PWM_FREQUENCY                 50                  // 50 Hz
#define SERVO_PWM_CHANNEL                   LEDC_CHANNEL_4      // 채널 4 사용

// 서보 펄스 폭 (us)
#define SERVO_MIN_PULSE_US                  500
#define SERVO_MAX_PULSE_US                  2500
#define SERVO_CENTER_PULSE_US               1500

/**
 * @brief       GPIO 초기화 함수
 */
bool custom_gpio_init(void);

/**
 * @brief       LED 스트립 색상 설정 함수
 */
bool custom_gpio_set_led_strip_color(const uint32_t input_ui32_red_value, const uint32_t input_ui32_green_value, const uint32_t input_ui32_blue_value);

/**
 * @brief       GPIO 해제 함수
 */
bool custom_gpio_deinit(void);

#endif
