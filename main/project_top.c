/*
******************************************************************************
* File Name          : project_top.c
* Description        : 초기 부팅에 필요한 Define 및 전역 함수 및 변수 관리
******************************************************************************
* 프로젝트 전반에 걸쳐 사용되는 전역 변수, 상수, 공통 유틸리티 함수를 정의하고 구현
*
* 주요 기능:
* - 펌웨어의 동작 모드(MODE) 및 디버그 Print 옵션을 설정하는 매크로와 핵심 상수들을 포함
* - 여러 FreeRTOS Task 간의 종료 및 동기화를 위한 전역 플래그 변수를 정의
* - 로그 출력용 시간 문자열 생성, 부팅 원인 출력 등 디버깅에 유용한 헬퍼 함수들을 구현
******************************************************************************

******************************************************************************
* first update : 2025/11/21
******************************************************************************
* final update : 2025/11/21
******************************************************************************
*/
#include "project_top.h"

esp_sleep_wakeup_cause_t esp_sleep_wakeup_cause;
esp_err_t global_esp_err;

bool b_iSENSOR_background_end   = false;
bool b_iSENSOR_background_ended = false;
bool b_deep_sleep_ready         = false;

#define PROJECT_TOP_DEBUG       DEBUG_PRINT

static const char *project_top_TAG  = "[@]project_top.c";

const char* getRuntimeString(void){
    static const int i_buffer_size = 32;
    char c_A_buffer[i_buffer_size];
    const uint64_t u64_us            = esp_timer_get_time(); // 부팅 이후 경과 시간 (마이크로초)
    const uint64_t u64_total_seconds = u64_us / 1000000ULL;
    const uint64_t u64_hours         = u64_total_seconds / 3600;
    const uint64_t u64_minutes       = (u64_total_seconds % 3600) / 60;
    const uint64_t u64_seconds       = u64_total_seconds % 60;
    const uint64_t u64_rem_us        = u64_us % 1000000ULL;

    snprintf(c_A_buffer, sizeof(c_A_buffer), "%02llu:%02llu:%02llu.%06llu", u64_hours, u64_minutes, u64_seconds, u64_rem_us);

    return c_A_buffer;
}

const void wakeup_cause_print(const esp_sleep_wakeup_cause_t input_esp_sleep_wakeup_cause){
    #define WAKEUP_CAUSE_PRINT_DEBUG         PROJECT_TOP_DEBUG

    #if WAKEUP_CAUSE_PRINT_DEBUG
    printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s wakeup_cause_print() - Booting 이유 : ", getRuntimeString(), project_top_TAG);
    #endif
    switch (input_esp_sleep_wakeup_cause) {
        case ESP_SLEEP_WAKEUP_UNDEFINED:
            #if WAKEUP_CAUSE_PRINT_DEBUG
            printf("전원 켜짐 또는 리셋 (정의되지 않음)\n" COLOR_RESET);
            #endif
            break;

        case ESP_SLEEP_WAKEUP_TIMER:
            #if WAKEUP_CAUSE_PRINT_DEBUG
            printf("타이머 만료로 기기 깨어남\n" COLOR_RESET);
            #endif
            break;

        case ESP_SLEEP_WAKEUP_EXT0:
            #if WAKEUP_CAUSE_PRINT_DEBUG
            printf("RTC_IO 단일 핀 외부 신호로 깨어남 (EXT0)\n" COLOR_RESET);
            #endif
            break;

        case ESP_SLEEP_WAKEUP_EXT1:
            #if WAKEUP_CAUSE_PRINT_DEBUG
            printf("RTC_CNTL 여러 핀 외부 신호로 깨어남 (EXT1)\n" COLOR_RESET);
            #endif
            break;

        case ESP_SLEEP_WAKEUP_GPIO:
            #if WAKEUP_CAUSE_PRINT_DEBUG
            printf("일반 GPIO 신호로 깨어남\n" COLOR_RESET);
            #endif
            break;

        case ESP_SLEEP_WAKEUP_TOUCHPAD:
            #if WAKEUP_CAUSE_PRINT_DEBUG
            printf("터치센서 감지로 깨어남\n" COLOR_RESET);
            #endif
            break;

        case ESP_SLEEP_WAKEUP_ULP:
            #if WAKEUP_CAUSE_PRINT_DEBUG
            printf("ULP 코프로세서 실행 완료로 깨어남\n" COLOR_RESET);
            #endif
            break;

        case ESP_SLEEP_WAKEUP_UART:
            #if WAKEUP_CAUSE_PRINT_DEBUG
            printf("UART 데이터 수신으로 깨어남\n" COLOR_RESET);
            #endif
            break;

        case ESP_SLEEP_WAKEUP_WIFI:
            #if WAKEUP_CAUSE_PRINT_DEBUG
            printf("Wi-Fi 이벤트로 깨어남\n" COLOR_RESET);
            #endif
            break;

        case ESP_SLEEP_WAKEUP_COCPU:
            #if WAKEUP_CAUSE_PRINT_DEBUG
            printf("코프로세서 이벤트로 깨어남\n" COLOR_RESET);
            #endif
            break;

        case ESP_SLEEP_WAKEUP_COCPU_TRAP_TRIG:
            #if WAKEUP_CAUSE_PRINT_DEBUG
            printf("코프로세서 트랩 발생으로 깨어남\n" COLOR_RESET);
            #endif
            break;

        case ESP_SLEEP_WAKEUP_BT:
            #if WAKEUP_CAUSE_PRINT_DEBUG
            printf("블루투스 이벤트로 깨어남\n" COLOR_RESET);
            #endif
            break;

        default:
            #if WAKEUP_CAUSE_PRINT_DEBUG
            printf("알 수 없는 원인으로 깨어남\n" COLOR_RESET);
            #endif
            break;
    }
}

// const uint8_t ui8_abs(const uint8_t input_ui8_a, const uint8_t input_ui8_b){
//     return (input_ui8_a > input_ui8_b) ? input_ui8_a - input_ui8_b : input_ui8_b - input_ui8_a;
// }
// const uint16_t ui16_abs(const uint16_t input_ui16_a, const uint16_t input_ui16_b){
//     return (input_ui16_a > input_ui16_b) ? input_ui16_a - input_ui16_b : input_ui16_b - input_ui16_a;
// }
// const uint64_t ui64_abs(const uint64_t input_ui64_a, const uint64_t input_ui64_b){
//     return (input_ui64_a > input_ui64_b) ? input_ui64_a - input_ui64_b : input_ui64_b - input_ui64_a;
// }

const int tick_to_delay(const int input_i_tick){
    return pdTICKS_TO_MS(input_i_tick);
}
const int ms_to_delay(const int input_i_ms){
    return pdMS_TO_TICKS(input_i_ms);
}