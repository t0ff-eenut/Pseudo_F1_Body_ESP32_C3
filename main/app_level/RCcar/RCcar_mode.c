/*
******************************************************************************
* File Name          : RCcar_mode.c
* Description        : RC Car Operation Mode Management Implementation
******************************************************************************
* Manages state transitions and mode-specific behaviors
******************************************************************************
* first update : 2025/12/04
******************************************************************************
*/

#include "RCcar_mode.h"

#define MODE_DEBUG  DEBUG
static const char *RCcar_mode_TAG = "[@]RCcar_mode.c";

static rme s_current_mode = MODE_IDLE;

bool rccar_mode_init(void) {
    #if MODE_DEBUG
    printf("[%s] "COLOR_WHITE"[Start]\t %s rccar_mode_init()\n" COLOR_RESET, custom_getRuntimeString(), RCcar_mode_TAG);
    #endif

    s_current_mode = MODE_IDLE;

    #if MODE_DEBUG
    printf("[%s] "COLOR_GREEN"[Done]\t %s Mode initialized to IDLE\n" COLOR_RESET, custom_getRuntimeString(), RCcar_mode_TAG);
    #endif

    return true;
}

void rccar_mode_set(rme mode) {
    if (s_current_mode == mode) return;

    #if MODE_DEBUG
    printf("[%s] "COLOR_YELLOW"[Change]\t %s Mode change: %d -> %d\n" COLOR_RESET, 
           custom_getRuntimeString(), RCcar_mode_TAG, s_current_mode, mode);
    #endif

    s_current_mode = mode;

    // Mode change actions
    switch (mode) {
        case MODE_IDLE:
            custom_motor_brake();
            custom_servo_center();
            custom_gpio_set_led_strip_color(50, 50, 0); // Yellow
            break;
        case MODE_MANUAL:
            custom_gpio_set_led_strip_color(0, 0, 50);  // Blue
            break;
        case MODE_AUTO:
            custom_gpio_set_led_strip_color(50, 0, 50); // Purple
            break;
        case MODE_ERROR:
            custom_motor_brake();
            custom_gpio_set_led_strip_color(255, 0, 0); // Red
            break;
        default:
            break;
    }
}

rme rccar_mode_get(void) {
    return s_current_mode;
}

void rccar_mode_task(void) {
    // Periodic tasks based on mode (if needed)
    // For now, simple LED blinking or status checks could go here
}
