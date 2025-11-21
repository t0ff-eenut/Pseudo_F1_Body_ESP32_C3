#ifndef PROJECT_TOP_H
#define PROJECT_TOP_H

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_sleep.h"
#include "esp_pm.h"
#include "esp_log.h"

#define TEXT_BOLD           "\033[1m"
#define TEXT_UNDER_LINE     "\033[4m"
#define TEXT_REVERSE        "\033[7m"

#define COLOR_BLACK         "\033[30m"
#define COLOR_RED           "\033[31m"
#define COLOR_GREEN         "\033[32m"
#define COLOR_YELLOW        "\033[33m"
#define COLOR_BLUE          "\033[34m"
#define COLOR_MAGENTA       "\033[35m"
#define COLOR_CYAN          "\033[36m"
#define COLOR_WHITE         "\033[37m"
#define COLOR_RESET         "\033[0m"

#define DEBUG_PRINT         true

#ifndef MIN
    #define MIN(a,b)        (( (a) < (b) ) ? (a) : (b))
#endif

#define ns_sleep            * ((1 us_sleep) / 1000)
#define us_sleep            * 1
#define ms_sleep            * (1000 us_sleep)
#define s_sleep             * (1000 ms_sleep)
#define m_sleep             * (60 s_sleep)
#define h_sleep             * (60 m_sleep)

// Enum
/**
 * @enum        queue_input_output_toggle(qiot)
 * @brief       Queue Input/Output Toggle enum
 * @attention   *주의사항
 * @warning     *경고
 * @note        *참고사항
 *
 * @param QUEUE_INPUT     0
 * @param QUEUE_OUTPUT    1
 * 
 * @see         
 * @details     디테일 설명
 * @todo        todo
 * @bug         bug
 */
// Struct
/**
 * @struct      send_uart_queue_struct(suqs)
 * @brief       Send Uart Queue Struct
 * @attention   *주의사항
 * @warning     *경고
 * @note        *참고사항
 *
 * @param uint16_t      ui16_adc_data
 * @param uint16_t      ui16_voltage_data
 * @param uint16_t      ui16_tp1
 * @param uint8_t       ui8_tp2
 * @param uint8_t       ui8_switch_status
 * @param uint16_t*     A_ui16_adc_buf
 * @param uint16_t*     A_ui16_adc_delta_buf
 * @param bool*         A_b_occu_buf
 * @param bool          b_occu_triger
 * 
 * @details     디테일 설명
 * @todo        todo
 * @bug         bug
 */
// 함수
/**
 * @brief       Custom Queue Initial Function
 * @attention   *주의사항 [const : 읽기 전용]
 * @param[in]   void
 * @return      bool    true : 초기화 성공, false : 초기화 실패
 * @warning     *경고
 * @note        *참고사항
 * 
 * @details     디테일 설명
 * @todo        todo
 * @bug         bug
 */

extern esp_sleep_wakeup_cause_t esp_sleep_wakeup_cause;
extern esp_err_t global_esp_err;

// extern bool b_iSENSOR_background_end;
// extern bool b_iSENSOR_background_ended;
// extern bool b_deep_sleep_ready;

/**
 * @brief       Get Runtime String Function
 * @attention   const : 읽기 전용
 * @param[in]   void
 * @return      char*   현재 동작 시간
 */
const char* getRuntimeString(void);

/**
 * @brief       Wake UP Cause Print Function
 * @attention   const : 읽기 전용
 * @param[in]   esp_sleep_wakeup_cause_t input_wakeup_cause [const]
 * @return      void
 */
const void wakeup_cause_print(const esp_sleep_wakeup_cause_t input_wakeup_cause);

// /**
//  * @brief       uint8_t Absolute Value Function
//  * @attention   const : 읽기 전용
//  * @param[in]   uint8_t input_ui8_a [const]
//  * @param[in]   uint8_t input_ui8_b [const]
//  * @return      uint8_t
//  */
// const uint8_t ui8_abs(const uint8_t input_ui8_a, const uint8_t input_ui8_b);

// /**
//  * @brief       uint16_t Absolute Value Function
//  * @attention   const : 읽기 전용
//  * @param[in]   uint16_t input_ui16_a [const]
//  * @param[in]   uint16_t input_ui16_b [const]
//  * @return      uint16_t
//  */
// const uint16_t ui16_abs(const uint16_t input_ui16_a, const uint16_t input_ui16_b);

// /**
//  * @brief       uint64_t Absolute Value Function
//  * @attention   const : 읽기 전용
//  * @param[in]   uint64_t input_ui64_a [const]
//  * @param[in]   uint64_t input_ui64_b [const]
//  * @return      uint64_t
//  */
// const uint64_t ui64_abs(const uint64_t input_ui64_a, const uint64_t input_ui64_b);

/**
 * @brief       Tick Value To Delay Time Value Conversion Function
 * @attention   const : 읽기 전용
 * @param[in]   int input_i_tick [const]
 * @return      int
 */
const int tick_to_delay(const int input_i_tick);

/**
 * @brief       ms Value To Delay Time Value Conversion Function
 * @attention   const : 읽기 전용
 * @param[in]   int input_i_ms [const]
 * @return      int
 */
const int ms_to_delay(const int input_i_ms);





// // memory.h
//     #define WINDOW_SIZE                                 30

// // queue.c
//     #define SEND_CALLBACK_BUFFER_SIZE                   3
//     #define RECEIVE_CALLBACK_BUFFER_SIZE                3
//     #define ADC_READ_BUFFER_SIZE                        300   // ADC 버퍼 크기
//     #define ADC_BUFFER_SIZE                             300   // ADC 버퍼 크기

// // band.c
//     #define SENSOR_MAC                                  {0x02, 0x20, 0x30, 0x40, 0x50, 0x01}
//     #define SWITCH_CONTROLLER_MAC                       {0x02, 0x20, 0x30, 0x40, 0x50, 0x02}

//     #define TRY_SEND_TIMEOUT_US                         2 * 1000 * 1000     // 데이터 전송 3초동안 시도
//     #define WATTING_SEND_CALLBACK_TIMEOUT_US            1 * 1000 * 1000     // SEND CALLBACK을 위한 1초 대기
//     #if MODE == SENSOR
//         #define WATTING_RECEIVE_CALLBACK_TIMEOUT_US     6 * 1000 * 1000     // RECEIVE CALLBACK을 위한 3초 대기
//     #endif
//     #if MODE == SWITCH
//         #define WATTING_RECEIVE_CALLBACK_TIMEOUT_US     4 * 1000 * 1000     // RECEIVE CALLBACK을 위한 1초 대기
//         // #define WATTING_RECEIVE_CALLBACK_TIMEOUT_US     300 * 1000     // RECEIVE CALLBACK을 위한 0.3초 대기
//     #endif
// // Mode
//     #define WATTING_OCCUPANCY_TIMEOUT_US                2 * 1000 * 1000     // 3초간 재실 확인 = 30번 ADC
//     // Switch
//         #define WATTING_PUSH_BUTTON_TIMEOUT_US          2 * 1000 * 1000     // 연속 버튼 누름을 파악하기 위한 2초 대기


// // main.c
//     // #define DEFAULT_SLEEP_TIME                          5 s_sleep
//     #define SYNCHRONIZE_SLEEP_TIME                      10 s_sleep
//     // Sensor
//         #define NONE_SLEEP_TIME                         -1
//     // Switch
//         #define CYCLE_SLEEP_TIME                        1 s_sleep


// ////////////////////////////////
//     // main.c
//     #define DEFAULT_SLEEP_TIME                          1 s_sleep   // Memory로 들어가야함


#endif // MAIN_TOP_H