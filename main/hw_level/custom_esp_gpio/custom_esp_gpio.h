#ifndef CUSTOM_ESP_GPIO_H
#define CUSTOM_ESP_GPIO_H

// #include "../../project_top.h"
#include "hw_level_top.h"

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "led_strip.h"
#include "driver/rmt.h"


#if CONFIG_IDF_TARGET_ESP32C3
    #if ESP32C3 == ESP32C3_MINI
        #define NVS_RESET_BUTTON_GPIO_NUM       GPIO_NUM_9      // BOOT 버튼

        #define OUTPUT_UART_PORT                UART_NUM_1
        #define TXD_PIN                         GPIO_NUM_7
        #define RXD_PIN                         GPIO_NUM_6

        #define PIR_OUTPUT_GPIO_NUM             GPIO_NUM_3      // 부팅 트리거    RTC핀
        #define ADC_UNIT                        ADC_UNIT_1
        #define ADC_CHANNEL                     ADC_CHANNEL_2   // GPIO3
        
        #define LED_STRIP_GPIO_NUM              GPIO_NUM_8
        #define LED_STRIP_LEN                   1
        #define LED_STRIP_RESOLUTION_HZ         10 * 1000 * 1000
        #define LED_STRIP_WITH_DMA              false
        
    #elif ESP32C3 == ESP32C3_SUPER_MINI
        #define NVS_RESET_BUTTON_GPIO_NUM       GPIO_NUM_9      // BOOT 버튼

        #define OUTPUT_UART_PORT                UART_NUM_1
        #define TXD_PIN                         GPIO_NUM_7
        #define RXD_PIN                         GPIO_NUM_6

        #define PIR_OUTPUT_GPIO_NUM             GPIO_NUM_3      // 부팅 트리거    RTC핀
        #define ADC_UNIT                        ADC_UNIT_1
        #define ADC_CHANNEL                     ADC_CHANNEL_2   // GPIO3
        
        #define LED_STRIP_GPIO_NUM              GPIO_NUM_8
        #define LED_STRIP_LEN                   1
        #define LED_STRIP_RESOLUTION_HZ         10 * 1000 * 1000
        #define LED_STRIP_WITH_DMA              false
    #endif

#else
    #define NVS_RESET_BUTTON_GPIO_NUM       GPIO_NUM_0      // BOOT 버튼

    #define OUTPUT_UART_PORT                UART_NUM_1
    #define TXD_PIN                         GPIO_NUM_17
    #define RXD_PIN                         GPIO_NUM_18

    #define PIR_OUTPUT_GPIO_NUM             GPIO_NUM_4      // 부팅 트리거
    #define ADC_UNIT                        ADC_UNIT_1
    #define ADC_CHANNEL                     ADC_CHANNEL_4   // GPIO5

    #define LED_STRIP_GPIO_NUM              GPIO_NUM_38
    #define LED_STRIP_LEN                   1
    #define LED_STRIP_RESOLUTION_HZ         10 * 1000 * 1000
    #define LED_STRIP_WITH_DMA              false

#endif

// /// enum 으로 수정
// #define SWITCH_STATUS_UNKNOWN   0
// #define SWITCH_STATUS_OFF       1
// #define SWITCH_STATUS_ON        2

extern uint8_t  ui8_switch_status;

/**
 * @brief       Custom GPIO Initial Function
 * @attention   *주의사항
 * @param[in]   void
 * @return      bool    true : 초기화 성공, false : 초기화 실패
 * @warning     *경고
 * @note        *참고사항
 * 
 * @details     디테일 설명
 * @todo        todo
 * @bug         bug
 */
bool custom_gpio_init(void);

/**
 * @brief       Custom GPIO Set LED Strip Color Function
 * @attention   *주의사항
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
bool custom_gpio_set_led_strip_color(const uint32_t input_ui32_red_value, const uint32_t input_ui32_green_value, const uint32_t input_ui32_blue_value);

/**
 * @brief       Custom GPIO Set LED Strip Color Function
 * @attention   *주의사항
 * @param[in]   void
 * @return      void
 * @warning     *경고
 * @note        *참고사항
 * 
 * @details     디테일 설명
 * @todo        todo
 * @bug         bug
 */
bool custom_gpio_deinit(void);

#endif
