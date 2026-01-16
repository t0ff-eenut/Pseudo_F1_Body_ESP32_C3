/**
 * @file        main.c
 * @brief       Pseudo F1 RC Car Body Control Unit (BCU) Firmware v1.0.0
 * @author      T0T
 * @date        2025-11-21
 * @version     1.0.0
 * 
 * @details     Pseudo F1 RC Car Body Control Unit (BCU) Firmware Code로써,
 *              ESP32-C3-MINI + Motor + Servo + UART 구조로 구성됨.
 */

#include "app_level_handle.h"         // HAL Level 통합 헤더 사용

typedef enum initial_list_enum {
    INIT_LIST_GPIO,
    INIT_LIST_NVS,
    INIT_LIST_MOTOR,
    INIT_LIST_SERVO,
    INIT_LIST_UART,
    INIT_LIST_CONTROL,
    INIT_LIST_END,
} ile;
static bool b_A_init_states[INIT_LIST_END] = {false,};

static bool initial(void);
static bool deinitial(void);

void app_main(void) {
    printf("\n");
    printf("===================================\n");
    printf("  Pseudo F1 RC Car BCU Firmware\n");
    printf("  Version: 1.0.0\n");
    printf("===================================\n\n");

    // Initialize all modules
    if (!initial()) {
        printf(COLOR_RED"[ERROR] Initialization failed!\n"COLOR_RESET);
        return;
    }

    printf(COLOR_GREEN"\n[SUCCESS] All modules initialized!\n\n"COLOR_RESET);

    // LED Strip - Green for ready
    custom_gpio_set_led_strip_color(0, 50, 0);

    // Main loop
    printf("[INFO] Entering main control loop...\n");
    while (1) {
        rccar_control_task();
        vTaskDelay(pdMS_TO_TICKS(10)); // 10ms cycle
    }
}

static bool initial(void) {
    bool b_success = true;

    // GPIO Init
    printf("[INIT] GPIO...\n");
    b_success &= custom_gpio_init();
    b_A_init_states[INIT_LIST_GPIO] = b_success;
    if (!b_success) {
        printf(COLOR_RED"[ERROR] GPIO init failed\n"COLOR_RESET);
        return false;
    }

    // NVS Init (optional)
    printf("[INIT] NVS...\n");
    b_success &= custom_nvs_init();
    b_A_init_states[INIT_LIST_NVS] = b_success;
    if (!b_success) {
        printf(COLOR_YELLOW"[WARN] NVS init failed, continuing...\n"COLOR_RESET);
        b_success = true;
    }

    // Motor Init
    printf("[INIT] Motor...\n");
    b_success &= custom_motor_init();
    b_A_init_states[INIT_LIST_MOTOR] = b_success;
    if (!b_success) {
        printf(COLOR_RED"[ERROR] Motor init failed\n"COLOR_RESET);
        return false;
    }

    // Servo Init
    printf("[INIT] Servo...\n");
    b_success &= custom_servo_init();
    b_A_init_states[INIT_LIST_SERVO] = b_success;
    if (!b_success) {
        printf(COLOR_RED"[ERROR] Servo init failed\n"COLOR_RESET);
        return false;
    }

    // UART Init
    printf("[INIT] UART...\n");
    b_success &= custom_uart_init();
    b_A_init_states[INIT_LIST_UART] = b_success;
    if (!b_success) {
        printf(COLOR_RED"[ERROR] UART init failed\n"COLOR_RESET);
        return false;
    }

    // Control Logic Init
    printf("[INIT] Control Logic...\n");
    b_success &= rccar_control_init();
    b_A_init_states[INIT_LIST_CONTROL] = b_success;
    if (!b_success) {
        printf(COLOR_RED"[ERROR] Control logic init failed\n"COLOR_RESET);
        return false;
    }

    return b_success;
}

static bool deinitial(void) {
    bool b_success = true;

    // Control logic doesn't need explicit deinit for now
    
    if (b_A_init_states[INIT_LIST_UART]) {
        b_success &= custom_uart_deinit();
    }
    if (b_A_init_states[INIT_LIST_SERVO]) {
        b_success &= custom_servo_deinit();
    }
    if (b_A_init_states[INIT_LIST_MOTOR]) {
        b_success &= custom_motor_deinit();
    }
    if (b_A_init_states[INIT_LIST_NVS]) {
        b_success &= custom_nvs_deinit();
    }
    if (b_A_init_states[INIT_LIST_GPIO]) {
        b_success &= custom_gpio_deinit();
    }

    return b_success;
}