/*
******************************************************************************
* File Name          : custom_esp_servo.c
* Description        : 서보 모터 제어 모듈
******************************************************************************
* ESP32 LEDC를 이용한 서보 PWM 제어
******************************************************************************
*/

#include "custom_esp_servo.h"

#define SERVO_DEBUG  DEBUG
static const char *custom_esp_servo_TAG = "[@]custom_esp_servo.c";

static int16_t s_current_angle = 0;

static uint32_t angle_to_duty(int16_t angle) {
    // 각도 제한 (-90 ~ +90)
    if (angle < -90) angle = -90;
    if (angle > 90) angle = 90;
    
    // -90..+90 범위를 MinPulse..MaxPulse로 매핑
    // 공식: Pulse = Center + (Angle/90) * (Max - Center)
    
    int32_t pulse_us = SERVO_CENTER_PULSE_US + (angle * (SERVO_MAX_PULSE_US - SERVO_MIN_PULSE_US) / 180);

    // LEDC Duty (13-bit, 8192)
    // Duty = (PulseUS / PeriodUS) * 8192
    // Period = 1/50Hz = 20000us
    
    uint32_t duty = (pulse_us * 8192) / 20000;
    return duty;
}

bool custom_servo_init(void) {
    #if SERVO_DEBUG
    printf("[%s] [시작] custom_servo_init()\n", custom_getRuntimeString());
    #endif
    
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = SERVO_PWM_MODE,
        .timer_num        = SERVO_PWM_TIMER,
        .duty_resolution  = SERVO_PWM_DUTY_RES,
        .freq_hz          = SERVO_PWM_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .speed_mode     = SERVO_PWM_MODE,
        .channel        = SERVO_PWM_CHANNEL,
        .timer_sel      = SERVO_PWM_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = SERVO_PWM_GPIO_NUM,
        .duty           = angle_to_duty(0),
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel);

    s_current_angle = 0;
    
    #if SERVO_DEBUG
    printf("[%s] [완료] custom_servo_init()\n", custom_getRuntimeString());
    #endif
    return true;
}

void custom_servo_set_angle(int16_t angle) {
    s_current_angle = angle;
    uint32_t duty = angle_to_duty(angle);
    ledc_set_duty(SERVO_PWM_MODE, SERVO_PWM_CHANNEL, duty);
    ledc_update_duty(SERVO_PWM_MODE, SERVO_PWM_CHANNEL);
}

int16_t custom_servo_get_angle(void) {
    return s_current_angle;
}

void custom_servo_center(void) {
    custom_servo_set_angle(0);
}

bool custom_servo_deinit(void) {
    custom_servo_center();
    return true;
}
