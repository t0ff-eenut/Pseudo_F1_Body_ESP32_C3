

/*
******************************************************************************
* File Name          : custom_esp_gpio.c
* Description        : ESP32의 GPIO 설정 및 제어
******************************************************************************
* ESP32의 GPIO 설정 및 제어를 위한 코드
* PIR 센서 OUTPUT, 외부 택드 버튼, 서보 모터 제어 등을 포함
* INTERRUPT 만 손 보면 될 듯
******************************************************************************

******************************************************************************
* first update : 2025/12/03
******************************************************************************
* final update : 2025/12/03
******************************************************************************
*/
#include "custom_esp_gpio.h"

#define GPIO_DEBUG  DEBUG
static const char *custom_esp_gpio_TAG = "[@]custom_esp_gpio.c";

// uint8_t ui8_switch_status = SWITCH_STATUS_OFF;

// GPIO Setting 여부
static bool b_A_gpio_states[40] = {false,};
// static led_strip_handle_t s_led_strip;

bool custom_gpio_init(void){
    #define CUSTOM_GPIO_INIT_DEBUG         GPIO_DEBUG

    static bool b_success = true;

    /////////////////////////////
    /// NVS 초기화 버튼 Initial ///
    /////////////////////////////
    #if CUSTOM_GPIO_INIT_DEBUG
    printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_init() - NVS 초기화를 위한 NVS_RESET_BUTTON_GPIO_NUM[%d] 설정(BOOT 버튼)\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, NVS_RESET_BUTTON_GPIO_NUM);
    printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - NVS_RESET_BUTTON_GPIO_NUM[%d] GPIO_MODE_INPUT 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, NVS_RESET_BUTTON_GPIO_NUM);
    #endif
    
    b_success &= !gpio_set_direction(NVS_RESET_BUTTON_GPIO_NUM, GPIO_MODE_INPUT);
    if(!b_success){
        #if CUSTOM_GPIO_INIT_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_init() - NVS_RESET_BUTTON_GPIO_NUM[%d] GPIO_MODE_INPUT 설정 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, NVS_RESET_BUTTON_GPIO_NUM);
        #endif
        return b_success;
    }
    #if CUSTOM_GPIO_INIT_DEBUG
    printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - NVS_RESET_BUTTON_GPIO_NUM[%d] GPIO_PULLUP_ONLY 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, NVS_RESET_BUTTON_GPIO_NUM);
    #endif
    
    b_success &= !gpio_set_pull_mode(NVS_RESET_BUTTON_GPIO_NUM, GPIO_PULLUP_ONLY);
    if(!b_success){
        #if CUSTOM_GPIO_INIT_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_init() - NVS_RESET_BUTTON_GPIO_NUM[%d] GPIO_PULLUP_ONLY 설정 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, NVS_RESET_BUTTON_GPIO_NUM);
        #endif
        return b_success;
    }
    b_A_gpio_states[NVS_RESET_BUTTON_GPIO_NUM] = true;

    ///////////////////////////////////
    /// PIR Signal 수신 GPIO Initial ///
    ///////////////////////////////////
    #if CUSTOM_GPIO_INIT_DEBUG
    printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_init() - 외부 PIR Signal Interrupt 수신을 위한 PIR_OUTPUT_GPIO_NUM[%d] 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, PIR_OUTPUT_GPIO_NUM);
    printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - PIR_OUTPUT_GPIO_NUM[%d] GPIO_MODE_INPUT 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, PIR_OUTPUT_GPIO_NUM);
    printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - PIR_OUTPUT_GPIO_NUM[%d] GPIO_PULLUP_DISABLE 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, PIR_OUTPUT_GPIO_NUM);
    printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - PIR_OUTPUT_GPIO_NUM[%d] GPIO_PULLDOWN_ENABLE 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, PIR_OUTPUT_GPIO_NUM);
    printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - PIR_OUTPUT_GPIO_NUM[%d] GPIO_INTR_DISABLE 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, PIR_OUTPUT_GPIO_NUM);
    #endif
    static const gpio_config_t pir_output_io_conf = {
        .pin_bit_mask = (1ULL << PIR_OUTPUT_GPIO_NUM),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    b_success &= !gpio_config(&pir_output_io_conf);
    if(!b_success){
        #if CUSTOM_GPIO_INIT_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_init() - PIR_OUTPUT_GPIO_NUM[%d] gpio_config() 설정 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, PIR_OUTPUT_GPIO_NUM);
        #endif
        return b_success;
    }
    b_A_gpio_states[PIR_OUTPUT_GPIO_NUM] = true;

    //////////////////////////////
    /// LED Strip GPIO Initial ///
    //////////////////////////////
    #if CUSTOM_GPIO_INIT_DEBUG
    printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_init() - RGB LED Control을 위해 LED_STRIP_GPIO_NUM[%d] Strip Config 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, LED_STRIP_GPIO_NUM);
    printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - LED_STRIP_GPIO_NUM[%d] strip_gpio_num = LED_STRIP_GPIO_NUM[%d] 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, LED_STRIP_GPIO_NUM, LED_STRIP_GPIO_NUM);
    printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - LED_STRIP_GPIO_NUM[%d] max_leds = LED_STRIP_LEN[%d] 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, LED_STRIP_GPIO_NUM, LED_STRIP_LEN);
    #endif
    static const led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_GPIO_NUM,
        .max_leds = LED_STRIP_LEN, // LED 개수
    };
    #if CUSTOM_GPIO_INIT_DEBUG
    printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_init() - RGB LED Control을 위해 LED_STRIP_GPIO_NUM[%d] rmt Config 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, LED_STRIP_GPIO_NUM);
    printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - LED_STRIP_GPIO_NUM[%d] resolution_hz = LED_STRIP_RESOLUTION_HZ[%d]\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, LED_STRIP_GPIO_NUM, LED_STRIP_RESOLUTION_HZ);
    printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - LED_STRIP_GPIO_NUM[%d] with_dma = LED_STRIP_WITH_DMA[%d]\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, LED_STRIP_GPIO_NUM, LED_STRIP_WITH_DMA);
    #endif
    static const led_strip_rmt_config_t rmt_config = {
        .resolution_hz = LED_STRIP_RESOLUTION_HZ,
        .flags.with_dma = LED_STRIP_WITH_DMA,
    };
    b_success &= !led_strip_new_rmt_device(&strip_config, &rmt_config, &s_led_strip);
    if(!b_success || (s_led_strip == NULL)){
        #if CUSTOM_GPIO_INIT_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_init() - LED_STRIP_GPIO_NUM[%d] led_strip_new_rmt_device() 설정 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, LED_STRIP_GPIO_NUM);
        #endif
        return b_success;
    }
    b_A_gpio_states[LED_STRIP_GPIO_NUM] = true;

    #if CUSTOM_GPIO_INIT_DEBUG
    printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - b_success : %d\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, b_success);
    #endif

    return b_success;
}

bool custom_gpio_set_led_strip_color(const uint32_t input_ui32_red_value, const uint32_t input_ui32_green_value, const uint32_t input_ui32_blue_value){
    #define CUSTOM_GPIO_SET_LED_STRIP_COLOR_DEBUG   GPIO_DEBUG
    
    static bool b_success = true;

    #if CUSTOM_GPIO_SET_LED_STRIP_COLOR_DEBUG
    printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_set_led_strip_color() - LED Strip Color 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG);
    printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_set_led_strip_color() - input_ui32_red_value : %ld | input_ui32_green_value : %ld | input_ui32_blue_value : %ld \n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, input_ui32_red_value, input_ui32_green_value, input_ui32_blue_value);
    #endif
    b_success &= !led_strip_set_pixel(s_led_strip, 0, input_ui32_red_value, input_ui32_green_value, input_ui32_blue_value);
    if(!b_success){
        #if CUSTOM_GPIO_SET_LED_STRIP_COLOR_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_set_led_strip_color() - led_strip_set_pixel() 설정 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG);
        #endif
        return b_success;
    }
    b_success &= !led_strip_refresh(s_led_strip);
    if(!b_success){
        #if CUSTOM_GPIO_SET_LED_STRIP_COLOR_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_set_led_strip_color() - led_strip_refresh() 설정 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG);
        #endif
        return b_success;
    }
    return b_success;
}

bool custom_gpio_deinit(void){
    #define CUSTOM_GPIO_DEINIT_DEBUG         GPIO_DEBUG

    static bool b_success = true;

    if(b_A_gpio_states[NVS_RESET_BUTTON_GPIO_NUM]){
        // 1. NVS 리셋 버튼 GPIO 해제
        #if CUSTOM_GPIO_DEINIT_DEBUG
        printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_deinit() - NVS 초기화를 위한 NVS_RESET_BUTTON_GPIO_NUM[%d] 설정 해제(BOOT 버튼)\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, NVS_RESET_BUTTON_GPIO_NUM);
        #endif
        b_success &= !gpio_reset_pin(NVS_RESET_BUTTON_GPIO_NUM);
        if(!b_success){
            #if CUSTOM_GPIO_DEINIT_DEBUG
            printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_deinit() - NVS_RESET_BUTTON_GPIO_NUM[%d] gpio_reset_pin() 설정 해제 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, NVS_RESET_BUTTON_GPIO_NUM);
            #endif
            return b_success;
        }
        b_A_gpio_states[NVS_RESET_BUTTON_GPIO_NUM] = false;
    }

    if(b_A_gpio_states[PIR_OUTPUT_GPIO_NUM]){
        // 3. SENSOR 모드에서 사용된 GPIO 해제
        #if CUSTOM_GPIO_DEINIT_DEBUG
        printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_deinit() - 외부 PIR Signal Interrupt 수신을 위한 PIR_OUTPUT_GPIO_NUM[%d] 설정 해제\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, PIR_OUTPUT_GPIO_NUM);
        #endif
        b_success &= !gpio_reset_pin(PIR_OUTPUT_GPIO_NUM);
        if(!b_success){
            #if CUSTOM_GPIO_DEINIT_DEBUG
            printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_deinit() - PIR_OUTPUT_GPIO_NUM[%d] gpio_reset_pin() 설정 해제 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, PIR_OUTPUT_GPIO_NUM);
            #endif
            return b_success;
        }
        b_A_gpio_states[PIR_OUTPUT_GPIO_NUM] = false;
    }
    
    if(b_A_gpio_states[LED_STRIP_GPIO_NUM] || (s_led_strip != NULL)){
        // 2. LED 스트립 리소스 해제
        #if CUSTOM_GPIO_DEINIT_DEBUG
        printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_deinit() - s_led_strip 설정 해제\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG);
        #endif
        b_success &= !led_strip_del(s_led_strip);
        if(!b_success){
            #if CUSTOM_GPIO_DEINIT_DEBUG
            printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_deinit() - LED_STRIP_GPIO_NUM[%d] led_strip_del() s_led_strip 설정 해제 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, LED_STRIP_GPIO_NUM);
            #endif
            return b_success;
        }
        s_led_strip = NULL; // 핸들을 NULL로 설정하여 중복 해제 방지
        b_A_gpio_states[LED_STRIP_GPIO_NUM] = false;
    }

    return b_success;
}