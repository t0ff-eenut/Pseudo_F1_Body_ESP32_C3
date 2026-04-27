/*
******************************************************************************
* File Name          : custom_esp_motor.c
* Description        : DC 모터 제어 모듈
******************************************************************************
* 2개의 DC 모터(전륜, 후륜)를 각각 2개의 핀으로 제어합니다.
* 로직:
*  전진:   IN1 = PWM, IN2 = 0          (속도 비례 구동)
*  후진:   IN1 = 0,   IN2 = PWM
*  코스팅: IN1 = 0,   IN2 = 0          (관성 회전, 제동력 없음)
*  브레이크: IN1 = PWM, IN2 = PWM (동일 duty → 쇼트 브레이크)
*         duty=0:    0% 브레이크 (코스팅 동일)
*         duty=512:  50% 브레이크
*         duty=1023: 100% 쇼트 브레이크
******************************************************************************
*/

#include "custom_esp_motor.h"

#define MOTOR_DEBUG DEBUG

static mss s_motor_state = {
    .speed_front = 0,
    .speed_rear = 0,
    .dir_front = MOTOR_STOP,
    .dir_rear = MOTOR_STOP
};

static uint32_t speed_to_duty(int8_t speed) {
    if (speed < 0) speed = -speed;
    // 0-127 범위를 0-1023으로 매핑
    return (uint32_t)(speed * 1023 / 127);
}

static void config_pwm_channel(int gpio_num, ledc_channel_t channel) {
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = MOTOR_PWM_MODE,
        .channel        = channel,
        .timer_sel      = MOTOR_PWM_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = gpio_num,
        .duty           = 0, // 초기 듀티 0%
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel);
}

bool custom_motor_init(void) {
    #if MOTOR_DEBUG
    printf("[%s] [시작] custom_motor_init()\n", custom_getRuntimeString());
    #endif

    // 타이머 설정
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = MOTOR_PWM_MODE,
        .timer_num        = MOTOR_PWM_TIMER,
        .duty_resolution  = MOTOR_PWM_DUTY_RES,
        .freq_hz          = MOTOR_PWM_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    // 채널 설정
    config_pwm_channel(MOTOR_FRONT_IN1_GPIO_NUM, MOTOR_FRONT_IN1_CHANNEL);
    config_pwm_channel(MOTOR_FRONT_IN2_GPIO_NUM, MOTOR_FRONT_IN2_CHANNEL);
    config_pwm_channel(MOTOR_REAR_IN1_GPIO_NUM, MOTOR_REAR_IN1_CHANNEL);
    config_pwm_channel(MOTOR_REAR_IN2_GPIO_NUM, MOTOR_REAR_IN2_CHANNEL);

    custom_motor_stop_all();

    #if MOTOR_DEBUG
    printf("[%s] [완료] custom_motor_init()\n", custom_getRuntimeString());
    #endif
    return true;
}

static void set_motor_pwm(ledc_channel_t ch_in1, ledc_channel_t ch_in2, int8_t speed) {
    uint32_t duty = speed_to_duty(speed);
    
    if (speed > 0) {
        // 전진: IN1=PWM, IN2=0
        ledc_set_duty(MOTOR_PWM_MODE, ch_in1, duty);
        ledc_update_duty(MOTOR_PWM_MODE, ch_in1);
        ledc_set_duty(MOTOR_PWM_MODE, ch_in2, 0);
        ledc_update_duty(MOTOR_PWM_MODE, ch_in2);
    } else if (speed < 0) {
        // 후진: IN1=0, IN2=PWM
        ledc_set_duty(MOTOR_PWM_MODE, ch_in1, 0);
        ledc_update_duty(MOTOR_PWM_MODE, ch_in1);
        ledc_set_duty(MOTOR_PWM_MODE, ch_in2, duty);
        ledc_update_duty(MOTOR_PWM_MODE, ch_in2);
    } else {
        // 정지: IN1=0, IN2=0
        ledc_set_duty(MOTOR_PWM_MODE, ch_in1, 0);
        ledc_update_duty(MOTOR_PWM_MODE, ch_in1);
        ledc_set_duty(MOTOR_PWM_MODE, ch_in2, 0);
        ledc_update_duty(MOTOR_PWM_MODE, ch_in2);
    }
}

void custom_motor_set_front(int8_t speed) {
    s_motor_state.speed_front = speed;
    if (speed > 0) s_motor_state.dir_front = MOTOR_FORWARD;
    else if (speed < 0) s_motor_state.dir_front = MOTOR_BACKWARD;
    else s_motor_state.dir_front = MOTOR_STOP;

    set_motor_pwm(MOTOR_FRONT_IN1_CHANNEL, MOTOR_FRONT_IN2_CHANNEL, speed);
}

void custom_motor_set_rear(int8_t speed) {
    s_motor_state.speed_rear = speed;
    if (speed > 0) s_motor_state.dir_rear = MOTOR_FORWARD;
    else if (speed < 0) s_motor_state.dir_rear = MOTOR_BACKWARD;
    else s_motor_state.dir_rear = MOTOR_STOP;

    set_motor_pwm(MOTOR_REAR_IN1_CHANNEL, MOTOR_REAR_IN2_CHANNEL, speed);
}

static void set_motor_brake(ledc_channel_t ch_in1, ledc_channel_t ch_in2, int8_t strength) {
    // strength: 0~127  (0=코스팅, 127=100% 쇼트 브레이크)
    if (strength < 0) strength = 0;
    uint32_t duty = (uint32_t)(strength * 1023 / 127);
    // IN1=PWM, IN2=PWM (동일 duty) → HIGH 구간=브레이크, LOW 구간=코스팅
    ledc_set_duty(MOTOR_PWM_MODE, ch_in1, duty);
    ledc_update_duty(MOTOR_PWM_MODE, ch_in1);
    ledc_set_duty(MOTOR_PWM_MODE, ch_in2, duty);
    ledc_update_duty(MOTOR_PWM_MODE, ch_in2);
}

void custom_motor_brake_front(int8_t strength) {
    s_motor_state.dir_front = MOTOR_BRAKE;
    s_motor_state.speed_front = 0;
    set_motor_brake(MOTOR_FRONT_IN1_CHANNEL, MOTOR_FRONT_IN2_CHANNEL, strength);
}

void custom_motor_brake_rear(int8_t strength) {
    s_motor_state.dir_rear = MOTOR_BRAKE;
    s_motor_state.speed_rear = 0;
    set_motor_brake(MOTOR_REAR_IN1_CHANNEL, MOTOR_REAR_IN2_CHANNEL, strength);
}

void custom_motor_brake_all(int8_t strength) {
    custom_motor_brake_front(strength);
    custom_motor_brake_rear(strength);
}

void custom_motor_set_both(int8_t front_speed, int8_t rear_speed) {
    custom_motor_set_front(front_speed);
    custom_motor_set_rear(rear_speed);
}

void custom_motor_stop_all(void) {
    custom_motor_set_both(0, 0);
}

mss custom_motor_get_state(void) {
    return s_motor_state;
}

bool custom_motor_deinit(void) {
    custom_motor_stop_all();
    return true;
}
