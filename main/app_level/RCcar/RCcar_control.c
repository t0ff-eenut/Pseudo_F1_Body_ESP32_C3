#include "RCcar_control.h"
#include "esp_timer.h"

#define CONTROL_DEBUG DEBUG
static const char *TAG = "[@]RCcar_control";

// ─── Failsafe 상태 변수 ────────────────────────────────────────────────────
static uint32_t s_last_packet_ms  = 0;   // 마지막 유효 패킷 수신 시각 (ms)
static uint32_t s_last_step_ms    = 0;   // 마지막 페일세이프 스텝 시각 (ms)
static int8_t   s_current_speed   = 0;   // 현재 속도 (점진적 감속 추적용)
static int16_t  s_current_angle   = 0;   // 현재 서보 각도 (점진적 정렬 추적용)
// ──────────────────────────────────────────────────────────────────────────

bool rccar_control_init(void) {
    #if CONTROL_DEBUG
    printf("[%s] [시작] rccar_control_init()\n", custom_getRuntimeString());
    #endif

    rccar_protocol_init();

    // 모터 정지 및 서보 중앙 정렬 확인
    custom_motor_stop_all();
    custom_servo_center();

    #if CONTROL_DEBUG
    printf("[%s] [완료] rccar_control_init()\n", custom_getRuntimeString());
    #endif
    return true;
}

void rccar_control_process_byte(uint8_t byte) {
    rc_packet_t packet;
    if (rccar_protocol_parse_byte(byte, &packet)) {
        // 유효한 패킷 수신됨
        switch (packet.cmd) {
            case CMD_CONTROL: {
                /*
                 * [CMD_CONTROL 파이프라인]
                 *
                 * ┌─────────────────────────────────────────────────────────┐
                 * │ UART 수신 (int8_t, 1바이트)                              │
                 * │  param1: -127 ~ +127  (음수=후진,  양수=전진)            │
                 * │  param2: -127 ~ +127  (음수=좌조향, 양수=우조향)         │
                 * └────────────────────┬────────────────────────────────────┘
                 *                      │
                 *          ┌───────────┴───────────┐
                 *          ▼                       ▼
                 * [Throttle]                   [Steering]
                 * param1 그대로 전달            param2 * 90 / 127
                 *  -127 ~ +127                  -90° ~ +90°
                 *          │                       │
                 *          ▼                       ▼
                 * custom_motor_set_both()    custom_servo_set_angle()
                 *  speed_to_duty(speed)       angle_to_duty(angle)
                 *  |speed| * 1023 / 127       1500 + (angle * 2000 / 180) us
                 *  0 ~ 1023 (10-bit PWM)      500 ~ 2500 us
                 *          │                       │
                 *          ▼                       ▼
                 *   LEDC CH0/1/2/3            LEDC CH4
                 *   20kHz PWM                 50Hz PWM
                 *   GPIO 4,5,6,10             GPIO 7
                 * └─────────────────────────────────────────────────────────┘
                 */
                // Param1: 속도 (Throttle) -127 ~ +127 → 모터 직접 전달
                // Param2: 조향 (Steering) -127 ~ +127 → 서보 -90° ~ +90°으로 변환
                int16_t servo_angle = (int16_t)packet.param2 * 90 / 127;
                custom_motor_set_both(packet.param1, packet.param1);
                custom_servo_set_angle(servo_angle);
                // 페일세이프 상태 갱신
                s_current_speed = packet.param1;
                s_current_angle = servo_angle;
                s_last_packet_ms = (uint32_t)(esp_timer_get_time() / 1000);
                break;
            }

            case CMD_BRAKE:
                // Param1: 전륜 브레이크 강도 0~127
                // Param2: 후륜 브레이크 강도 0~127
                // 전/후륜 독립 제어 가능 (드리프트 시 후륜만 강하게 제동 등)
                custom_motor_brake_front(packet.param1);
                custom_motor_brake_rear(packet.param2);
                s_last_packet_ms = (uint32_t)(esp_timer_get_time() / 1000);
                break;

            case CMD_EMERGENCY_STOP:
                custom_motor_stop_all();
                s_current_speed  = 0;
                s_current_angle  = 0;
                s_last_packet_ms = (uint32_t)(esp_timer_get_time() / 1000);
                break;

            case CMD_HEARTBEAT:
                // 워치독 리셋 - 페일세이프 타이머 갱신
                s_last_packet_ms = (uint32_t)(esp_timer_get_time() / 1000);
                break;

            default:
                break;
        }
    }
}

void rccar_control_task(void) {
    // UART 버퍼의 모든 바이트를 처리
    uint8_t data;
    while (custom_uart_read_byte(&data)) {
        rccar_control_process_byte(data);
    }

    // ─── Failsafe: 점진적 감속 및 서보 중앙 정렬 ───────────────────────────
    uint32_t now_ms = (uint32_t)(esp_timer_get_time() / 1000);
    if (now_ms - s_last_packet_ms > FAILSAFE_TIMEOUT_MS) {
        // FAILSAFE_STEP_INTERVAL_MS 주기마다 한 단계씩 감속/정렬
        if (now_ms - s_last_step_ms >= FAILSAFE_STEP_INTERVAL_MS) {
            s_last_step_ms = now_ms;

            // 속도 점진적 감소 (0에 가까워질수록 MOTOR_STEP 이하면 0으로 클램프)
            if (s_current_speed > FAILSAFE_MOTOR_STEP) {
                s_current_speed -= FAILSAFE_MOTOR_STEP;
            } else if (s_current_speed < -FAILSAFE_MOTOR_STEP) {
                s_current_speed += FAILSAFE_MOTOR_STEP;
            } else {
                s_current_speed = 0;
            }
            custom_motor_set_both(s_current_speed, s_current_speed);

            // 서보 점진적 중앙 정렬 (0°에 가까워질수록 SERVO_STEP 이하면 0으로 클램프)
            if (s_current_angle > FAILSAFE_SERVO_STEP) {
                s_current_angle -= FAILSAFE_SERVO_STEP;
            } else if (s_current_angle < -FAILSAFE_SERVO_STEP) {
                s_current_angle += FAILSAFE_SERVO_STEP;
            } else {
                s_current_angle = 0;
            }
            custom_servo_set_angle((int16_t)s_current_angle);
        }
    }
    // ──────────────────────────────────────────────────────────────────────
}
