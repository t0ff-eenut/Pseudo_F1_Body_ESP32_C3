

/*
******************************************************************************
* File Name          : custom_esp_gpio.c
* Description        : RC카용 GPIO 설정 및 제어
******************************************************************************
* L298N 모터 드라이버, 서보 모터, LED Strip 제어
******************************************************************************
* first update : 2025/12/04
******************************************************************************
*/
#include "custom_esp_gpio.h"

#define GPIO_DEBUG  DEBUG
static const char *custom_esp_gpio_TAG = "[@]custom_esp_gpio.c";

// GPIO Setting 여부
static bool b_A_gpio_states[40] = {false,};
static led_strip_handle_t s_led_strip = NULL;

bool custom_gpio_init(void){
    #define CUSTOM_GPIO_INIT_DEBUG         GPIO_DEBUG

    bool b_success = true;

    /////////////////////////////
    /// NVS 초기화 버튼 Initial ///
    /////////////////////////////
    #if CUSTOM_GPIO_INIT_DEBUG
    printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_init() - NVS_RESET_BUTTON_GPIO_NUM[%d] 설정(BOOT 버튼)\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_gpio_TAG, NVS_RESET_BUTTON_GPIO_NUM);
    #endif
    
    b_success &= !gpio_set_direction(NVS_RESET_BUTTON_GPIO_NUM, GPIO_MODE_INPUT);
    b_success &= !gpio_set_pull_mode(NVS_RESET_BUTTON_GPIO_NUM, GPIO_PULLUP_ONLY);
    if(!b_success){
        #if CUSTOM_GPIO_INIT_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_init() - NVS_RESET_BUTTON_GPIO_NUM 설정 실패\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_gpio_TAG);
        #endif
        return b_success;
    }
    b_A_gpio_states[NVS_RESET_BUTTON_GPIO_NUM] = true;

    ///////////////////////////////////
    /// Motor A Direction GPIO (IN1, IN2) ///
    ///////////////////////////////////
    #if CUSTOM_GPIO_INIT_DEBUG
    printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_init() - Motor A 방향 GPIO 설정 (IN1:%d, IN2:%d)\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_gpio_TAG, MOTOR_A_IN1_GPIO_NUM, MOTOR_A_IN2_GPIO_NUM);
    #endif
    
    static const gpio_config_t motor_a_dir_conf = {
        .pin_bit_mask = (1ULL << MOTOR_A_IN1_GPIO_NUM) | (1ULL << MOTOR_A_IN2_GPIO_NUM),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    b_success &= !gpio_config(&motor_a_dir_conf);
    if(!b_success){
        #if CUSTOM_GPIO_INIT_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_init() - Motor A 방향 GPIO 설정 실패\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_gpio_TAG);
        #endif
        return b_success;
    }
    // 초기 상태: 정지 (LOW, LOW)
    gpio_set_level(MOTOR_A_IN1_GPIO_NUM, 0);
    gpio_set_level(MOTOR_A_IN2_GPIO_NUM, 0);
    b_A_gpio_states[MOTOR_A_IN1_GPIO_NUM] = true;
    b_A_gpio_states[MOTOR_A_IN2_GPIO_NUM] = true;

    ///////////////////////////////////
    /// Motor B Direction GPIO (IN3, IN4) ///
    ///////////////////////////////////
    #if CUSTOM_GPIO_INIT_DEBUG
    printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_init() - Motor B 방향 GPIO 설정 (IN3:%d, IN4:%d)\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_gpio_TAG, MOTOR_B_IN3_GPIO_NUM, MOTOR_B_IN4_GPIO_NUM);
    #endif
    
    static const gpio_config_t motor_b_dir_conf = {
        .pin_bit_mask = (1ULL << MOTOR_B_IN3_GPIO_NUM) | (1ULL << MOTOR_B_IN4_GPIO_NUM),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    b_success &= !gpio_config(&motor_b_dir_conf);
    if(!b_success){
        #if CUSTOM_GPIO_INIT_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_init() - Motor B 방향 GPIO 설정 실패\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_gpio_TAG);
        #endif
        return b_success;
    }
    // 초기 상태: 정지 (LOW, LOW)
    gpio_set_level(MOTOR_B_IN3_GPIO_NUM, 0);
    gpio_set_level(MOTOR_B_IN4_GPIO_NUM, 0);
    b_A_gpio_states[MOTOR_B_IN3_GPIO_NUM] = true;
    b_A_gpio_states[MOTOR_B_IN4_GPIO_NUM] = true;

    ///////////////////////////////////
    /// Motor PWM (ENA, ENB) - LEDC ///
    ///////////////////////////////////
    #if CUSTOM_GPIO_INIT_DEBUG
    printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_init() - Motor PWM 타이머 설정\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_gpio_TAG);
    #endif
    
    ledc_timer_config_t motor_timer = {
        .speed_mode       = MOTOR_PWM_MODE,
        .timer_num        = MOTOR_PWM_TIMER,
        .duty_resolution  = MOTOR_PWM_DUTY_RES,
        .freq_hz          = MOTOR_PWM_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    b_success &= !ledc_timer_config(&motor_timer);

    // Motor A PWM Channel (ENA)
    #if CUSTOM_GPIO_INIT_DEBUG
    printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_init() - Motor A PWM 채널 설정 (ENA:%d)\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_gpio_TAG, MOTOR_A_ENA_GPIO_NUM);
    #endif
    ledc_channel_config_t motor_a_channel = {
        .speed_mode     = MOTOR_PWM_MODE,
        .channel        = MOTOR_A_PWM_CHANNEL,
        .timer_sel      = MOTOR_PWM_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = MOTOR_A_ENA_GPIO_NUM,
        .duty           = 0,
        .hpoint         = 0
    };
    b_success &= !ledc_channel_config(&motor_a_channel);
    b_A_gpio_states[MOTOR_A_ENA_GPIO_NUM] = true;

    // Motor B PWM Channel (ENB)
    #if CUSTOM_GPIO_INIT_DEBUG
    printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_init() - Motor B PWM 채널 설정 (ENB:%d)\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_gpio_TAG, MOTOR_B_ENB_GPIO_NUM);
    #endif
    ledc_channel_config_t motor_b_channel = {
        .speed_mode     = MOTOR_PWM_MODE,
        .channel        = MOTOR_B_PWM_CHANNEL,
        .timer_sel      = MOTOR_PWM_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = MOTOR_B_ENB_GPIO_NUM,
        .duty           = 0,
        .hpoint         = 0
    };
    b_success &= !ledc_channel_config(&motor_b_channel);
    b_A_gpio_states[MOTOR_B_ENB_GPIO_NUM] = true;

    if(!b_success){
        #if CUSTOM_GPIO_INIT_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_init() - Motor PWM 설정 실패\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_gpio_TAG);
        #endif
        return b_success;
    }

    ///////////////////////////////////
    /// Servo PWM - LEDC ///
    ///////////////////////////////////
    #if CUSTOM_GPIO_INIT_DEBUG
    printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_init() - Servo PWM 타이머 설정\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_gpio_TAG);
    #endif
    
    ledc_timer_config_t servo_timer = {
        .speed_mode       = SERVO_PWM_MODE,
        .timer_num        = SERVO_PWM_TIMER,
        .duty_resolution  = SERVO_PWM_DUTY_RES,
        .freq_hz          = SERVO_PWM_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    b_success &= !ledc_timer_config(&servo_timer);

    #if CUSTOM_GPIO_INIT_DEBUG
    printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_init() - Servo PWM 채널 설정 (GPIO:%d)\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_gpio_TAG, SERVO_PWM_GPIO_NUM);
    #endif
    ledc_channel_config_t servo_channel = {
        .speed_mode     = SERVO_PWM_MODE,
        .channel        = SERVO_PWM_CHANNEL,
        .timer_sel      = SERVO_PWM_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = SERVO_PWM_GPIO_NUM,
        .duty           = 0,
        .hpoint         = 0
    };
    b_success &= !ledc_channel_config(&servo_channel);
    b_A_gpio_states[SERVO_PWM_GPIO_NUM] = true;

    if(!b_success){
        #if CUSTOM_GPIO_INIT_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_init() - Servo PWM 설정 실패\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_gpio_TAG);
        #endif
        return b_success;
    }

    //////////////////////////////
    /// LED Strip GPIO Initial ///
    //////////////////////////////
    #if CUSTOM_GPIO_INIT_DEBUG
    printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_init() - LED Strip 설정 (GPIO:%d)\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_gpio_TAG, LED_STRIP_GPIO_NUM);
    #endif
    
    static const led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_GPIO_NUM,
        .max_leds = LED_STRIP_LEN,
    };
    static const led_strip_rmt_config_t rmt_config = {
        .resolution_hz = LED_STRIP_RESOLUTION_HZ,
        .flags.with_dma = LED_STRIP_WITH_DMA,
    };
    b_success &= !led_strip_new_rmt_device(&strip_config, &rmt_config, &s_led_strip);
    if(!b_success || (s_led_strip == NULL)){
        #if CUSTOM_GPIO_INIT_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_init() - LED Strip 설정 실패\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_gpio_TAG);
        #endif
        return b_success;
    }
    b_A_gpio_states[LED_STRIP_GPIO_NUM] = true;

    #if CUSTOM_GPIO_INIT_DEBUG
    printf("[%s] "COLOR_GREEN"[완료-DONE]\t %s custom_gpio_init() - GPIO 초기화 완료\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_gpio_TAG);
    #endif

    return b_success;
}

bool custom_gpio_set_led_strip_color(const uint32_t input_ui32_red_value, const uint32_t input_ui32_green_value, const uint32_t input_ui32_blue_value){
    #define CUSTOM_GPIO_SET_LED_STRIP_COLOR_DEBUG   GPIO_DEBUG
    
    bool b_success = true;

    #if CUSTOM_GPIO_SET_LED_STRIP_COLOR_DEBUG
    printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_set_led_strip_color() - R:%ld G:%ld B:%ld\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_gpio_TAG, input_ui32_red_value, input_ui32_green_value, input_ui32_blue_value);
    #endif
    
    b_success &= !led_strip_set_pixel(s_led_strip, 0, input_ui32_red_value, input_ui32_green_value, input_ui32_blue_value);
    b_success &= !led_strip_refresh(s_led_strip);
    
    return b_success;
}

bool custom_gpio_deinit(void){
    #define CUSTOM_GPIO_DEINIT_DEBUG         GPIO_DEBUG

    bool b_success = true;

    // NVS Reset Button
    if(b_A_gpio_states[NVS_RESET_BUTTON_GPIO_NUM]){
        b_success &= !gpio_reset_pin(NVS_RESET_BUTTON_GPIO_NUM);
        b_A_gpio_states[NVS_RESET_BUTTON_GPIO_NUM] = false;
    }

    // Motor A Direction
    if(b_A_gpio_states[MOTOR_A_IN1_GPIO_NUM]){
        b_success &= !gpio_reset_pin(MOTOR_A_IN1_GPIO_NUM);
        b_A_gpio_states[MOTOR_A_IN1_GPIO_NUM] = false;
    }
    if(b_A_gpio_states[MOTOR_A_IN2_GPIO_NUM]){
        b_success &= !gpio_reset_pin(MOTOR_A_IN2_GPIO_NUM);
        b_A_gpio_states[MOTOR_A_IN2_GPIO_NUM] = false;
    }

    // Motor B Direction
    if(b_A_gpio_states[MOTOR_B_IN3_GPIO_NUM]){
        b_success &= !gpio_reset_pin(MOTOR_B_IN3_GPIO_NUM);
        b_A_gpio_states[MOTOR_B_IN3_GPIO_NUM] = false;
    }
    if(b_A_gpio_states[MOTOR_B_IN4_GPIO_NUM]){
        b_success &= !gpio_reset_pin(MOTOR_B_IN4_GPIO_NUM);
        b_A_gpio_states[MOTOR_B_IN4_GPIO_NUM] = false;
    }

    // Motor PWM - LEDC stop
    ledc_stop(MOTOR_PWM_MODE, MOTOR_A_PWM_CHANNEL, 0);
    ledc_stop(MOTOR_PWM_MODE, MOTOR_B_PWM_CHANNEL, 0);
    b_A_gpio_states[MOTOR_A_ENA_GPIO_NUM] = false;
    b_A_gpio_states[MOTOR_B_ENB_GPIO_NUM] = false;

    // Servo PWM - LEDC stop
    ledc_stop(SERVO_PWM_MODE, SERVO_PWM_CHANNEL, 0);
    b_A_gpio_states[SERVO_PWM_GPIO_NUM] = false;

    // LED Strip
    if(b_A_gpio_states[LED_STRIP_GPIO_NUM] || (s_led_strip != NULL)){
        b_success &= !led_strip_del(s_led_strip);
        s_led_strip = NULL;
        b_A_gpio_states[LED_STRIP_GPIO_NUM] = false;
    }

    #if CUSTOM_GPIO_DEINIT_DEBUG
    printf("[%s] "COLOR_GREEN"[완료-DONE]\t %s custom_gpio_deinit() - GPIO 해제 완료\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_gpio_TAG);
    #endif

    return b_success;
}