/*
******************************************************************************
* File Name          : custom_esp_servo.c
* Description        : Servo Motor Control Module
******************************************************************************
* Servo PWM using ESP32 LEDC
* GPIO7, 50Hz, 500-2500us pulse width for -90~+90 degrees
* Limited to -45~+45 degrees for steering
******************************************************************************
* first update : 2025/12/04
******************************************************************************
*/

#include "custom_esp_servo.h"

#define SERVO_DEBUG  DEBUG
static const char *custom_esp_servo_TAG = "[@]custom_esp_servo.c";

// Current servo angle
static int16_t s_i16_current_angle = 0;

// Helper: Convert angle to PWM duty
// 50Hz = 20ms period, 13-bit resolution = 8192 steps
// 500us (min) = 8192 * 500 / 20000 = 205
// 1500us (center) = 8192 * 1500 / 20000 = 614
// 2500us (max) = 8192 * 2500 / 20000 = 1024
static uint32_t angle_to_duty(int16_t i16_angle) {
    // Clamp angle to -45 ~ +45
    if (i16_angle < -45) i16_angle = -45;
    if (i16_angle > 45) i16_angle = 45;
    
    // Map -45~+45 to 500us~2500us
    // -45 -> 1000us, 0 -> 1500us, +45 -> 2000us (narrower range for steering)
    // Actually, let's use full range for testing: -45 -> 750us, 0 -> 1500us, +45 -> 2250us
    uint32_t pulse_us = SERVO_CENTER_PULSE_US + (i16_angle * (SERVO_MAX_PULSE_US - SERVO_MIN_PULSE_US) / 180);
    
    // Convert pulse width to duty (13-bit, 50Hz = 20000us period)
    // duty = pulse_us * 8192 / 20000
    uint32_t duty = (pulse_us * 8192) / 20000;
    
    return duty;
}

bool custom_servo_init(void) {
    #if SERVO_DEBUG
    printf("[%s] "COLOR_WHITE"[Start]\t %s custom_servo_init()\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_servo_TAG);
    #endif
    
    // Center the servo
    s_i16_current_angle = 0;
    custom_servo_center();
    
    #if SERVO_DEBUG
    printf("[%s] "COLOR_GREEN"[Done]\t %s custom_servo_init() - Servo module initialized\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_servo_TAG);
    #endif
    
    return true;
}

void custom_servo_set_angle(int16_t i16_angle) {
    #if SERVO_DEBUG
    printf("[%s] "COLOR_WHITE"[Info]\t %s custom_servo_set_angle() - angle: %d\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_servo_TAG, i16_angle);
    #endif
    
    // Clamp angle
    if (i16_angle < -45) i16_angle = -45;
    if (i16_angle > 45) i16_angle = 45;
    
    s_i16_current_angle = i16_angle;
    
    // Set PWM duty
    uint32_t duty = angle_to_duty(i16_angle);
    ledc_set_duty(SERVO_PWM_MODE, SERVO_PWM_CHANNEL, duty);
    ledc_update_duty(SERVO_PWM_MODE, SERVO_PWM_CHANNEL);
}

int16_t custom_servo_get_angle(void) {
    return s_i16_current_angle;
}

void custom_servo_center(void) {
    #if SERVO_DEBUG
    printf("[%s] "COLOR_YELLOW"[Action]\t %s custom_servo_center()\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_servo_TAG);
    #endif
    
    custom_servo_set_angle(0);
}

bool custom_servo_deinit(void) {
    #if SERVO_DEBUG
    printf("[%s] "COLOR_WHITE"[Info]\t %s custom_servo_deinit()\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_servo_TAG);
    #endif
    
    // Center servo before deinit
    custom_servo_center();
    
    return true;
}
