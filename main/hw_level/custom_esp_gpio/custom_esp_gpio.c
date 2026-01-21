
/*
******************************************************************************
* File Name          : custom_esp_gpio.c
* Description        : GPIO Initialization for RC Car
******************************************************************************
* - Motor Pins (PWM capable)
* - Servo Pin (PWM)
* - LED Strip
* - Buttons
******************************************************************************
*/
#include "custom_esp_gpio.h"

#define GPIO_DEBUG  DEBUG
static const char *custom_esp_gpio_TAG = "[@]custom_esp_gpio.c";

// State tracking
static bool b_gpio_initialized = false;
static led_strip_handle_t s_led_strip = NULL;

bool custom_gpio_init(void){
    #if GPIO_DEBUG
    printf("[%s] [Start] custom_gpio_init()\n", custom_getRuntimeString());
    #endif

    bool b_success = true;

    // 1. NVS Reset Button (Input)
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << NVS_RESET_BUTTON_GPIO_NUM),
        .pull_down_en = 0,
        .pull_up_en = 1
    };
    gpio_config(&io_conf);

    // 2. Motor Pins are configured by LEDC driver in custom_esp_motor.c
    // We do NOT need to configure them as GPIO_MODE_OUTPUT here if LEDC takes over.
    // However, it's good practice to ensure they are in a known state or let the motor module handle it completely.
    // The previous implementation configured them as GPIO first, then LEDC.
    // I will let custom_esp_motor.c handle the pin configuration for motors.
    // But I will configure the LED Strip here.

    // 3. LED Strip
    #if GPIO_DEBUG
    printf("[%s] [Info] Configuring LED Strip on GPIO %d\n", custom_getRuntimeString(), LED_STRIP_GPIO_NUM);
    #endif
    
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_GPIO_NUM,
        .max_leds = LED_STRIP_LEN,
    };
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = LED_STRIP_RESOLUTION_HZ,
        .flags.with_dma = LED_STRIP_WITH_DMA,
    };

    esp_err_t err = led_strip_new_rmt_device(&strip_config, &rmt_config, &s_led_strip);
    if (err != ESP_OK) {
        printf("[%s] [Error] LED Strip init failed\n", custom_getRuntimeString());
        b_success = false;
    }

    b_gpio_initialized = b_success;

    #if GPIO_DEBUG
    printf("[%s] [Done] custom_gpio_init()\n", custom_getRuntimeString());
    #endif

    return b_success;
}

bool custom_gpio_set_led_strip_color(const uint32_t red, const uint32_t green, const uint32_t blue){
    if (!s_led_strip) return false;
    led_strip_set_pixel(s_led_strip, 0, red, green, blue);
    return (led_strip_refresh(s_led_strip) == ESP_OK);
}

bool custom_gpio_deinit(void){
    if (s_led_strip) {
        led_strip_del(s_led_strip);
        s_led_strip = NULL;
    }
    b_gpio_initialized = false;
    return true;
}
