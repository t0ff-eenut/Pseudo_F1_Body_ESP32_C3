#ifndef CUSTOM_ESP_GPIO_H
#define CUSTOM_ESP_GPIO_H

#include "project_top.h"

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "led_strip.h"
#include "driver/rmt.h"

// #define NVS_RESET_BUTTON_GPIO_NUM       GPIO_NUM_9      // BOOT 버튼

// #define OUTPUT_UART_PORT                UART_NUM_1
// #define TXD_PIN                         (GPIO_NUM_7)
// #define RXD_PIN                         (GPIO_NUM_6)

// #define PIR_OUTPUT_GPIO_NUM             GPIO_NUM_3      // 부팅 트리거    RTC핀
// #define ADC_UNIT                        ADC_UNIT_1
// #define ADC_CHANNEL                     ADC_CHANNEL_2   // GPIO3

// #define LED_STRIP_GPIO_NUM              GPIO_NUM_8
// #define LED_STRIP_LEN                   1

/**
 * @brief       Custom GPIO Initial Function
 * @attention   const : 읽기 전용
 * @param[in]   void
 * @return      bool    true : 초기화 성공, false : 초기화 실패
 */
const bool custom_gpio_init(void);

// #define SERVO_PULSE_DELAY   500
#define SERVO_ADC_CHENAL    14
#define SERVO_PULSE_CYCLE   20000       // 20ms
#define SERVO_PULSE_US_MIN  500         // 0.5ms
#define SERVO_PULSE_US_MAX  2500        // 2.5ms
#define SERVO_MAX_ANGLE     180     
#define SERVO_MIN_ANGLE     0

#define SERVO_ON_ANGLE      SERVO_MIN_ANGLE
#define SERVO_OFF_ANGLE     90
#define SERVO_WORK_TIME_MS  2000        // 2초

/**
 * @brief       Custom GPIO ervo Angle To Duty Output Function
 * @attention   *주의사항 [static : 파일 내부 전용, const : 읽기 전용]
 * @param[in]   int         input_i8_input_angle : 각도 입력
 * @return      uint32_t    ui32_duty : us 단위의 Duty 시간
 * @warning     *경고
 * @note        *참고사항
 * 
 * @details     각도에 해당하는 Duty 폭 시간을 반환
 * @todo        todo
 * @bug         bug
 */
static const uint32_t custom_gpio_servo_angle_to_duty(int input_i8_angle_value);

/**
 * @brief       Custom GPIO Servo Angle Setting Function
 * @attention   *주의사항 [const : 읽기 전용]
 * @param[in]   int         input_i8_angle_value : 각도 입력
 * @return      void
 * @warning     *경고
 * @note        *참고사항
 * 
 * @details     각도를 입력하면 해당 Duty 폭을 Servo Motor에 설정
 * @todo        todo
 * @bug         bug
 */
const bool custom_gpio_servo_angle_set(uint8_t input_i8_angle_value);

/**
 * @brief       Custom GPIO Switch Control Function
 * @attention   *주의사항 [const : 읽기 전용]
 * @param[in]   int8_t         input_i8_angle_value : 스위치 ON/OFF Signal 입력
 * @return      bool    true : servo motor 동작 성공, false : servo motor 동작 실패
 * @warning     *경고
 * @note        *참고사항
 * 
 * @details     디테일 설명
 * @todo        todo
 * @bug         bug
 */
const bool custom_gpio_switch_control(int8_t input_i8_switch_command_value);


/**
 * @brief       Custom GPIO Set LED Strip Color Function
 * @attention   *주의사항 [const : 읽기 전용]
 * @param[in]   uint32_t    input_ui32_red_value : Red 색상 정도(0 ~ 255)
 * @param[in]   uint32_t    input_ui32_green_value : Green 색상 정도(0 ~ 255)
 * @param[in]   uint32_t    input_ui32_blue_value : Blue 색상 정도(0 ~ 255)
 * @return      bool    true : LED Strip 색상 변경 성공, false : LED Strip 색상 변경 실패
 * @warning     *경고
 * @note        *참고사항
 * 
 * @details     디테일 설명
 * @todo        todo
 * @bug         bug
 */
const bool custom_gpio_set_led_strip_color(uint32_t input_ui32_red_value, uint32_t input_ui32_green_value, uint32_t input_ui32_blue_value);

/**
 * @brief       Custom GPIO Set LED Strip Color Function
 * @attention   *주의사항 [const : 읽기 전용]
 * @param[in]   void
 * @return      void
 * @warning     *경고
 * @note        *참고사항
 * 
 * @details     디테일 설명
 * @todo        todo
 * @bug         bug
 */
const bool custom_gpio_deinit(void);

#endif