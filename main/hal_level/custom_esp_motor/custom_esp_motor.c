/*
******************************************************************************
* File Name          : custom_esp_motor.c
* Description        : DC Motor Control Module for L298N Driver
******************************************************************************
* L298N Motor Driver Control using ESP32 LEDC (PWM)
* Motor A (Left)  : ENA(GPIO2), IN1(GPIO3), IN2(GPIO4)
* Motor B (Right) : ENB(GPIO10), IN3(GPIO5), IN4(GPIO6)
******************************************************************************
* first update : 2025/12/04
******************************************************************************
*/

#include "custom_esp_motor.h"

#define MOTOR_DEBUG  DEBUG
static const char *custom_esp_motor_TAG = "[@]custom_esp_motor.c";

// Current motor state
static mss s_motor_state = {
    .i8_speed_left = 0,
    .i8_speed_right = 0,
    .mde_dir_left = MOTOR_STOP,
    .mde_dir_right = MOTOR_STOP
};

// Helper: Convert speed (-100~+100) to PWM duty (0~1023 for 10-bit)
static uint32_t speed_to_duty(int8_t i8_speed) {
    if (i8_speed < 0) i8_speed = -i8_speed;  // absolute value
    if (i8_speed > 100) i8_speed = 100;
    
    // Map 0-100 to 0-1023 (10-bit resolution)
    return (uint32_t)(i8_speed * 1023 / 100);
}

// Helper: Set motor A direction pins
static void set_motor_a_direction(mde direction) {
    switch (direction) {
        case MOTOR_FORWARD:
            gpio_set_level(MOTOR_A_IN1_GPIO_NUM, 1);
            gpio_set_level(MOTOR_A_IN2_GPIO_NUM, 0);
            break;
        case MOTOR_BACKWARD:
            gpio_set_level(MOTOR_A_IN1_GPIO_NUM, 0);
            gpio_set_level(MOTOR_A_IN2_GPIO_NUM, 1);
            break;
        case MOTOR_BRAKE:
            gpio_set_level(MOTOR_A_IN1_GPIO_NUM, 1);
            gpio_set_level(MOTOR_A_IN2_GPIO_NUM, 1);
            break;
        case MOTOR_STOP:
        default:
            gpio_set_level(MOTOR_A_IN1_GPIO_NUM, 0);
            gpio_set_level(MOTOR_A_IN2_GPIO_NUM, 0);
            break;
    }
}

// Helper: Set motor B direction pins
static void set_motor_b_direction(mde direction) {
    switch (direction) {
        case MOTOR_FORWARD:
            gpio_set_level(MOTOR_B_IN3_GPIO_NUM, 1);
            gpio_set_level(MOTOR_B_IN4_GPIO_NUM, 0);
            break;
        case MOTOR_BACKWARD:
            gpio_set_level(MOTOR_B_IN3_GPIO_NUM, 0);
            gpio_set_level(MOTOR_B_IN4_GPIO_NUM, 1);
            break;
        case MOTOR_BRAKE:
            gpio_set_level(MOTOR_B_IN3_GPIO_NUM, 1);
            gpio_set_level(MOTOR_B_IN4_GPIO_NUM, 1);
            break;
        case MOTOR_STOP:
        default:
            gpio_set_level(MOTOR_B_IN3_GPIO_NUM, 0);
            gpio_set_level(MOTOR_B_IN4_GPIO_NUM, 0);
            break;
    }
}

bool custom_motor_init(void) {
    #if MOTOR_DEBUG
    printf("[%s] "COLOR_WHITE"[Start]\t %s custom_motor_init()\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_motor_TAG);
    #endif
    
    // Reset motor state
    s_motor_state.i8_speed_left = 0;
    s_motor_state.i8_speed_right = 0;
    s_motor_state.mde_dir_left = MOTOR_STOP;
    s_motor_state.mde_dir_right = MOTOR_STOP;
    
    // Set initial state: stopped
    custom_motor_coast();
    
    #if MOTOR_DEBUG
    printf("[%s] "COLOR_GREEN"[Done]\t %s custom_motor_init() - Motor module initialized\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_motor_TAG);
    #endif
    
    return true;
}

void custom_motor_set_left(int8_t i8_speed) {
    #if MOTOR_DEBUG
    printf("[%s] "COLOR_WHITE"[Info]\t %s custom_motor_set_left() - speed: %d\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_motor_TAG, i8_speed);
    #endif
    
    // Clamp speed to valid range
    if (i8_speed > 100) i8_speed = 100;
    if (i8_speed < -100) i8_speed = -100;
    
    s_motor_state.i8_speed_left = i8_speed;
    
    // Determine direction
    if (i8_speed > 0) {
        s_motor_state.mde_dir_left = MOTOR_FORWARD;
        set_motor_a_direction(MOTOR_FORWARD);
    } else if (i8_speed < 0) {
        s_motor_state.mde_dir_left = MOTOR_BACKWARD;
        set_motor_a_direction(MOTOR_BACKWARD);
    } else {
        s_motor_state.mde_dir_left = MOTOR_STOP;
        set_motor_a_direction(MOTOR_STOP);
    }
    
    // Set PWM duty
    uint32_t duty = speed_to_duty(i8_speed);
    ledc_set_duty(MOTOR_PWM_MODE, MOTOR_A_PWM_CHANNEL, duty);
    ledc_update_duty(MOTOR_PWM_MODE, MOTOR_A_PWM_CHANNEL);
}

void custom_motor_set_right(int8_t i8_speed) {
    #if MOTOR_DEBUG
    printf("[%s] "COLOR_WHITE"[Info]\t %s custom_motor_set_right() - speed: %d\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_motor_TAG, i8_speed);
    #endif
    
    // Clamp speed to valid range
    if (i8_speed > 100) i8_speed = 100;
    if (i8_speed < -100) i8_speed = -100;
    
    s_motor_state.i8_speed_right = i8_speed;
    
    // Determine direction
    if (i8_speed > 0) {
        s_motor_state.mde_dir_right = MOTOR_FORWARD;
        set_motor_b_direction(MOTOR_FORWARD);
    } else if (i8_speed < 0) {
        s_motor_state.mde_dir_right = MOTOR_BACKWARD;
        set_motor_b_direction(MOTOR_BACKWARD);
    } else {
        s_motor_state.mde_dir_right = MOTOR_STOP;
        set_motor_b_direction(MOTOR_STOP);
    }
    
    // Set PWM duty
    uint32_t duty = speed_to_duty(i8_speed);
    ledc_set_duty(MOTOR_PWM_MODE, MOTOR_B_PWM_CHANNEL, duty);
    ledc_update_duty(MOTOR_PWM_MODE, MOTOR_B_PWM_CHANNEL);
}

void custom_motor_set_both(int8_t i8_left, int8_t i8_right) {
    #if MOTOR_DEBUG
    printf("[%s] "COLOR_WHITE"[Info]\t %s custom_motor_set_both() - L:%d R:%d\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_motor_TAG, i8_left, i8_right);
    #endif
    
    custom_motor_set_left(i8_left);
    custom_motor_set_right(i8_right);
}

void custom_motor_brake(void) {
    #if MOTOR_DEBUG
    printf("[%s] "COLOR_YELLOW"[Action]\t %s custom_motor_brake()\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_motor_TAG);
    #endif
    
    // Set direction to brake (both IN high)
    set_motor_a_direction(MOTOR_BRAKE);
    set_motor_b_direction(MOTOR_BRAKE);
    
    // Full duty for active braking
    ledc_set_duty(MOTOR_PWM_MODE, MOTOR_A_PWM_CHANNEL, 1023);
    ledc_update_duty(MOTOR_PWM_MODE, MOTOR_A_PWM_CHANNEL);
    ledc_set_duty(MOTOR_PWM_MODE, MOTOR_B_PWM_CHANNEL, 1023);
    ledc_update_duty(MOTOR_PWM_MODE, MOTOR_B_PWM_CHANNEL);
    
    s_motor_state.i8_speed_left = 0;
    s_motor_state.i8_speed_right = 0;
    s_motor_state.mde_dir_left = MOTOR_BRAKE;
    s_motor_state.mde_dir_right = MOTOR_BRAKE;
}

void custom_motor_coast(void) {
    #if MOTOR_DEBUG
    printf("[%s] "COLOR_YELLOW"[Action]\t %s custom_motor_coast()\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_motor_TAG);
    #endif
    
    // Set direction to stop (both IN low) - coast/free spin
    set_motor_a_direction(MOTOR_STOP);
    set_motor_b_direction(MOTOR_STOP);
    
    // Zero duty
    ledc_set_duty(MOTOR_PWM_MODE, MOTOR_A_PWM_CHANNEL, 0);
    ledc_update_duty(MOTOR_PWM_MODE, MOTOR_A_PWM_CHANNEL);
    ledc_set_duty(MOTOR_PWM_MODE, MOTOR_B_PWM_CHANNEL, 0);
    ledc_update_duty(MOTOR_PWM_MODE, MOTOR_B_PWM_CHANNEL);
    
    s_motor_state.i8_speed_left = 0;
    s_motor_state.i8_speed_right = 0;
    s_motor_state.mde_dir_left = MOTOR_STOP;
    s_motor_state.mde_dir_right = MOTOR_STOP;
}

mss custom_motor_get_state(void) {
    return s_motor_state;
}

bool custom_motor_deinit(void) {
    #if MOTOR_DEBUG
    printf("[%s] "COLOR_WHITE"[Info]\t %s custom_motor_deinit()\n" COLOR_RESET, custom_getRuntimeString(), custom_esp_motor_TAG);
    #endif
    
    // Stop motors before deinit
    custom_motor_coast();
    
    return true;
}
