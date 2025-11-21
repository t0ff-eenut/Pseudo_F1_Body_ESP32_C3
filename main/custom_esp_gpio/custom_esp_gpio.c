/*
******************************************************************************
* File Name          : custom_esp_gpio.c
* Description        : ESP32의 GPIO 설정 및 제어
******************************************************************************
* ESP32의 GPIO 설정 및 제어를 위한 코드
* AC Motor Driver 제어 / Servo Motor 제어 / LED 제어 모듈
******************************************************************************

******************************************************************************
* first update : 2025/11/21
******************************************************************************
* final update : 2025/11/21
******************************************************************************
*/
#include "custom_esp_gpio/custom_esp_gpio.h"

#define GPIO_DEBUG  DEBUG_PRINT
static const char *custom_esp_gpio_TAG = "[@]custom_esp_gpio.c";

// GPIO Setting 여부
static bool b_A_gpio_states[40] = {false,};

// 성공하면 true, 실패하면 false 반환
const bool custom_gpio_init(void){
    #define CUSTOM_GPIO_INIT_DEBUG         GPIO_DEBUG

    static bool b_success = true;

// /// NVS 초기화 버튼 ///
//     #if CUSTOM_GPIO_INIT_DEBUG
//     printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_init() - NVS 초기화를 위한 NVS_RESET_BUTTON_GPIO_NUM[%d] 설정(BOOT 버튼)\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, NVS_RESET_BUTTON_GPIO_NUM);
//     printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - NVS_RESET_BUTTON_GPIO_NUM[%d] GPIO_MODE_INPUT 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, NVS_RESET_BUTTON_GPIO_NUM);
//     #endif
//     // b_success = gpio_set_direction(NVS_RESET_BUTTON_GPIO_NUM, GPIO_MODE_INPUT) == ESP_OK ? true : false;
//     b_success &= !gpio_set_direction(NVS_RESET_BUTTON_GPIO_NUM, GPIO_MODE_INPUT);
//     if(!b_success){
//         #if CUSTOM_GPIO_INIT_DEBUG
//         printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_init() - NVS_RESET_BUTTON_GPIO_NUM[%d] GPIO_MODE_INPUT 설정 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, NVS_RESET_BUTTON_GPIO_NUM);
//         #endif
//         return b_success;
//     }
//     #if CUSTOM_GPIO_INIT_DEBUG
//     printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - NVS_RESET_BUTTON_GPIO_NUM[%d] - GPIO_PULLUP_ONLY 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, NVS_RESET_BUTTON_GPIO_NUM);
//     #endif
//     // b_success = gpio_set_pull_mode(NVS_RESET_BUTTON_GPIO_NUM, GPIO_PULLUP_ONLY) == ESP_OK ? true : false;
//     b_success &= !gpio_set_pull_mode(NVS_RESET_BUTTON_GPIO_NUM, GPIO_PULLUP_ONLY);
//     if(!b_success){
//         #if CUSTOM_GPIO_INIT_DEBUG
//         printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_init() - NVS_RESET_BUTTON_GPIO_NUM[%d] GPIO_PULLUP_ONLY 설정 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, NVS_RESET_BUTTON_GPIO_NUM);
//         #endif
//         return b_success;
//     }
//     b_A_gpio_states[NVS_RESET_BUTTON_GPIO_NUM] = true;
// /// NVS 초기화 버튼 ///

    #if CUSTOM_GPIO_INIT_DEBUG
    printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_init() - 외부 PIR Signal Interrupt 수신을 위한 PIR_OUTPUT_GPIO_NUM[%d] 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, PIR_OUTPUT_GPIO_NUM);
    printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - GPIO_MODE_INPUT 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG);
    printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - GPIO_PULLUP_DISABLE 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG);
    printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - GPIO_PULLDOWN_ENABLE 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG);
    printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - GPIO_INTR_DISABLE 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG);
    #endif
    // gpio_set_direction(PIR_OUTPUT_GPIO_NUM, GPIO_MODE_INPUT);
    // gpio_set_pull_mode(PIR_OUTPUT_GPIO_NUM, GPIO_PULLDOWN_ONLY);
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


    #if MODE == SWITCH
        #if CUSTOM_GPIO_INIT_DEBUG
        printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_init() - 외부 Button Signal Interrupt 수신을 위한 EXT_SWITCH_BUTTON_GPIO_NUM[%d] 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, EXT_SWITCH_BUTTON_GPIO_NUM);
        printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - GPIO_MODE_INPUT 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG);
        printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - GPIO_PULLUP_DISABLE 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG);
        printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - GPIO_PULLDOWN_ENABLE 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG);
        printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - GPIO_INTR_DISABLE 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG);
        #endif
        // 외부 버튼    1(3.3v):ON, 0(GND):OFF
        static const gpio_config_t ext_btn_io_conf = {
            .pin_bit_mask = (1ULL << EXT_SWITCH_BUTTON_GPIO_NUM),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_ENABLE,
            .intr_type = GPIO_INTR_DISABLE
        };
        b_success &= !gpio_config(&ext_btn_io_conf);
        if(!b_success){
            #if CUSTOM_GPIO_INIT_DEBUG
            printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_init() - EXT_SWITCH_BUTTON_GPIO_NUM[%d] gpio_config() 설정 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, EXT_SWITCH_BUTTON_GPIO_NUM);
            #endif
            return b_success;
        }
        b_A_gpio_states[EXT_SWITCH_BUTTON_GPIO_NUM] = true;

        #if CUSTOM_GPIO_INIT_DEBUG
        printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_init() - Servo Motor Control을 위해 SERVO_MOTOR_GPIO_NUM[%d] PWM 타이머 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, SERVO_MOTOR_GPIO_NUM);
        printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - LEDC_LOW_SPEED_MODE[%d]\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, LEDC_LOW_SPEED_MODE);
        printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - LEDC_TIMER_14_BIT[%d]\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, LEDC_TIMER_14_BIT);
        printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - LEDC_TIMER_0[%d]\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, LEDC_TIMER_0);
        printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - freq_hz[%d]\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, 50);
        printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - LEDC_AUTO_CLK[%d]\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, LEDC_AUTO_CLK);
        #endif
        // PWM 타이머 설정 (타이머 설정 - 박자의 규칙 정하기)
        static const ledc_timer_config_t ledc_timer = {
            .speed_mode       = LEDC_LOW_SPEED_MODE,
            .duty_resolution  = LEDC_TIMER_14_BIT,
            .timer_num        = LEDC_TIMER_0,
            .freq_hz          = 50,  // 50Hz (서보모터 주기)
            .clk_cfg          = LEDC_AUTO_CLK
        };
        b_success &= !ledc_timer_config(&ledc_timer);
        if(!b_success){
            #if CUSTOM_GPIO_INIT_DEBUG
            printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_init() - SERVO_MOTOR_GPIO_NUM[%d] ledc_timer_config() 설정 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, SERVO_MOTOR_GPIO_NUM);
            #endif
            return b_success;
        }
        #if CUSTOM_GPIO_INIT_DEBUG
        printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_init() - Servo Motor Control을 위해 SERVO_MOTOR_GPIO_NUM[%d] PWM 채널 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, SERVO_MOTOR_GPIO_NUM);
        printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - LEDC_LOW_SPEED_MODE[%d]\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, LEDC_LOW_SPEED_MODE);
        printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - LEDC_CHANNEL_0[%d]\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, LEDC_CHANNEL_0);
        printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - LEDC_TIMER_0[%d]\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, LEDC_TIMER_0);
        printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - LEDC_INTR_DISABLE[%d]\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, LEDC_INTR_DISABLE);
        printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - SERVO_MOTOR_GPIO_NUM[%d]\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, SERVO_MOTOR_GPIO_NUM);
        printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - duty[%d]\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, 0);
        printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - hpoint[%d]\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, 0);
        #endif
        // PWM 채널 설정 (채널 설정 - 실제 박수치기)
        static const ledc_channel_config_t servo_motor_channel_config = {
            .speed_mode     = LEDC_LOW_SPEED_MODE,
            .channel        = LEDC_CHANNEL_0,
            .timer_sel      = LEDC_TIMER_0,
            .intr_type      = LEDC_INTR_DISABLE,
            .gpio_num       = SERVO_MOTOR_GPIO_NUM,
            .duty           = 0,
            .hpoint         = 0
        };
        b_success &= !ledc_channel_config(&servo_motor_channel_config);
        if(!b_success){
            #if CUSTOM_GPIO_INIT_DEBUG
            printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_init() - SERVO_MOTOR_GPIO_NUM[%d] ledc_channel_config() 설정 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, SERVO_MOTOR_GPIO_NUM);
            #endif
            return b_success;
        }
        b_A_gpio_states[SERVO_MOTOR_GPIO_NUM] = true;
    #endif
    
    /// LED ///
    #if CUSTOM_GPIO_INIT_DEBUG
    printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_init() - RGB LED Control을 위해 LED_STRIP_GPIO_NUM[%d] Strip Config 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, LED_STRIP_GPIO_NUM);
    printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - strip_gpio_num - LED_STRIP_GPIO_NUM[%d]\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, LED_STRIP_GPIO_NUM);
    printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - max_leds - LED_STRIP_LEN[%d]\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, LED_STRIP_LEN);
    #endif
    static const led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_GPIO_NUM,
        .max_leds = LED_STRIP_LEN, // LED 개수
    };
    #if CUSTOM_GPIO_INIT_DEBUG
    printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_init() - RGB LED Control을 위해 LED_STRIP_GPIO_NUM[%d] 제어 Config 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, LED_STRIP_GPIO_NUM);
    printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - resolution_hz[%d]\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, 10 * 1000 * 1000);
    printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - with_dma[%d]\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, false);
    #endif
    static const led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000,  // 10MHz
        .flags.with_dma = false,
    };
    b_success &= !led_strip_new_rmt_device(&strip_config, &rmt_config, &s_led_strip);
    if(!b_success || (s_led_strip == NULL)){
        #if CUSTOM_GPIO_INIT_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_init() - LED_STRIP_GPIO_NUM[%d] led_strip_new_rmt_device() 설정 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, LED_STRIP_GPIO_NUM);
        #endif
        return b_success;
    }
    b_A_gpio_states[LED_STRIP_GPIO_NUM] = true;
    /// LED ///

    #if CUSTOM_GPIO_INIT_DEBUG
    printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_gpio_init() - b_success : %d\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, b_success);
    #endif

    return b_success;
}

#if MODE == SWITCH

    static const uint32_t custom_gpio_servo_angle_to_duty(int input_i8_angle_value){
        // | 1ms | 1.5ms | 2ms | ----------- 20ms|
        // | 0°  | 90°   | 180°|
        // 실제 출력 0.5ms ~ 2.5ms
        // 타겟 0.36 ~ 2.62ms

        static const uint32_t ui32_duty_us   = (SERVO_PULSE_US_MIN + ((SERVO_PULSE_US_MAX - SERVO_PULSE_US_MIN) * input_i8_angle_value) / SERVO_MAX_ANGLE);
        static const uint32_t ui32_duty      = ((ui32_duty_us * (1 << SERVO_ADC_CHENAL)) / SERVO_PULSE_CYCLE);

        return ui32_duty;
    }

    const bool custom_gpio_servo_angle_set(uint8_t input_i8_angle_value){
        #define CUSTOM_GPIO_SERVO_ANGLE_SET_DEBUG         GPIO_DEBUG

        static bool             b_success       = true;
        static const uint32_t   ui32_duty_us    = custom_gpio_servo_angle_to_duty(input_i8_angle_value);

        b_success &= !ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, ui32_duty_us);
        if(!b_success){
            #if CUSTOM_GPIO_SERVO_ANGLE_SET_DEBUG
            printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_servo_angle_set() - ledc_set_duty() 설정 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG);
            printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_servo_angle_set() - ui32_duty_us : %lu\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, ui32_duty_us);
            #endif
            return b_success;
        }
        b_success &= !ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        if(!b_success){
            #if CUSTOM_GPIO_SERVO_ANGLE_SET_DEBUG
            printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_servo_angle_set() - ledc_update_duty() 설정 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG);
            #endif
            return b_success;
        }
        vTaskDelay(ms_to_delay(SERVO_WORK_TIME_MS));
        return b_success;
    }

    const bool custom_gpio_switch_control(int8_t input_i8_switch_command_value){
        #define CUSTOM_GPIO_SWITCH_CONTROL_DEBUG         GPIO_DEBUG
        static bool b_success = true;

        // ON : 0도, OFF : 180도
        if(input_i8_switch_command_value == SWITCH_STATUS_ON){
            #if CUSTOM_GPIO_SWITCH_CONTROL_DEBUG
            printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_switch_control() - SWITCH_ON[%02X]\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, input_i8_switch_command_value);
            #endif
            b_success &= !custom_gpio_servo_angle_set(SERVO_ON_ANGLE);
            if(!b_success){
                #if CUSTOM_GPIO_SWITCH_CONTROL_DEBUG
                printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_switch_control() - custom_gpio_servo_angle_set(SERVO_ON_ANGLE) 설정 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG);
                #endif
                return b_success;
            }
            ui8_switch_status = SWITCH_STATUS_ON;
        }
        else if(input_i8_switch_command_value == SWITCH_STATUS_OFF){
            #if CUSTOM_GPIO_SWITCH_CONTROL_DEBUG
            printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_switch_control() - SWITCH_OFF[%02X]\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, input_i8_switch_command_value);
            #endif
            custom_gpio_servo_angle_set(SERVO_OFF_ANGLE);
            if(!b_success){
                #if CUSTOM_GPIO_SWITCH_CONTROL_DEBUG
                printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_switch_control() - custom_gpio_servo_angle_set(SERVO_OFF_ANGLE) 설정 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG);
                #endif
                return b_success;
            }
            ui8_switch_status = SWITCH_STATUS_OFF;
        }
        else{
            #if CUSTOM_GPIO_SWITCH_CONTROL_DEBUG
            printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_switch_control() - input_i8_switch_command_value Error[%02X]\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, input_i8_switch_command_value);
            #endif
            b_success = false;
        }

        return b_success;
    }
    //////////////////// MOTOR ////////////////////
#endif

//////////////////// LED ////////////////////
const bool custom_gpio_set_led_strip_color(uint32_t input_ui32_red_value, uint32_t input_ui32_green_value, uint32_t input_ui32_blue_value){
    static bool b_success = true;

    b_success &= !led_strip_set_pixel(s_led_strip, 0, input_ui32_red_value, input_ui32_green_value, input_ui32_blue_value);
    if(!b_success){
        #if CUSTOM_GPIO_DEINIT_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_set_led_strip_color() - led_strip_set_pixel() 설정 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG);
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_set_led_strip_color() - input_ui32_red_value : %d input_ui32_green_value : %d input_ui32_blue_value : %d \n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, input_ui32_red_value, input_ui32_green_value, input_ui32_blue_value);
        #endif
        return b_success;
    }
    b_success &= !led_strip_refresh(s_led_strip);
    if(!b_success){
        #if CUSTOM_GPIO_DEINIT_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_set_led_strip_color() - led_strip_refresh() 설정 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG);
        #endif
        return b_success;
    }
    return b_success;
}

// void custom_gpio_clear_led_strip(void){
//     led_strip_clear(s_led_strip);
//     // vTaskDelay(1000 / portTICK_PERIOD_MS);
// }
//////////////////// LED ////////////////////

const bool custom_gpio_deinit(void){
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

    #if MODE == SENSOR
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
    #endif

    #if MODE == SWITCH
        if(b_A_gpio_states[EXT_SWITCH_BUTTON_GPIO_NUM]){
            // 4. SWITCH 모드에서 사용된 GPIO 및 주변기기 해제
            #if CUSTOM_GPIO_DEINIT_DEBUG
            printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_deinit() -외부 Button Signal Interrupt 수신을 위한 EXT_SWITCH_BUTTON_GPIO_NUM[%d] 설정 해제\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, EXT_SWITCH_BUTTON_GPIO_NUM);
            #endif
            gpio_reset_pin(EXT_SWITCH_BUTTON_GPIO_NUM);

            b_success &= !gpio_reset_pin(EXT_SWITCH_BUTTON_GPIO_NUM);
            if(!b_success){
                #if CUSTOM_GPIO_DEINIT_DEBUG
                printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_deinit() - EXT_SWITCH_BUTTON_GPIO_NUM[%d] gpio_reset_pin() 설정 해제 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, EXT_SWITCH_BUTTON_GPIO_NUM);
                #endif
                return b_success;
            }
            b_A_gpio_states[EXT_SWITCH_BUTTON_GPIO_NUM] = false;
        }

        if(b_A_gpio_states[SERVO_MOTOR_GPIO_NUM]){
            // 서보모터 PWM 정지
            #if CUSTOM_GPIO_DEINIT_DEBUG
            printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_gpio_deinit() - SERVO_MOTOR_GPIO_NUM[%d] ledc_stop() Servo Motor PWM 정지 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, SERVO_MOTOR_GPIO_NUM);
            #endif
            b_success &= !ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
            if(!b_success){
                #if CUSTOM_GPIO_DEINIT_DEBUG
                printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_gpio_deinit() - SERVO_MOTOR_GPIO_NUM[%d] ledc_stop() Servo Motor PWM 정지 설정 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_gpio_TAG, SERVO_MOTOR_GPIO_NUM);
                #endif
                return b_success;
            }
            b_A_gpio_states[SERVO_MOTOR_GPIO_NUM] = false;
        }
    #endif

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

// // ISR 함수
// static void IRAM_ATTR gpio_isr_handler(void* arg) {
//     int pin = (int)arg;  // 어떤 GPIO에서 인터럽트가 발생했는지 확인 가능
//     ESP_EARLY_LOGI(TAG, "Interrupt on GPIO %d", pin);
// }